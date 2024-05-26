/**
 * @file   ConcurrentTaskQueue.h
 * @author xiao guo
 * 
 *
 * @date   2024-5-26 
 */
#pragma once
#include <xiao/utils/TaskQueue.h>
#include <queue>

BEGIN_NAMESPACE(xiao)


/**
 * @brief This class implements a task queue running in parallel. Basically this
 * can be called a threads pool.
 */
class XIAO_EXPORT ConcurrentTaskQueue : public TaskQueue
{
public:
	ConcurrentTaskQueue(size_t threadNum, const std::string& name);

	virtual void runTaskInQueue(const std::function<void()>& task);
	virtual void runTaskInQueue(std::function<void()>&& task);

	virtual std::string getName() const
	{
		return queueName_;
	}

	size_t getTaskCount();

	void stop();

	~ConcurrentTaskQueue();

private:
	std::string queueName_;
	size_t queueCount_;

	std::atomic_bool stop_;

	std::queue<std::function<void()>> taskQueue_;
	std::vector<std::thread> threads_;
	void queueFunc(int queueNum);

	std::mutex taskMutex_;
	std::condition_variable taskCond_;
};


END_NAMESPACE(xiao)
