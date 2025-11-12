#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"
#include "PrefabManager.h"
#include "Game.h"
#include "InitResource.h"
#include "tinyxml2.h"


const shared_ptr<Scene>& SceneManager::GetSceneByIndex(int index)
{
	assert(_scenes.size() > index);

	return _scenes[index];
}



shared_ptr<Scene> SceneManager::Initialize(bool isHardCodeLoad)
{
	if (isHardCodeLoad)
	{
		_scenes = GAME->GetGameDesc().sceneCodeLoader->Init(GAME->GetShader());

		return _scenes[0];
	}
	else
	{
		XmlDocument doc;
		tinyxml2::XMLError error = doc.LoadFile(path.c_str());
		assert(error == tinyxml2::XML_SUCCESS);

		XmlElement* rootElement = doc.FirstChildElement("ScenesData");
		
		XmlElement* sceneElement = rootElement->FirstChildElement("Scene");
		while (sceneElement)
		{
			shared_ptr<Scene> scene = make_shared<Scene>();
			scene->LoadScene(doc, sceneElement);

			



			{ 

				scene->_skyObject = PREFAB->GetPrefab("Sky_Space");
			}






			_scenes.push_back(scene);

			sceneElement = sceneElement->NextSiblingElement("Scene");
		}




		return _scenes[0];
	}
}

shared_ptr<Scene> SceneManager::ObsolateInitialize()
{
	shared_ptr<Scene> scene = make_shared<Scene>();
	PREFAB->ObsolateInitialize();
	vector<shared_ptr<GameObject>> gameObjects = PREFAB->ObsolateGetPrefabs();
	for (shared_ptr<GameObject> gameObject : gameObjects)
	{
		scene->AddGameObject(gameObject);
	}
	_scenes.push_back(scene);
	return scene;


}

void SceneManager::SaveScene()
{
	XmlDocument doc;
	tinyxml2::XMLDeclaration* dec = doc.NewDeclaration();
	doc.LinkEndChild(dec);

	XmlElement* root = doc.NewElement("ScenesData");
	doc.LinkEndChild(root);

	for (shared_ptr<Scene> scene : _scenes)
	{
		XmlElement* sceneElement = doc.NewElement("Scene");
		root->LinkEndChild(sceneElement);
		scene->SaveScene(doc, sceneElement);
	}

	doc.SaveFile(path.c_str());

}
