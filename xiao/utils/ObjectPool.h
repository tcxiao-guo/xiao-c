/**
 * @file   ObjectPool.h
 * @author xiao guo
 * 
 *
 * @date   2024-5-26 
 */

#pragma once

#include <xiao/utils/NonCopyable.h>
#include <memory>
#include <mutex>
#include <vector>

BEGIN_NAMESPACE(xiao)

template<typename T>
class ObjectPool : public NonCopyable,
	public std::enable_shared_from_this<ObjectPool<T>>
{
public:
	std::shared_ptr<T> getObject()
	{
		static_assert(!std::is_pointer<T>::value,
			"The parameter type of the ObjectPool template can't be "
			"pointer type");
		T* p{ nullptr };
		{
			std::lock_guard<std::mutex> lock(mtx_);
			if (!objs_.empty())
			{
				p = objs_.back();
				objs_.pop_back();
			}
		}

		if (p == nullptr)
		{
			p = new T;
		}

		assert(p);
		std::weak_ptr<ObjectPool<T>> weakPtr = this->shared_from_this();
		auto obj = std::shared_ptr<T>(p, [weakPtr](T* ptr) {
			auto self = weakPtr.lock();
			if (self)
			{
				std::lock_guard<std::mutex> lock(self->mtx_);
				self->objs_.push_back(ptr);
			}
			else
			{
				delete ptr;
			}
			});
		return obj;
	}
private:
	std::mutex mtx_;
	std::vector<T*> objs_;
};

END_NAMESPACE(xiao)
