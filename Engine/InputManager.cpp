#include "pch.h"
#include "InputManager.h"
#include "Game.h"

#if 1
void InputManager::Init(HWND hwnd)
{
	_hwnd = hwnd;
	_states.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);
}

void InputManager::Update()
{
	HWND hwnd = ::GetActiveWindow();
	if (_hwnd != hwnd)
	{
		for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
			_states[key] = KEY_STATE::NONE;

		return;
	}

	BYTE asciiKeys[KEY_TYPE_COUNT] = {};
	if (::GetKeyboardState(asciiKeys) == false)
		return;
	// ※ BYTE[256] 변환에서 보듯이, 아스키코드와 유사한 크기의, VIRTUAL_KEY_CODE 를 사용(LBUTTON, RBUTTON 도 인식)	
	

	for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
	{
		// 키가 눌려 있으면 true
		if (asciiKeys[key] & 0x80)
		{
			KEY_STATE& state = _states[key];

			// 이전 프레임에 키를 누른 상태라면 PRESS
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::PRESS;
			else
				state = KEY_STATE::DOWN;
		}
		else
		{
			KEY_STATE& state = _states[key];

			// 이전 프레임에 키를 누른 상태라면 UP
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::UP;
			else
				state = KEY_STATE::NONE;
		}
	}



	

	::GetCursorPos(&_mousePos);
	::ScreenToClient(_hwnd, &_mousePos);


	if (_isCursorHided)
	{
		const GameDesc& gameDesc = GAME->GetGameDesc();

		POINT center;
		center.x = static_cast<int32>(gameDesc.width * 0.5f);
		center.y = static_cast<int32>(gameDesc.height * 0.5f);


		_cursorMoveVec.x = _mousePos.x - center.x;
		_cursorMoveVec.y = _mousePos.y - center.y;



		::ClientToScreen(_hwnd, &center);


		::SetCursorPos(center.x, center.y);
	}
}

void InputManager::SetCursorState(bool isCursorHided)
{
	_isCursorHided = isCursorHided;

	if (isCursorHided)
	{
		const GameDesc& gameDesc = GAME->GetGameDesc();
		POINT center;
		center.x = static_cast<int32>(gameDesc.width * 0.5f);
		center.y = static_cast<int32>(gameDesc.height * 0.5f);
		::ClientToScreen(_hwnd, &center);

		::SetCursorPos(center.x, center.y);


		while (ShowCursor(false) >= 0);
	}
	else
	{
		while (ShowCursor(true) < 0);
	}

}

#endif










































































#if 0 // First
void InputManager::Init(HWND hwnd)
{
	_hwnd = hwnd;
	_states.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);
}

void InputManager::Update()
{
	HWND hwnd = ::GetActiveWindow();
	if (_hwnd != hwnd)
	{
		for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
		{
			_states[key] = KEY_STATE::NONE;
		}

		return;
	}

	BYTE asciKeys[KEY_TYPE_COUNT] = {};
	if (::GetKeyboardState(asciKeys) == false)
	{
		return;
	}

	for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
	{
		if (asciKeys[key] & 0x80)
		{
			KEY_STATE& state = _states[key];

			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
			{
				state = KEY_STATE::PRESS;
			}
			else
			{
				state = KEY_STATE::DOWN;
			}
		}
		else
		{
			KEY_STATE& state = _states[key];

			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
			{
				state = KEY_STATE::UP;
			}
			else
			{
				state = KEY_STATE::NONE;
			}
		}
	}

	::GetCursorPos(&_mousePos);
	::ScreenToClient(_hwnd, &_mousePos);
}
#endif // First