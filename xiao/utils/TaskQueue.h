/**
 * @file   TaskQueue.h
 * @author xiao guo
 * 
 *
 * @date   2024-5-26 
 */
#pragma once

#include "NonCopyable.h"
#include <functional>
#include <string>
#include <future>

BEGIN_NAMESPACE(xiao)

class TaskQueue : public NonCopyable
{
public:
	virtual void runTaskInQueue(const std::function<void()>& task) = 0;
	virtual void runTaskInQueue(std::function<void()>&& task) = 0;
	virtual std::string getName() const
	{
		return "";
	};

	/**
	 * @brief Run a task in the queue sychronously. This means that the task is 
	 * executed before the method returns.
	 */
	void syncTaskInQueue(const std::function<void()>& task)
	{
		std::promise<int> prom;
		std::future<int> fut = prom.get_future();
		runTaskInQueue([&]() {
			task();
			prom.set_value(1);
			});
		fut.get();
	};
	virtual ~TaskQueue()
	{
	}


};

END_NAMESPACE(xiao)
