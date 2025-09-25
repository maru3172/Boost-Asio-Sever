#pragma once

struct JobData
{
	JobData(std::weak_ptr<JobQueue> owner, JobRef job) : owner(owner), job(job) {}
	std::weak_ptr<JobQueue> owner;
	JobRef job;
};

struct TimerItem
{
	bool operator<(const TimerItem& other) const { return executeTick > other.executeTick; }

	uint64 executeTick = 0;
	JobData* jobData = nullptr;
};

class JobTimer
{
	USE_LOCK;
	PriorityQueue<TimerItem> _items;
	Atomic<bool> _distributing = false;
public:
	void Reserve(uint64 tickAfter, std::weak_ptr<JobQueue> owner, JobRef job);
	void Distribute(uint64 now);
	void Clear();
};

