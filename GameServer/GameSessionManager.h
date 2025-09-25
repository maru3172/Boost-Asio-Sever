#pragma once

class GameSession;

using GameSessionRef = std::shared_ptr<GameSession>;

class GameSessionManager
{
	USE_LOCK;
	std::set<GameSessionRef> _sessions;
public:
	void Add(GameSessionRef session);
	void Remove(GameSessionRef session);
	void Broadcast(SendBufferRef sendBuffer);
};

extern GameSessionManager GSessionManager;
