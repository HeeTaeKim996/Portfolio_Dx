#pragma once

class Scene;
class IReturnScene;

class SceneManager
{
private:
	SceneManager() {}
public:
	static SceneManager* GetInstance()
	{
		static SceneManager instance;
		return &instance;
	}

public:
	const shared_ptr<Scene>& const GetSceneByIndex(int index);


	
	shared_ptr<Scene> Initialize(bool isHardCodeLoad);
	shared_ptr<Scene> ObsolateInitialize();


	void SaveScene();
private:
	string path = "../Assets/Scenes/Scenes.xml";
	vector<shared_ptr<Scene>> _scenes;
};

