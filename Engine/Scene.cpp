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

	_skyObject->SetRenderIndex(ComponentType::MeshRenderer); // 클로닝이랑 로딩 때 모두 처리하기 귀찮아서, 여기 배치
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





void Scene::EditUpdate() // 임시로 이렇게 처리
{
	Camera::readyRender = false;

	for (const shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		if (!gameObject)
			continue;
		
		if (shared_ptr<Camera> camera
			= static_pointer_cast<Camera>(gameObject->GetFixedComponent(ComponentType::Camera)))
		{
			camera->Update(); // GameObjects 중 마지막 카메라가 렌더링

			GAME->SetEditControllingObject(gameObject);
		}

		if (shared_ptr<BaseCollider> collider = gameObject->GetCollider())
		{
			collider->UpdateColliderTransform(gameObject->GetTransform()); // ※ 임시 Edit 에서 트랜스폼 업데이트 목적
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

	_skyObject->SetRenderIndex(ComponentType::MeshRenderer); // 클로닝이랑 로딩 때 모두 처리하기 귀찮아서, 여기 배치
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
		//TCHAR msg[256];
		//wsprintf(msg, L"%d", id);
		//MessageBox(NULL, NULL, msg, MB_OK);
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

	if (GAME->GetGameState() == GameState::Play) // ※ 씬 복사에는 발동하면 안되므로
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


	/*
		■ Scene, GameObject, Transform, CircularDoublyLinkedList 과 클로닝 관련 정리

		 - 위 4 개와 연관된 개념을 한번에 정리

		 1) Scene 은 루트 오브젝트 뿐만 아닌 모든 오브젝트를 관리한다
		  - Scene 은 모든 오브젝트를 컨테이너로 들고 있고, Update 등에서 컨테이너 순서대로 호출하는 게 구조가 깔끔하다
			( 상용 엔진도 이런 방식을 사용한다 함 )

		 2) 씬을 클로닝 할 때에는, 오브젝트 컨테이너를 순회하며 클로닝 하면 안되고, 루트 오브젝트에서 자식 오브젝트로 내려오는
			구조로 클로닝해야 한다
		  - Scene 을 통째로 카피하는 make_shared<Scene>(*this); 를 하는 건, 아무 의미 없다. shared_ptr<> 로 관리되기 때문에,
			복사가 아닌 원본 주소를 복사하는 꼴이기 때문
		  - 만약 자식 오브젝트가 컨테이너에서 부모 오브젝트보다 앞서 있다면, 자식 오브젝트의 부모로 설정되는 오브젝트는
			클로닝 부모가 아닌 원래 부모가 되어 꼬인다
		  - 따라서, 게임 오브젝트들을 Clone 코드로 수동 복사하되,
			(게임 오브젝트 뿐만 아닌 컴퍼넌트 및 모노비헤비어 상속 클래스들도..)
			루트 오브젝트에서 자식 오브젝트로 내려오는 순서로 복사 필요
		  - 이를 위해 자료구조인 CircularDoublyLinkedList 를 활용하여, RootObject 들을 따로 관리함

		 3) 복사 순서
		  - 여러번의 시행착오 끝에 현재 버전에 왔는데 (여전히 완벽하지는 않겠지만), 시행 착오 과정을 아래에 기록함
		  - Scene - GameObject - Clone 의, 루트 오브젝트에서 자식으로 타고 내려가는 클로닝 작업에서, 코드 순서 주의 필요
			(하단은 시행착오들을 기록)
		   1. 부모가 카피된 후, 자식이 카피돼야, 카피된 부모 관계에 오류가 없다.
		   2. 트랜스폼을 정의하지 않은 상태에서, 씬에 게임오브젝트를 AddGameObject 하면, 추후 자식이 될 오브젝트라도,
			  CircDbNode (루트 오브젝트만 받아야 하는) 를 발급받기에, 트랜스폼 관계 정의 이후, 씬에 에드 오브젝트 를 해야 한다
		   3. 루트 -> 자식 으로가는 클로닝 재귀함수 호출이 완료된 후에,
			  AddGameObject (잘못된 버전에서는 AddGameObject가 트랜스폼을 기준으로 재귀호출 되는 구조였음)
			  하는 구조는 안된다.
			  클로닝 재귀함수 호출이 완료되면, 자식 GameObject의 refCount = 0 이 되기 때문에, AddGameObject에서 크래시 난다.
		   4. 3 문제 때문에, Clone 내에 Scene 에 AddGAmeObject 코드가 있는 경우에도, 재귀 호출의 특성상 2 문제를 피하기 까다로우며,
			  설령 순서에 주의해서 2문제를 피한다 해도, 재귀호출의 특성상 자식 오브젝트가 반드시 부모 오브젝트보다 먼저 Scene에
			  AddGameObject 되게 된다. 자식이 먼저 부모보다 먼저 Scene 에 등록된다는 것이, 작성 기준 문제된다 보지는 않지만,
			  코드가 꼬일 가능성이 높아 보인다.

		  ==> 위 시행착오들을 겪으며, 작성 기준 현재 코드는 이렇다.
			 1. GameObject 의 클로닝에서 재귀호출을 하고,
				오브젝트가 만들어진 후,
				자식 오브젝트를 트랜스폼을 참조하여 클로닝.
				클로닝 후, 트랜스폼의 부모 관계 설정
			 2. GameObject 의 클로닝에서 매개변수로 vector<shared_ptr<GameObject?& 를 넘겨서, 부모 먼저 push_back 하고,
			   이후 자식들이 push_back 되는 순서로 코딩.
				임시 벡터에 shared_ptr 이 있기 때문에, 앞선 3. refCount == 0 이 되는 문제를 방지한다
			 3. Clone 재귀 호출이 모두 종료된 후, 앞선 벡터 순차대로, 씬에서 AddGameObject 처리
				(여기서 AddGameObject 는 재귀 호출이 아닌 단일 GameObject 를 Add)

			※ 현재는 코드가 조금 복잡하고, 씬에 등록할 때에도,
			  1. 게임 오브젝트를 직접 만들고, 부모 자식 상관 없이 모든 오브젝트를 AddGameObject
			  2. 게임오브젝트를 클로닝하고, 벡터대로 씬에 AddGameObject
			  이렇게 순서에 주의해야 하는 구조로 처리중이다.
				==>> 추후 모든 게임 오브젝트를 프리팹 으로 리소스에 로드(1 작업에서 Scene 에 Add하는 작업을 제외한?) 한 후,
					 Instantiate 코드는 2. 를 참조해서 만드는 것이 좋을 듯 하다

			※ 위 내용들을 종합하면 알 수 있듯이, 씬에서의 AddGameObject와, GameObject를 만드는 과정,
			   부모 -> 자식 순서대로 클로닝 되는 재귀 호출 클로닝 함수. 이 셋은 독립적으로 작동할 수 있어야 한다.
			   작성 기준 위 방법은 해당 조건을 충족하는 것 같다.
	*/
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
			// ※ 이전에는 RemoveComponent후 loadTransform 을 Add했었는데, 프리팹의 기존 부모 관계가 사라지기 때문에
			//   문제가 됐음. XmlElement 를 이요해 XYZ XYZW XYZ 를 직접 구하는 게 효율적이겠지만, 편의 때문에 이렇게
		}

		goElement = goElement->NextSiblingElement("GameObject");
	}

}

