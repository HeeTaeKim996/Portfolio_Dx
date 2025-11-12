#include "pch.h"
#include "SceneCodeLoader.h"
#include "Engine/PrefabManager.h"
#include "Engine/GameObject.h"
#include "Engine/Scene.h"

vector<shared_ptr<Scene>> SceneCodeLoader::Init(shared_ptr<Shader> shader)
{
	vector<shared_ptr<Scene>> scenes;

	shared_ptr<Scene> firstScene = make_shared<Scene>();
	firstScene->SetSceneName("FirstScene");
	scenes.push_back(firstScene);

	firstScene->Instantiate("Camera");

	firstScene->Instantiate("Kachujin_OnlyModel");

	shared_ptr<GameObject> kachu = firstScene->Instantiate("Kachujin_Animation");
	kachu->GetTransform()->AddPosition(Vec3(50.f, -4.f, -4.f));

	firstScene->Instantiate("Kachujin_Animation");



	firstScene->Instantiate("SunErathMoon");

	return scenes;
}
