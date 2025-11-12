#include "pch.h"
#include "Scene.h"
#include "Transform.h"
#include "GameObject.h"
#include "Component.h"
#include "Camera.h"
#include "ModelRenderer.h"
#include "MeshRenderer.h"
#include "ModelAnimator.h"
#include "PrefabManager.h"
#include "tinyxml2.h"
#include "Transform.h"

#include "SphereCollider.h"
#include "BoxCollider.h"
#include "InputManager.h"

Scene::Scene()
{
	_gameObjects.resize(10);
	_circDbLinkedList = make_shared<CircDbLinkedList>();
}

Scene::~Scene()
{
	int i = 0; // DEBUG
}




void Scene::Awake()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		if (!gameObject)
			continue;
		gameObject->Awake();
	}
}

void Scene::Start()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		if (!gameObject)
			continue;
		gameObject->Start();
	}
}

void Scene::Update()
{
	Camera::readyRender = false;

	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		if (!gameObject)
			continue;
		gameObject->Update();
	}

	if (INPUT->GetButtonDown(KEY_TYPE::O))
	{
		GAME->isColliderRender = !GAME->isColliderRender;
	}

}

void Scene::LateUpdate()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		if (!gameObject)
			continue;
		gameObject->LateUpdate();
	}
}

void Scene::FixedUpdate()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		if (!gameObject)
			continue;
		gameObject->FixedUpdate();
	}
}

void Scene::Render()
{
	if (!Camera::readyRender)
		return;

	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		if (!gameObject)
			continue;
		gameObject->Render();

		if (GAME->isColliderRender)
		{
			if (shared_ptr<BaseCollider> baseCollider
				= static_pointer_cast<BaseCollider>(gameObject->GetFixedComponent(ComponentType::Collider)))
			{
				if (baseCollider->IsEnabled())
				{
					baseCollider->Render();
				}
			}
		}
	}

	_skyObject->SetRenderIndex(ComponentType::MeshRenderer);
	_skyObject->Render();
}

void Scene::OnEnable()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		if (!gameObject)
			continue;
		gameObject->OnEnable();
	}
}

void Scene::OnDisable()
{
	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		if (!gameObject)
			continue;
		gameObject->OnDisable();
	}
}





void Scene::EditUpdate()
{
	Camera::readyRender = false;

	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		if (!gameObject)
			continue;
		
		if (shared_ptr<Camera> camera
			= static_pointer_cast<Camera>(gameObject->GetFixedComponent(ComponentType::Camera)))
		{
			camera->Update();

			GAME->SetEditControllingObject(gameObject);
		}

		if (shared_ptr<BaseCollider> collider = gameObject->GetCollider())
		{
			collider->UpdateColliderTransform(gameObject->GetTransform());
		}
	}


	if (INPUT->GetButtonDown(KEY_TYPE::O))
	{
		GAME->isColliderRender = !GAME->isColliderRender;
	}
}


void Scene::EditRender()
{
	if (!Camera::readyRender)
		return;

	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		if (!gameObject)
			continue;
		gameObject->EditRender();

		if (GAME->isColliderRender)
		{
			if (shared_ptr<BaseCollider> baseCollider
				= static_pointer_cast<BaseCollider>(gameObject->GetFixedComponent(ComponentType::Collider)))
			{
				baseCollider->Render();
			}
		}
	}

	_skyObject->SetRenderIndex(ComponentType::MeshRenderer);
	_skyObject->Render();
}





void Scene::AddGameObject(shared_ptr<GameObject> gameObject)
{


	if (!_freeIndexList.empty())
	{
		uint32 id = _freeIndexList.back();
		_freeIndexList.pop_back();

		uint8 useCount = (id >> 24) + 1;
		uint32 index = id & 0x00'FFFFFF;

		id = (useCount << 24) + index;
		gameObject->SetId(id);

		string prefabName = gameObject->GetName();


		gameObject->SetName(prefabName + "_" + std::to_string(useCount) + "_" + std::to_string(index));


		_gameObjects[index] = gameObject;
	}
	else
	{
		if (_gameObjectAddingIndex >= _gameObjects.size())
		{
			_gameObjects.resize(_gameObjects.size() + 10);
		}

		assert(_gameObjectAddingIndex <= (1 << 24));


		uint32 id = _gameObjectAddingIndex;
		gameObject->SetId(id);


		string prefabName = gameObject->GetName();
		gameObject->SetName(prefabName + "_" + std::to_string(0) + "_" + std::to_string(id));
		

		_gameObjects[id] = gameObject;


		_gameObjectAddingIndex++;
	}

	gameObject->SetScene(shared_from_this());

	shared_ptr<Transform> transform = gameObject->GetTransform();
	if (transform)
	{
		if (!transform->HasParent())
		{
			CircDbNode* node = _circDbLinkedList->InsertRear(gameObject->GetId());
			gameObject->SetCircDbNode(node);
		}
	}

	if (GAME->GetGameState() == GameState::Play)
	{
		gameObject->Awake();
		gameObject->OnEnable();
		gameObject->Start();
	}
}

