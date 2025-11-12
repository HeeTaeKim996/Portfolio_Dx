#pragma once

#if 1

/*
#define DECLARE_SINGLE(classname)			\
private:									\
	classname() { }							\
public:										\
	static classname* GetInstance()			\
	{										\
		static classname s_instance;		\
		return &s_instance;					\
	}
#define GET_SINGLE(classname)	classname::GetInstance()
*/


// Bone
#define MAX_MODEL_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500



#define CHECK(p)	assert(SUCCEEDED(p))
#define GAME		Game::GetInstance()		
#define GRAPHICS	Graphics::GetInstance()
#define DEVICE		GRAPHICS->GetDevice()
#define DC			GRAPHICS->GetDeviceContext()
#define INPUT		InputManager::GetInstance()
#define TIME		TimeManager::GetInstance()
#define DT			TIME->GetDeltaTime()
#define RESOURCES	ResourceManager::GetInstance()
#define RENDER		RenderManager::GetInstance()
#define IMGUI			ImGuiManager::GetInstance()
#define SCENE		SceneManager::GetInstance()
#define PREFAB		PrefabManager::GetInstance()
#define PHYSICS		Physics::GetInstance()
#define TIMER		TimerManager::GetInstance()

namespace tinyxml2
{
	class XMLDocument;
	class XMLElement;
}




enum class GameState
{
	Edit,
	Play,
};


enum class Layermask : uint8
{
	None = 0,
	Terrain = 1 << 0,
	Player = 1 << 1,
	Enemy = 1 << 2,

	End,
	ALL = (((uint8)Layermask::End - 1) << 1) - 1
};


#define PI 3.14159265358979323846f
#define INV_PI 0.31830988618379067154f
#define CONTACT_SLOP 1e-2
#define COS_45 0.70710678f
#define RAD_TO_DEG 57.29577951308232f
#define DEG_TO_RAD 0.017453292519943295f

/*---------------------
		 Flags
---------------------*/





/*---------------------
		 Address
---------------------*/
#define RESOURCE_FBX_PATH  "../Assets/Resources/FBXs/"

#define RESOURCE_MODEL_PATH "../Assets/Resources/Models/"

#define RESOURCE_TEXTURE_PATH "../Assets/Resources/Textures/"

#endif 














































































#if 0 // First
#define DECLARE_SINGLE(classname)			\
private:									\
	classname() {}							\
public:										\
	static classname* GetInstance()			\
	{										\
		static classname s_instance;		\
		return &s_instance;					\
	}

#define GET_SINGLE(classname)	classname::GetInstance()
#endif // First