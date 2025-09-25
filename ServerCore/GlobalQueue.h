#pragma once

class GlobalQueue
{
	LockQueue<JobQueueRef> _jobQueues;
public:
	GlobalQueue();
	~GlobalQueue();

	void Push(JobQueueRef jobQueue);
	JobQueueRef Pop();
};

