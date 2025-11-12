#include "pch.h"
#include "PrefabManager.h"
#include "FileUtils.h"
#include "Scene.h"
#include "tinyxml2.h"
#include "GameObject.h"
#include "Utils.h"

PrefabManager::PrefabManager()
{
}

void PrefabManager::ObsolateInitialize()
{
	ObsolatePrefabs.clear();

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError error = doc.LoadFile("../Resources/Prefabs/prefabsXML.xml");
	assert(error == tinyxml2::XML_SUCCESS);

	tinyxml2::XMLElement* prefabsElement = doc.FirstChildElement();
	
	tinyxml2::XMLElement* goElement = prefabsElement->FirstChildElement("GameObject");
	while (goElement)
	{
		shared_ptr<GameObject> gameObject = make_shared<GameObject>();
		gameObject->LoadXML(doc, goElement, ObsolatePrefabs);

		goElement = goElement->NextSiblingElement("GameObject");
	}
}

void PrefabManager::ObsolateSavePrefab()
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLDeclaration* dec = doc.NewDeclaration();
	doc.LinkEndChild(dec);


	shared_ptr<Scene> scene = GAME->GetCurrentScene();

	tinyxml2::XMLElement* prefabElement = doc.NewElement("Prefabs");
	doc.LinkEndChild(prefabElement);

	vector<shared_ptr<GameObject>> rootObjects = scene->GetRootObjects();
	for (shared_ptr<GameObject> rootObject : rootObjects)
	{
		tinyxml2::XMLElement* objectElement = doc.NewElement("GameObject");
		prefabElement->LinkEndChild(objectElement);
		rootObject->WriteXML(doc, objectElement);
	}



	doc.SaveFile("../Resources/Prefabs/prefabsXML.xml");
}


void PrefabManager::Initialize(unordered_map<string, vector<shared_ptr<GameObject>>> loadedPrefabs)
{
	_prefabs = loadedPrefabs;
}

shared_ptr<GameObject> PrefabManager::GetPrefab(string key)
{
	auto it = _prefabs.find(key);
	if (it == _prefabs.end())
		assert(0);


	return it->second[0];
}

vector<shared_ptr<GameObject>> PrefabManager::GetAllPrefabs()
{
	vector<shared_ptr<GameObject>> gameObjects;

	for (pair<string, vector<shared_ptr<GameObject>>> p : _prefabs)
	{
		gameObjects.push_back(p.second[0]);
	}

	return gameObjects;
}