shared_ptr<Camera> Scene::GetCamera()
{
	shared_ptr<Camera> camera; // GameObjects 중 마지막 카메라가 렌더링이니, 마지막 camera까지 조회
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
	// ※ Edit에서도 Physics를 써서 레이처리하면 AABB 처리해서 빠를테지만, Physics가 EditScene에서도 작동하게 하려면 코드를 많이 고쳐야해서, 대충 때움. 추후 필요하다면 수정
#if 0
	const GameDesc& gameDesc = GAME->GetGameDesc();

	float width = gameDesc.width;
	float height = gameDesc.height;
	
	float d_a = Camera::S_MatProjection[0][0];
	float d = Camera::S_MatProjection[1][1];

	float viewX = (+2.0f * screenX / width - 1.0f) / d_a;
	float viewY = (-2.0f * screenY / height + 1.0f) / d;
	// 위 공식은 화면 좌표계 -> 뷰공간 좌표계 변환 공식. 강의 노트에 추가 정리

	float minDistance = FLT_MAX;
	shared_ptr<GameObject> picked;
	for (shared_ptr<GameObject>& gameObject : _gameObjects)
	{
		if (!gameObject)
			continue;
		if (!gameObject->GetCollider())
			continue;

		// ViewSpace에서 
		SimpleVec4 rayOrigin = SimpleVec4(0.f, 0.f, 0.f, 1.f); // NDC Z 가 0_1 이니 Z가 0 ( Start Point )
		SimpleVec4 rayEnd = SimpleVec4(viewX, viewY, 1.0f, 0.f); // NDC Z 가 0_1 이니 Z ( End Point )

		
		SimpleMatrix vInv = MyMathUtils::MyMatrixToSimpleMatrix(Camera::S_MatVInv);

		SimpleVec3 worldRayOrigin = XMVector3TransformCoord(rayOrigin, vInv);
		SimpleVec3 worldRayDir = XMVector3TransformNormal(rayEnd, vInv);
		// 시간남으면 함수 만들자. 간단할듯 NDC  -1 1 / -1 1 / 0 1 을 전제로 작동


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
	// 카메라 데이터를 static Mat 으로 관리하니 한계가 많다. RenderCam 으로 Camera 클래스를 등록해서 렌더링 하는 구조로 추후 변경하자
	// 지금은 땜빵으로 tempNear, tempFar 상수 사용, width, hegith 도 원래는 카메라에서 가져와야 함

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




