#pragma once

#if 1
class Scene;
class Shader;
class IReturnScene; // TEMP
class IReturnMonobehaviour;
class ITransferShader;
class IReturnGameObjects;
class GameObject;

struct GameDesc
{
	wstring appName = L"Portfolio";
	HINSTANCE hInstance = 0;
	HWND hWnd = NULL;
	float width = 800;
	float height = 600;

	bool vsync = false;
	bool windowed = true;
	Color clearColor = Color(0.f, 0.f, 0.f, 0.f);
	
	shared_ptr<IReturnMonobehaviour> MonobehaviourLoader;

	shared_ptr<IReturnScene> TempIntResourcePrefabScene; // TEMP

	
	shared_ptr<ITransferShader> resourceLoader; 
	shared_ptr<IReturnGameObjects> prefabLoader;

	shared_ptr<IReturnScene> sceneCodeLoader;



};




class Game
{
	//DECLARE_SINGLE(Game);
private:
	Game() {}

public:
	static Game* GetInstance()
	{
		static Game instance;
		return &instance;
	}

public:
	WPARAM Run(GameDesc& desc);

	const GameDesc& GetGameDesc() const { return _desc; }

	const GameState& GetGameState() const { return _gameState; }

	shared_ptr<Shader> GetShader() { return _shader; }

private:
	ATOM MyRegisterClass();
	BOOL InitInstance(int cmdShow);


	static LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
	
private:
	void InitGame();

public:
	void StartEditScene();
	void StartPlayScene();


private:

	void UpdateEditScene();

	void UpdatePlayScene();


	void SwitchScene(shared_ptr<Scene> Scene);

	/* Temp Edit Controlling Object Start */
private:
	// TEMP
	shared_ptr<GameObject> _editControllingObject; 
	float editControllingObjects_MovementSpeed = 30.f;
	float editControllingObjects_RotateSpeed = 40.f;
	
public:
	shared_ptr<GameObject> _editPointer;
	shared_ptr<GameObject> _editPointer_sphere;
	shared_ptr<GameObject> _editPointer_XAxis;
	shared_ptr<GameObject> _editPointer_YAxis;
	shared_ptr<GameObject> _editPointer_ZAxis_Reverse;

public:
	void SetEditControllingObject(shared_ptr<GameObject> go) { _editControllingObject = go; }
	void UpdateEditControllingObject();
	/* Temp Edit Controlling Object End */

public:
	shared_ptr<Scene> GetCurrentScene() { return _scene; }


private:
	shared_ptr<IReturnMonobehaviour> _monobehavioursLoader;

private:
	GameDesc _desc;
	shared_ptr<Shader> _shader;
	shared_ptr<Scene> _scene;
	int _sceneIndex = 0;
	GameState _gameState = GameState::Edit;

public:
	bool isGamePaused = false;
	bool isColliderRender = true;
	bool isPlaySceneCursorHided = true;
};
#endif





































































#if 0 // First
struct GameDesc
{
	shared_ptr<class IExecute> app = nullptr;
	wstring appName = L"Portfolio";
	HINSTANCE hInstance = 0;
	HWND hWnd = NULL;
	float width = 800;
	float height = 600;
	bool vsync = false;
	bool windowed = true;
	Color clearColor = Color(0.f, 0.f, 0.f, 0.f);
};


class Game
{
	//DECLARE_SINGLE(Game);
private:
	Game() {}

public:
	static Game* GetInstance()
	{
		static Game instance;
		return &instance;
	}


public:
	WPARAM Run(GameDesc& desc);

	GameDesc& GetGameDesc() { return _desc; }

private:
	ATOM MyRegisterClass();
	BOOL InitInstance(int cmdShow);

	void Update();

	static LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

private:
	GameDesc _desc;
};
#endif // First