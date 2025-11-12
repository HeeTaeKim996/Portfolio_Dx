#pragma once


class TimerActor;
class Timer;

#include <queue>

struct TimerData
{
	TimerData(weak_ptr<TimerActor> InOwner, shared_ptr<Timer> InTimer) : owner(InOwner), timer(InTimer) {}


	weak_ptr<TimerActor> owner;
	shared_ptr<Timer> timer;
};

struct TimerItem
{
	bool operator < (const TimerItem& other) const
	{
		return executeTick > other.executeTick;
	}

	uint64 executeTick = 0;
	TimerData* timerData = nullptr;
};



class TimerManager
{
private:
	TimerManager() {}
public:
	static TimerManager* GetInstance()
	{
		static TimerManager instance;
		return &instance;
	}
public:
	void Reserve(uint64 tickAfter, weak_ptr<TimerActor> actor, shared_ptr<Timer> timer);
	void Update();
	void Clear();

private:
	priority_queue<TimerItem> _items;
};

