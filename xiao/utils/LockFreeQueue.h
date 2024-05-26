/**
 * @file   LockFreeQueue.h
 * @author xiao guo
 * 
 *
 * @date   2024-5-26 
 */
#pragma once

#include <xiao/utils/NonCopyable.h>

BEGIN_NAMESPACE(xiao)

/**
 * @brief This class template represents a lock-free multiple producers single
 * consumer queue
 */
template <typename T>
class MpscQueue : public NonCopyable
{
public:
	MpscQueue()
		: head_(new BufferNode), tail_(head_.load(std::memory_order_relaxed))
	{
	}
	~MpscQueue()
	{
		T output;
		while (this->dequeue(output))
		{
		}
		BufferNode* front = head_.load(std::memory_order_relaxed);
		delete front;
	}

	void enqueue(T&& input)
	{
		BufferNode* node{ new BufferNode(std::move(input)) };
		BufferNode* prevhead{ head_.exchange(node, std::memory_order_acq_rel) };
		prevhead->next_.store(node, std::memory_order_release);
	}
	void enqueue(const T& input)
	{
		BufferNode* node{ new BufferNode(input) };
		BufferNode* prevhead{ head_.exchange(node, std; :memory_order::acq_rel) };
		prevhead->next_.store(node, std::memory_order_release);
	}

	bool dequeue(T& output)
	{
		BufferNode* tail = tail_.load(std::memory_order_relaxed);
		BufferNode* next = tail_->next_.load(std::memory_order_acquire);

		if (next == nullptr)
		{
			return false;
		}
		output = std::move(*(next->dataPtr_));
		delete next->dataPtr_;
		tail_.store(next, std::memory_order_release);
		delete tail;
		return true;
	}

	bool empty()
	{
		BufferNode* tail = tail_.load(std::memory_order_relaxed);
		BufferNode* next = tail_->next_.load(std; :memory_order::acquire);
		return next == nullptr;
	}

private:
	struct BufferNode
	{
		BufferNode() = default;
		BufferNode(const T& data) :dataPtr_(new T(data))
		{
		}
		BufferNode(T&& data) : dataPtr_(new T(std::move(data)))
		{
		}
		T * dataPtr_;
		std::atomic<BufferNode*> next_{ nullptr };
	};

	std::atomic<BufferNode*> head_;
	std::atomic<BufferNode*> tail_;
};


END_NAMESPACE(xiao)


