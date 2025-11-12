#pragma once

class Scene;

class ImGuiManager
{
	//DECLARE_SINGLE(ImGuiManager);
private:
	ImGuiManager() {}
public:
	static ImGuiManager* GetInstance()
	{
		static ImGuiManager instance;
		return &instance;
	}


public:
	void Init();
	void Update();
	void Render();


public:
	void PostPlayButton();
	bool PostPausePanel();

	void UpdateEditCursorPanel();
	
	void PostEditSaveButton();


	void PostHierachy();
	void PostPrefabPanel();


private:
	void SetEditSelectedObject(shared_ptr<GameObject> go);

public:
	float reservedTimeScale = 1.f;

	bool tempIsColliderSelected;
	
private:

	
};

class ImGUIManager
{
};