void Scene::DestroyGameObject(shared_ptr<GameObject> gameObject)
{
	uint32 id = gameObject->GetId();
	uint32 useCount = id >> 24;
	uint32 index = id & 0x00'FF'FF'FF;
#ifdef _DEBUG
	assert(_gameObjects[index] == gameObject);
#endif



	gameObject->Destroy();

	_gameObjects[index] = nullptr;
	_freeIndexList.push_back(id);

	if (gameObject->HasCircDbNode())
	{
		CircDbNode* node = gameObject->GetCircDbNode();
		_circDbLinkedList->Remove(node);
	}

	shared_ptr<Transform> transform = gameObject->GetTransform();
	if (transform)
	{
		vector<Transform*> childrenTransform = transform->GetChildren();
		for (Transform* childTransform : childrenTransform)
		{
			shared_ptr<GameObject> gameObject = childTransform->GetGameObject();
			DestroyGameObject(gameObject);
		}
	}
}

void Scene::AddRootObject(shared_ptr<GameObject> gameObject)
{
#ifdef _DEBUG
	assert(!gameObject->HasCircDbNode() && !gameObject->GetTransform()->HasParent());
#endif

	CircDbNode* node = _circDbLinkedList->InsertRear(gameObject->GetId());
	gameObject->SetCircDbNode(node);
}

void Scene::RemoveRootObject(shared_ptr<GameObject> gameObject)
{
	if (!gameObject->HasCircDbNode())
		return;

	CircDbNode* node = gameObject->GetCircDbNode();
	gameObject->RemoveCircDbNode();

	_circDbLinkedList->Remove(node);
}




vector<shared_ptr<GameObject>> Scene::GetRootObjects()
{
	vector<shared_ptr<GameObject>> gameObjects;

	CircDbNode* node = _circDbLinkedList->GetFirstNode();

	while (_circDbLinkedList->IsEnd(node) == false)
	{
		uint32 index = node->id & 0x00'FF'FF'FF;
		shared_ptr<GameObject> gameObject = _gameObjects[index];
		gameObjects.push_back(gameObject);

		node = node->next;
	}

	return gameObjects;
}

shared_ptr<Scene> Scene::Clone()
{

	shared_ptr<Scene> copy = make_shared<Scene>();


	vector<shared_ptr<GameObject>> rootObjects = GetRootObjects();
	for (shared_ptr<GameObject> gameObject : rootObjects)
	{
		assert(gameObject != nullptr);

		vector<shared_ptr<GameObject>> reserveObjects;
		gameObject->Clone(copy, reserveObjects);

		for (shared_ptr<GameObject> reserveObject : reserveObjects)
		{
			copy->AddGameObject(reserveObject);
		}
	}

	vector<shared_ptr<GameObject>> nullObjects;
	copy->_skyObject = _skyObject->Clone(copy, nullObjects);


	return copy;
}

shared_ptr<GameObject> Scene::Instantiate(string prefabKey)
{
	shared_ptr<GameObject> prefab = PREFAB->GetPrefab(prefabKey);

	vector<shared_ptr<GameObject>> reserveObjects;
	prefab->Clone(shared_from_this(), reserveObjects);


	{ // DEBUG

		//reserveObjects.size() > 1;
	}


	for (shared_ptr<GameObject> reserveObject : reserveObjects)
	{
		AddGameObject(reserveObject);
	}

	reserveObjects[0]->SetPrefabName(prefabKey);

	return reserveObjects[0];
}

shared_ptr<GameObject> Scene::Instantiate(string prefabKey, Vec3 position)
{
	shared_ptr<GameObject> gameObject = Instantiate(prefabKey);
	gameObject->GetOrAddTransform()->SetPosition(position);

	return gameObject;
}

void Scene::SaveScene(XmlDocument& doc, XmlElement* parent)
{
	XmlElement* sceneNameElement = doc.NewElement("Name");
	parent->LinkEndChild(sceneNameElement);
	sceneNameElement->SetText(_sceneName.c_str());

	XmlElement* gameObjectsElement = doc.NewElement("GameObjects");
	parent->LinkEndChild(gameObjectsElement);

	XmlElement* gameObjectElement;

	for (shared_ptr<GameObject> gameObject : _gameObjects)
	{
		if (!gameObject)
			continue;
		string prefabName = gameObject->GetPrefabName();
		if (prefabName == "")
			continue;

		gameObjectElement = doc.NewElement("GameObject");
		gameObjectsElement->LinkEndChild(gameObjectElement);

		XmlElement* prefabNameElement = doc.NewElement("PrefabName");
		gameObjectElement->LinkEndChild(prefabNameElement);

		prefabNameElement->SetText(prefabName.c_str());

		shared_ptr<Transform> transform = gameObject->GetTransform();
		if (transform)
		{
			transform->WriteXML(doc, gameObjectElement);
		}
	}

}

