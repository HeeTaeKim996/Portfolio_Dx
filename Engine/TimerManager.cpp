#include "pch.h"
#include "TimerManager.h"

#include "Timer.h"
#include "TimerActor.h"


void TimerManager::Reserve(uint64 tickAfter, weak_ptr<TimerActor> actor, shared_ptr<Timer> timer)
{
	const uint64 executeTick = ::GetTickCount64() + tickAfter;
	TimerData* timerData = new TimerData(actor, timer);

	_items.push(TimerItem{ executeTick, timerData });
}

void TimerManager::Update()
{
	uint64 now = ::GetTickCount64();

	while (!_items.empty() && now >= _items.top().executeTick)
	{
		TimerItem timerItem = _items.top();
		_items.pop();

		timerItem.timerData->timer->Execute();
		delete timerItem.timerData;
	}
}

void TimerManager::Clear()
{
	while (!_items.empty())
	{
		TimerItem timerItem = _items.top();
		_items.pop();
		delete timerItem.timerData;
	}
}
