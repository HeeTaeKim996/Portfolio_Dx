#pragma once

#include "Timer.h"

#include "TimerManager.h"

#include <utility>

class TimerActor : public enable_shared_from_this<TimerActor>
{
public:
	inline void DoTimer(uint64 tickAfter, CallbackType&& callback)
	{
		shared_ptr<Timer> timer = make_shared<Timer>(std::move(callback));
		TimerManager::GetInstance()->Reserve(tickAfter, shared_from_this(), timer);
	}

	template<typename T, typename Ret, typename... Args>
	inline void DoTimer(uint64 tickAfter, Ret(T::* memFunc)(Args...), Args... args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());

		shared_ptr<Timer> timer = make_shared<Timer>(owner, memFunc, std::forward<Args>(args)...);

		TimerManager::GetInstance()->Reserve(tickAfter, shared_from_this(), timer);
	}
};

