#pragma once
#include <thread>
#include <functional>

class ThreadManager
{
	Mutex _lock;
	std::vector<std::thread> _threads;
public:
	ThreadManager();
	~ThreadManager();

	void Launch(std::function<void(void)> callback);
	void Join();

	static void InitTLS();
	static void DestroyTLS();

	static void DoGlobalQueueWork();
	static void DistributeReservedJobs();
};