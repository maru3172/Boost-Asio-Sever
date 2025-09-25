#pragma once
#include <stack>
#include <map>
#include <vector>

class DeadLockProfiler
{
	std::unordered_map<const char*, int32> _nameToId;
	std::unordered_map<int32, const char*> _idToName;
	std::map<int32, std::set<int32>> _lockHistory;

	Mutex _lock;

	std::vector<int32> _discoveredOrder;
	int32 _discoveredCount = 0;
	std::vector<bool> _finished;
	std::vector<int32> _parent;

	void Dfs(int32 index);
public:
	void PushLock(const char* name);
	void PopLock(const char* name);
	void CheckCycle();
};