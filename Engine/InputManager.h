#pragma once


#if 1
enum class KEY_TYPE
{
	UP = VK_UP,
	DOWN = VK_DOWN,
	LEFT = VK_LEFT,
	RIGHT = VK_RIGHT,

	W = 'W',
	A = 'A',
	S = 'S',
	D = 'D',

	Q = 'Q',
	E = 'E',
	Z = 'Z',
	C = 'C',

	O = 'O',
	P = 'P',

	KEY_1 = '1',
	KEY_2 = '2',
	KEY_3 = '3',
	KEY_4 = '4',
	KEY_5 = '5',
	KEY_6 = '6',

	LBUTTON = VK_LBUTTON,
	RBUTTON = VK_RBUTTON,

	NUMPAD_0 = VK_NUMPAD0,
	NUMPAD_1 = VK_NUMPAD1,
	NUMPAD_2 = VK_NUMPAD2,
	NUMPAD_3 = VK_NUMPAD3,
	NUMPAD_4 = VK_NUMPAD4,
	NUMPAD_5 = VK_NUMPAD5,
	NUMPAD_6 = VK_NUMPAD6,
	NUMPAD_7 = VK_NUMPAD7,
	NUMPAD_8 = VK_NUMPAD8,
	NUMPAD_9 = VK_NUMPAD9,

	ESCAPE = VK_ESCAPE,
	SPACE = VK_SPACE,
	
};

enum class KEY_STATE
{
	NONE,
	PRESS,
	DOWN,
	UP,
	END
};

enum
{
	KEY_TYPE_COUNT = static_cast<int32>(UINT8_MAX + 1),
	KEY_STATE_COUNT = static_cast<int32>(KEY_STATE::END),
};

class InputManager
{
	//DECLARE_SINGLE(InputManager);
private:
	InputManager() {}
public:
	static InputManager* GetInstance()
	{
		static InputManager instance;
		return &instance;
	}

public:
	void Init(HWND hwnd);
	void Update();

	
	bool GetButton(KEY_TYPE key) { return GetState(key) == KEY_STATE::PRESS; }
	
	bool GetButtonDown(KEY_TYPE key) { return GetState(key) == KEY_STATE::DOWN; }
	
	bool GetButtonUp(KEY_TYPE key) { return GetState(key) == KEY_STATE::UP; }

	void SetCursorState(bool isCursorHided);
	const POINT& GetMousePos() { return _mousePos; }
	Vec2 GetCursorMoveVec() { return _cursorMoveVec; }
	bool IsCursorHided() { return _isCursorHided; }
private:
	inline KEY_STATE GetState(KEY_TYPE key) { return _states[static_cast<uint8>(key)]; }

private:
	HWND _hwnd;
	vector<KEY_STATE> _states;
	POINT _mousePos = {};

	Vec2 _cursorMoveVec;
	bool _isCursorHided = false;
};
#endif