void Scene::LoadScene(XmlDocument& doc, XmlElement* parent)
{
	XmlElement* sceneNameElement = parent->FirstChildElement("Name");
	_sceneName = sceneNameElement->GetText();


	XmlElement* gameObjectsElement = parent->FirstChildElement("GameObjects");

	XmlElement* goElement = gameObjectsElement->FirstChildElement("GameObject");
	while (goElement)
	{
		XmlElement* prefabNameElemnt = goElement->FirstChildElement("PrefabName");

		assert(prefabNameElemnt);

		string prefabName = prefabNameElemnt->GetText();


		shared_ptr<GameObject> go = Instantiate(prefabName);


		XmlElement* transformElement = goElement->FirstChildElement("Transform");
		if (transformElement)
		{
			shared_ptr<Transform> loadTransform = make_shared<Transform>();
			loadTransform->LoadXML(doc, transformElement);

			shared_ptr<Transform> transform = go->GetTransform();


			transform->SetScale(loadTransform->GetScale());
			transform->SetRotation(loadTransform->GetRotation());
			transform->SetPosition(loadTransform->GetPosition());
		}

		goElement = goElement->NextSiblingElement("GameObject");
	}

}

shared_ptr<Camera> Scene::GetCamera()
{
	shared_ptr<Camera> camera; 
	for (shared_ptr<GameObject> gameObject : _gameObjects)
	{
		shared_ptr<Camera> goCamera 
			= static_pointer_cast<Camera>(gameObject->GetFixedComponent(ComponentType::Camera));
		if (goCamera)
			camera = goCamera;
	}

	assert(camera);

	return camera;
}


shared_ptr<GameObject> Scene::RayPick(int32 screenX, int32 screenY)
{	
#if 0 // Obsolate
	const GameDesc& gameDesc = GAME->GetGameDesc();

	float width = gameDesc.width;
	float height = gameDesc.height;
	
	float d_a = Camera::S_MatProjection[0][0];
	float d = Camera::S_MatProjection[1][1];

	float viewX = (+2.0f * screenX / width - 1.0f) / d_a;
	float viewY = (-2.0f * screenY / height + 1.0f) / d;
	

	float minDistance = FLT_MAX;
	shared_ptr<GameObject> picked;
	for (shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		if (!gameObject)
			continue;
		if (!gameObject->GetCollider())
			continue;

		
		SimpleVec4 rayOrigin = SimpleVec4(0.f, 0.f, 0.f, 1.f); 
		SimpleVec4 rayEnd = SimpleVec4(viewX, viewY, 1.0f, 0.f);

		
		SimpleMatrix vInv = MyMathUtils::MyMatrixToSimpleMatrix(Camera::S_MatVInv);

		SimpleVec3 worldRayOrigin = XMVector3TransformCoord(rayOrigin, vInv);
		SimpleVec3 worldRayDir = XMVector3TransformNormal(rayEnd, vInv);
		

		myVec3 myRayOrigin = MyMathUtils::SimpleVec3ToMyVec3(worldRayOrigin);
		myVec3 myNormalizedRayDir = MyMathUtils::SimpleVec3ToMyVec3(worldRayDir);
		myNormalizedRayDir.Normalize();


		float distance = 0.f;
		if (gameObject->GetCollider()->Intersects_AssumeNormal(myRayOrigin, myNormalizedRayDir, OUT distance) == false)
			continue;

		if (distance < minDistance)
		{
			minDistance = distance;
			picked = gameObject;
		}
	}

	return picked;
#endif
	
	const GameDesc& gameDesc = GAME->GetGameDesc();

	float width = gameDesc.width;
	float height = gameDesc.height;


	float tempNear = 1.f;
	float tempFar = 1000.f;
	
	myRay ray(width, height, tempNear, tempFar, Camera::S_MatVInv, Camera::S_MatProjection, screenX, screenY);
	float minDistance = FLT_MAX;
	shared_ptr<GameObject> picked;

	for (shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		if (!gameObject)
			continue;
		if (!gameObject->GetCollider())
			continue;
		
		float distance = 0.f;
		if (gameObject->GetCollider()->Intersects(ray, distance) == false)
			continue;

		if (distance < minDistance)
		{
			minDistance = distance;
			picked = gameObject;
		}	
	}

	return picked;
}

vector<shared_ptr<BaseCollider>> Scene::GetColliders()
{
	vector<shared_ptr<BaseCollider>> colliders;

	for (shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		if (!gameObject)
			continue;
		if (shared_ptr<BaseCollider> collider = gameObject->GetCollider())
			colliders.push_back(collider);
	}

	return colliders;
}




