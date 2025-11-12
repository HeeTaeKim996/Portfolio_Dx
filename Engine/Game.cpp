#include "pch.h"
#include "Game.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "ImGuiManager.h"
#include "ResourceManager.h"
#include "RenderManager.h"
#include "Scene.h"

#include "InitResource.h"
#include "SceneManager.h"
#include "ITransferShader.h"
#include "IReturnGameObjects.h"
#include "PrefabManager.h"
#include "GameObject.h"
#include "ResourceManager.h"
#include "MeshRenderer.h"
#include "ResourceBase.h"
#include "Material.h"
#include "Physics.h"
#include "BaseCollider.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "TimerManager.h"


#if 1

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
// �� �Լ��� .h �� ������� �ʰ�, .cpp �� �����θ� �ԷµƱ⿡, ��ó�� extern ���� ����


WPARAM Game::Run(GameDesc& desc)
{
	_desc = desc;

	_desc.width = 1700;
	_desc.height = 1000;


	// 1) ������ â ���� ���
	MyRegisterClass();

	// 2) ������ â ����
	if (!InitInstance(SW_SHOWNORMAL)) // �� SW_SHOWNORMAL : nCmdShow(â �ּ�,�ִ�,�Ϲ� �� ������) �� �⺻�� �ǹ�
		return FALSE;

	GRAPHICS->Init(_desc.hWnd);
	INPUT->Init(_desc.hWnd);
	IMGUI->Init(); // �� ���� ��.. ���� 60.ImGUI ���� �϶���� ��..


	InitGame();
	StartEditScene();

	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			if (_gameState == GameState::Edit)
			{
				UpdateEditScene();
			}
			else if (_gameState == GameState::Play)
			{
				UpdatePlayScene();
			}
		}
	}

	return msg.wParam;
}


ATOM Game::MyRegisterClass()
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = _desc.hInstance;
	wcex.hIcon = ::LoadIcon(NULL, IDI_WINLOGO);
	wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _desc.appName.c_str();
	wcex.hIconSm = wcex.hIcon;

	return RegisterClassExW(&wcex);
}

BOOL Game::InitInstance(int cmdShow)
{
	RECT windowRect = { 0, 0, _desc.width, _desc.height };
	::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

	_desc.hWnd = CreateWindowW(_desc.appName.c_str(), _desc.appName.c_str(), WS_OVERLAPPEDWINDOW,
		0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, _desc.hInstance, nullptr);

	if (!_desc.hWnd)
		return FALSE;

	::ShowWindow(_desc.hWnd, cmdShow);
	::UpdateWindow(_desc.hWnd);

	return TRUE;
}

LRESULT CALLBACK Game::WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(handle, message, wParam, lParam))
		return true;
	// ������ �޼����� ImGui ����ü�� �켱 �ݿ�. ImGui �� ���õ� �޼������, ���� �޼����� ó�� X

	switch (message)
	{
	case WM_SIZE:
		break;
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return ::DefWindowProc(handle, message, wParam, lParam);
	}
}




void Game::InitGame()
{
	_shader = make_shared<Shader>(L"PortfolioShader.fx");
	TIME->Init();
	RESOURCES->Init();

	_desc.resourceLoader->TransferShader(_shader); // Init Resource

	PREFAB->Initialize(_desc.prefabLoader->ReturnGameObjects(_shader));


	//_scene = SCENE->Initialize(_desc.initResource->Init(_shader)); // TEMP
	//_scene = SCENE->ObsolateInitialize(); // Obsolate


	//_scene = SCENE->Initialize(true);
	SwitchScene(SCENE->Initialize(false));

	RENDER->Init(_shader);


	{
		_editPointer = make_shared<GameObject>();
		_editPointer->GetOrAddTransform();


		float axisLength = 18.f;
		float axisThickness = 0.2f;


		{
			_editPointer_sphere = make_shared<GameObject>();
			shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>(GAME->GetShader());
			meshRenderer->SetMesh(RESOURCES->Get<Mesh>(L"Sphere"));
			meshRenderer->SetMaterial(RESOURCES->Get<Material>(L"Leather"));
			meshRenderer->SetPass(MeshRenderer::Pass::WHITE);
			_editPointer_sphere->AddComponent(meshRenderer);
			_editPointer_sphere->GetOrAddTransform()->SetParent(*_editPointer->GetTransform().get());
			_editPointer_sphere->GetTransform()->SetScale(myVec3(1.f, 1.f, 1.f));
		}


		{
			_editPointer_XAxis = make_shared<GameObject>();
			shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>(GAME->GetShader());
			shared_ptr<Mesh> mesh = make_shared<Mesh>();
			mesh->CreateCube(axisLength, axisThickness, axisThickness);
			meshRenderer->SetMesh(mesh);
			shared_ptr<Material> material = make_shared<Material>(GAME->GetShader());
			meshRenderer->SetMaterial(material);
			meshRenderer->SetPass(MeshRenderer::Pass::RED);
			_editPointer_XAxis->AddComponent(meshRenderer);
			_editPointer_XAxis->GetOrAddTransform()->SetParent(*_editPointer->GetTransform().get());
			_editPointer_XAxis->GetTransform()->SetPosition(Vec3(axisLength / 2.f, 0.f, 0.f));
		}

		{
			_editPointer_YAxis = make_shared<GameObject>();
			shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>(GAME->GetShader());
			shared_ptr<Mesh> mesh = make_shared<Mesh>();
			mesh->CreateCube(axisThickness, axisLength, axisThickness);
			meshRenderer->SetMesh(mesh);
			shared_ptr<Material> material = make_shared<Material>(GAME->GetShader());
			meshRenderer->SetMaterial(material);
			meshRenderer->SetPass(MeshRenderer::Pass::GREEN);
			_editPointer_YAxis->AddComponent(meshRenderer);
			_editPointer_YAxis->GetOrAddTransform()->SetParent(*_editPointer->GetTransform().get());
			_editPointer_YAxis->GetTransform()->SetPosition(Vec3(0.f, axisLength / 2.f, 0.f));
		}


		{
			_editPointer_ZAxis_Reverse = make_shared<GameObject>();
			shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>(GAME->GetShader());
			shared_ptr<Mesh> mesh = make_shared<Mesh>();
			mesh->CreateCube(axisThickness, axisThickness, axisLength);
			meshRenderer->SetMesh(mesh);
			shared_ptr<Material> material = make_shared<Material>(GAME->GetShader());
			meshRenderer->SetMaterial(material);
			meshRenderer->SetPass(MeshRenderer::Pass::BLUE);
			_editPointer_ZAxis_Reverse->AddComponent(meshRenderer);
			_editPointer_ZAxis_Reverse->GetOrAddTransform()->SetParent(*_editPointer->GetTransform().get());
			_editPointer_ZAxis_Reverse->GetTransform()->SetPosition(Vec3(0.f, 0.f, -axisLength / 2.f));
		}

	}


}


void Game::StartEditScene()
{
	INPUT->SetCursorState(false);

	TIME->SetTimeScale(1.f);
	TIMER->Clear();

	SwitchScene(SCENE->GetSceneByIndex(_sceneIndex));
	_gameState = GameState::Edit;
	isColliderRender = true;

	{ // @@ �ӽ� �ݶ��̴� EditScene �� ���̰� �ϱ� ����..
		vector<shared_ptr<GameObject>> gameObjects = _scene->GetGameObjects();
		for (shared_ptr<GameObject> gameObject : gameObjects)
		{
			if (!gameObject)
				continue;

			if (shared_ptr<BaseCollider> collider = gameObject->GetCollider())
			{
				shared_ptr<Transform> transform = gameObject->GetTransform();
				transform->Awake();
				transform->Enable();
				transform->Start();


				collider->Awake();
				collider->Enable();
				collider->Start();

				transform->AddPosition(Vec3(0.f, 0.f, 0.f));
			}
		}
	}

}

void Game::StartPlayScene()
{
	INPUT->SetCursorState(true);
	isPlaySceneCursorHided = true;

	TIME->SetTimeScale(1.f);
	TIMER->Clear();

	SwitchScene(SCENE->GetSceneByIndex(_sceneIndex)->Clone());

	_gameState = GameState::Play;
	// �� ���� ����. Scene�� AddGameObject ���� GameState �� Play ��, �ΰ��� �� Awake, OnEnable, OnDisable ȣ���̹Ƿ�, Scene ���翡����
	//   Scene->AddGameObject ���� �ߵ��ϸ� �ȵǹǷ�, �� SwitchScene ���Ŀ�, GameState �ٲٱ�

	PHYSICS->Initialize();

	isColliderRender = false;

	_scene->Awake();
	_scene->OnEnable();
	_scene->Start();
}

void Game::UpdateEditScene()
{
	TIME->Update();
	INPUT->Update();

	GRAPHICS->RenderBegin();


	_scene->EditUpdate(); // @@ Edit ī�޶�(���ӿ�����Ʈ ��� x) �� �̵� �� ���� ó����, EidtUpdate ���.
	//_scene->Update(); // �ӽ÷� �ΰ��� ������ Update ���

	IMGUI->Update();
	{
		IMGUI->PostPlayButton();
		IMGUI->PostEditSaveButton();
		IMGUI->PostPrefabPanel();
		IMGUI->PostHierachy();
		IMGUI->UpdateEditCursorPanel();
	}

	if (_gameState == GameState::Edit)
	{
		UpdateEditControllingObject(); // ���� ����. sceneEditUpdate => ImGUI->PostHierachy => �ش� �ڵ� ���� �ʿ�.
	}


	{ // Light
		LightDesc lightDesc;
		lightDesc.ambient = Color(0.4f, 0.4f, 0.4f, 0.4f);
		lightDesc.diffuse = Color(1.f, 1.f, 1.f, 1.f);
		lightDesc.specular = Color(0.f, 0.f, 0.f, 0.f);
		lightDesc.direction = Vec3(1.f, 0.f, 1.f);
		RENDER->PushLightData(lightDesc);
	}
	//_scene->EditRender(); // Obsolate
	_scene->EditRender();

	if (_editControllingObject && !_editControllingObject->GetFixedComponent(ComponentType::Camera))
	{
		_editPointer_sphere->EditRender();
		_editPointer_XAxis->EditRender();
		_editPointer_YAxis->EditRender();
		_editPointer_ZAxis_Reverse->EditRender();
	}


	IMGUI->Render();

	GRAPHICS->RenderEnd();
}





void Game::UpdatePlayScene()
{
	TIME->Update();
	INPUT->Update();

	GRAPHICS->RenderBegin();


	IMGUI->Update();


	if (!isGamePaused && INPUT->GetButton(KEY_TYPE::ESCAPE))
	{
		IMGUI->reservedTimeScale = TIME->GetTimeScale();
		TIME->SetTimeScale(0.f);
		isGamePaused = true;

		INPUT->SetCursorState(false);
	}

	if (isGamePaused && IMGUI->PostPausePanel())
	{
		/* - �̷��� if false �� ��ġ�� ������, ���� ���� ��ư�� ������, ���� scene �� ���� �ȴ�.
			 ������ UpdatePlayScene �� ����Ǿ�, ������ ���� �ʱ�ȭ�� Scene �� Update, LateUpdate,
			 Render�� ȣ��Ǳ� ������, Start, Awake ���� �Ҵ��ؾ� �Ѵ� �������� nullptr�� �Ǿ� ũ���ð�
			 ����.
		   - �׷��� ó������ if true -> return; ���� �Լ� ��ü�� �����غ��� �ߴµ�, ImGui ���� ����ߴµ�
			 Render �� ���� �ʾҴ� �ؼ� �� ũ���ð� ������.
		   - �� ������ �޾�, ����ó�� �ڵ尡 ������											*/

		IMGUI->Render();
		return;
	}

	PHYSICS->Update();

	TIMER->Update();
	_scene->Update();
	{ // Light
		LightDesc lightDesc;
		lightDesc.ambient = Color(0.4f, 0.4f, 0.4f, 0.4f);
		lightDesc.diffuse = Color(1.f, 1.f, 1.f, 1.f);
		lightDesc.specular = Color(0.f, 0.f, 0.f, 0.f);
		lightDesc.direction = Vec3(1.f, 0.f, 1.f);
		RENDER->PushLightData(lightDesc);
	}
	_scene->LateUpdate();
	_scene->Render();




	IMGUI->Render();

	GRAPHICS->RenderEnd();
}

void Game::SwitchScene(shared_ptr<Scene> scene)
{
	_scene = scene;

	_editControllingObject = nullptr;
}






































































void Game::UpdateEditControllingObject()
{
	if (!_editControllingObject)
		return;



	shared_ptr<Transform> transform = _editControllingObject->GetTransform();
	if (!transform)
		return;

	if (!IMGUI->tempIsColliderSelected)
	{
		if (INPUT->GetButton(KEY_TYPE::W))
		{

			transform->AddPosition(transform->GetZAxis() * editControllingObjects_MovementSpeed * DT);

		}
		else if (INPUT->GetButton(KEY_TYPE::S))
		{

			transform->AddPosition(-transform->GetZAxis() * editControllingObjects_MovementSpeed * DT);

		}

		if (INPUT->GetButton(KEY_TYPE::D))
		{

			transform->AddPosition(transform->GetXAxis() * editControllingObjects_MovementSpeed * DT);

		}
		else if (INPUT->GetButton(KEY_TYPE::A))
		{

			transform->AddPosition(-transform->GetXAxis() * editControllingObjects_MovementSpeed * DT);

		}


		if (INPUT->GetButton(KEY_TYPE::Q))
		{

			transform->AddPitchRotation(editControllingObjects_RotateSpeed * DT);


		}
		else if (INPUT->GetButton(KEY_TYPE::E))
		{

			transform->AddPitchRotation(-editControllingObjects_RotateSpeed * DT);

		}

		if (INPUT->GetButton(KEY_TYPE::Z))
		{

			transform->AddYawRotation(editControllingObjects_RotateSpeed * DT);

		}
		else if (INPUT->GetButton(KEY_TYPE::C))
		{

			transform->AddYawRotation(-editControllingObjects_RotateSpeed * DT);

		}

		if (INPUT->GetButton(KEY_TYPE::NUMPAD_4))
		{

			transform->AddUnitYRotation(editControllingObjects_RotateSpeed * DT);

		}
		if (INPUT->GetButton(KEY_TYPE::NUMPAD_6))
		{

			transform->AddUnitYRotation(-editControllingObjects_RotateSpeed * DT);

		}
		if (INPUT->GetButton(KEY_TYPE::NUMPAD_8))
		{

			transform->AddPitchRotation(editControllingObjects_RotateSpeed * DT);

		}
		if (INPUT->GetButton(KEY_TYPE::NUMPAD_2))
		{

			transform->AddPitchRotation(-editControllingObjects_RotateSpeed * DT);

		}
		if (INPUT->GetButton(KEY_TYPE::NUMPAD_7))
		{

			transform->AddPosition(transform->Up() * editControllingObjects_MovementSpeed * DT);

		}
		if (INPUT->GetButton(KEY_TYPE::NUMPAD_1))
		{

			transform->AddPosition(-transform->Up() * editControllingObjects_MovementSpeed * DT);

		}
		if (INPUT->GetButton(KEY_TYPE::NUMPAD_9))
		{

			transform->AddRollRotation(editControllingObjects_RotateSpeed * DT);

		}
		if (INPUT->GetButton(KEY_TYPE::NUMPAD_3))
		{

			transform->AddRollRotation(-editControllingObjects_RotateSpeed * DT);

		}

		Vec3 scale = transform->GetScale();
		static float scaleRatio = 1.1f;

		if (INPUT->GetButton(KEY_TYPE::KEY_1))
		{
			float scaleX = scale.X;
			scaleX += scaleX * scaleRatio * DT;
			scale.X = scaleX;
		}
		else if (INPUT->GetButton(KEY_TYPE::KEY_2))
		{
			float scaleX = scale.X;
			scaleX -= scaleX * scaleRatio * DT;
			scale.X = scaleX;
		}
		if (INPUT->GetButton(KEY_TYPE::KEY_3))
		{
			float scaleY = scale.Y;
			scaleY += scaleY * scaleRatio * DT;
			scale.Y = scaleY;
		}
		else if (INPUT->GetButton(KEY_TYPE::KEY_4))
		{
			float scaleY = scale.Y;
			scaleY -= scaleY * scaleRatio * DT;
			scale.Y = scaleY;
		}
		if (INPUT->GetButton(KEY_TYPE::KEY_5))
		{
			float scaleZ = scale.Z;
			scaleZ += scaleZ * scaleRatio * DT;
			scale.Z = scaleZ;
		}
		else if (INPUT->GetButton(KEY_TYPE::KEY_6))
		{
			float scaleZ = scale.Z;
			scaleZ -= scaleZ * scaleRatio * DT;
			scale.Z = scaleZ;
		}


		transform->SetScale(scale);
	}
	else
	{
		static float moveSpeed = 30.f;
		static float rotateSpeed = 40.f;


		shared_ptr<BaseCollider> collider = _editControllingObject->GetCollider();






		switch (collider->GetColliderType())
		{
		case ColliderType::Sphere:
		{
			shared_ptr<SphereCollider> sphereCollider = static_pointer_cast<SphereCollider>(collider);

			if (INPUT->GetButton(KEY_TYPE::KEY_1) | INPUT->GetButton(KEY_TYPE::KEY_3) || INPUT->GetButton(KEY_TYPE::KEY_5))
			{
				float& radius = sphereCollider->GetRadius();
				radius += radius * 0.1f * DT;
			}
			else if (INPUT->GetButton(KEY_TYPE::KEY_2) | INPUT->GetButton(KEY_TYPE::KEY_4) || INPUT->GetButton(KEY_TYPE::KEY_6))
			{
				float& radius = sphereCollider->GetRadius();
				radius -= radius * 0.1f * DT;
			}



			Vec3& offset = collider->GetOffset();

			if (INPUT->GetButton(KEY_TYPE::W))
			{
				offset += Vec3(0.f, 0.f, moveSpeed * DT);
			}
			else if (INPUT->GetButton(KEY_TYPE::S))
			{
				offset += Vec3(0.f, 0.f, -moveSpeed * DT);
			}
			if (INPUT->GetButton(KEY_TYPE::D))
			{
				offset += Vec3(moveSpeed * DT, 0.f, 0.f);
			}
			else if (INPUT->GetButton(KEY_TYPE::A))
			{
				offset -= Vec3(moveSpeed * DT, 0.f, 0.f);
			}
			if (INPUT->GetButton(KEY_TYPE::NUMPAD_7))
			{
				offset += Vec3(0.f, moveSpeed * DT, 0.f);
			}
			else if (INPUT->GetButton(KEY_TYPE::NUMPAD_1))
			{
				offset -= Vec3(0.f, moveSpeed * DT, 0.f);
			}



			break;
		}
		case ColliderType::OBB:
		{
			shared_ptr<BoxCollider> boxCollider = static_pointer_cast<BoxCollider>(collider);

			Vec3& halfExtents = boxCollider->GetHalfExtents();
			if (INPUT->GetButton(KEY_TYPE::KEY_1))
			{
				halfExtents.X += halfExtents.X * 0.1 * DT;
			}
			else if (INPUT->GetButton(KEY_TYPE::KEY_2))
			{
				halfExtents.X -= halfExtents.X * 0.1 * DT;
			}
			if (INPUT->GetButton(KEY_TYPE::KEY_3))
			{
				halfExtents.Y += halfExtents.Y * 0.1 * DT;
			}
			else if (INPUT->GetButton(KEY_TYPE::KEY_4))
			{
				halfExtents.Y -= halfExtents.Y * 0.1 * DT;
			}
			if (INPUT->GetButton(KEY_TYPE::KEY_5))
			{
				halfExtents.Z += halfExtents.Z * 0.1 * DT;
			}
			else if (INPUT->GetButton(KEY_TYPE::KEY_6))
			{
				halfExtents.Z -= halfExtents.Z * 0.1 * DT;
			}


			myVec3 scale, pos;
			Quaternion& quat = boxCollider->GetQuaternion();
			TransformBase transform(pos, quat, scale);
			if (INPUT->GetButton(KEY_TYPE::NUMPAD_6))
			{
				transform.AddUnitYRotation(40.f * DT);
			}
			else if(INPUT->GetButton(KEY_TYPE::NUMPAD_4))
			{
				transform.AddUnitYRotation(-40.f * DT);
			}
			if (INPUT->GetButton(KEY_TYPE::NUMPAD_8))
			{
				transform.AddPitchRotation(40.f * DT);
			}
			else if (INPUT->GetButton(KEY_TYPE::NUMPAD_2))
			{
				transform.AddPitchRotation(-40.f * DT);
			}
			if (INPUT->GetButton(KEY_TYPE::NUMPAD_9))
			{
				transform.AddRollRotation(40.f * DT);
			}
			else if (INPUT->GetButton(KEY_TYPE::NUMPAD_3))
			{
				transform.AddRollRotation(-40.f * DT);
			}
			quat = transform.GetRotation();




			float obbMoveSpeed = moveSpeed * 0.03f;

			Vec3& offset = collider->GetOffset();

			if (INPUT->GetButton(KEY_TYPE::W))
			{
				offset += Vec3(0.f, 0.f, obbMoveSpeed * DT);
			}
			else if (INPUT->GetButton(KEY_TYPE::S))
			{
				offset += Vec3(0.f, 0.f, -obbMoveSpeed * DT);
			}
			if (INPUT->GetButton(KEY_TYPE::D))
			{
				offset += Vec3(obbMoveSpeed * DT, 0.f, 0.f);
			}
			else if (INPUT->GetButton(KEY_TYPE::A))
			{
				offset -= Vec3(obbMoveSpeed * DT, 0.f, 0.f);
			}
			if (INPUT->GetButton(KEY_TYPE::NUMPAD_7))
			{
				offset += Vec3(0.f, obbMoveSpeed * DT, 0.f);
			}
			else if (INPUT->GetButton(KEY_TYPE::NUMPAD_1))
			{
				offset -= Vec3(0.f, obbMoveSpeed * DT, 0.f);
			}



			break;
		}

		case ColliderType::Capsule:
		{
			shared_ptr<CapsuleCollider> capsuleCollider = static_pointer_cast<CapsuleCollider>(collider);


			
			float& radius = capsuleCollider->GetRadius();
			float& halfHeight = capsuleCollider->GetHalfHeight();


			if (INPUT->GetButton(KEY_TYPE::KEY_1))
			{
				radius += radius * 0.3f * DT;
			}
			else if (INPUT->GetButton(KEY_TYPE::KEY_2))
			{
				radius -= radius * 0.3f * DT;
			}
			if (INPUT->GetButton(KEY_TYPE::KEY_3))
			{
				halfHeight += halfHeight * 0.3f * DT;
			}
			else if (INPUT->GetButton(KEY_TYPE::KEY_4))
			{
				halfHeight -= halfHeight * 0.3f * DT;
			}





			myVec3 scale, pos;
			Quaternion& quat = capsuleCollider->GetQuaternion();
			TransformBase transform(pos, quat, scale);
			if (INPUT->GetButton(KEY_TYPE::NUMPAD_6))
			{
				transform.AddUnitYRotation(40.f * DT);
			}
			else if (INPUT->GetButton(KEY_TYPE::NUMPAD_4))
			{
				transform.AddUnitYRotation(-40.f * DT);
			}
			if (INPUT->GetButton(KEY_TYPE::NUMPAD_8))
			{
				transform.AddPitchRotation(40.f * DT);
			}
			else if (INPUT->GetButton(KEY_TYPE::NUMPAD_2))
			{
				transform.AddPitchRotation(-40.f * DT);
			}
			if (INPUT->GetButton(KEY_TYPE::NUMPAD_9))
			{
				transform.AddRollRotation(40.f * DT);
			}
			else if (INPUT->GetButton(KEY_TYPE::NUMPAD_3))
			{
				transform.AddRollRotation(-40.f * DT);
			}
			quat = transform.GetRotation();











			Vec3& offset = collider->GetOffset();

			if (INPUT->GetButton(KEY_TYPE::W))
			{
				offset += Vec3(0.f, 0.f, moveSpeed * DT);
			}
			else if (INPUT->GetButton(KEY_TYPE::S))
			{
				offset += Vec3(0.f, 0.f, -moveSpeed * DT);
			}
			if (INPUT->GetButton(KEY_TYPE::D))
			{
				offset += Vec3(moveSpeed * DT, 0.f, 0.f);
			}
			else if (INPUT->GetButton(KEY_TYPE::A))
			{
				offset -= Vec3(moveSpeed * DT, 0.f, 0.f);
			}
			if (INPUT->GetButton(KEY_TYPE::NUMPAD_7))
			{
				offset += Vec3(0.f, moveSpeed * DT, 0.f);
			}
			else if (INPUT->GetButton(KEY_TYPE::NUMPAD_1))
			{
				offset -= Vec3(0.f, moveSpeed * DT, 0.f);
			}
			break;
		}

		}
	}


}



#endif 


























































#if 0 //First
WPARAM Game::Run(GameDesc& desc)
{
	_desc = desc;
	assert(_desc.app != nullptr);

	MyRegisterClass();

	if (!InitInstance(SW_SHOWNORMAL))
	{
		return FALSE;
	}

	GRAPHICS->Init(_desc.hWnd);
	TIME->Init();
	INPUT->Init(_desc.hWnd);

	_desc.app->Init();

	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			Update();
		}
	}

	return msg.wParam;
}

ATOM Game::MyRegisterClass()
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = _desc.hInstance;
	wcex.hIcon = ::LoadIcon(NULL, IDI_WINLOGO);
	wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _desc.appName.c_str();
	wcex.hIconSm = wcex.hIcon;

	return RegisterClassExW(&wcex);
}

BOOL Game::InitInstance(int cmdShow)
{
	RECT windowRect = { 0, 0, _desc.width, _desc.height };
	::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

	_desc.hWnd = CreateWindow(_desc.appName.c_str(), _desc.appName.c_str(), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		nullptr, nullptr, _desc.hInstance, nullptr);

	if (!_desc.hWnd)
	{
		return FALSE;
	}

	::ShowWindow(_desc.hWnd, cmdShow);
	::UpdateWindow(_desc.hWnd);

	return TRUE;
}

LRESULT CALLBACK Game::WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		break;
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return ::DefWindowProc(handle, message, wParam, lParam);
	}
}

void Game::Update()
{
	TIME->Update();
	INPUT->Update();

	GRAPHICS->RenderBegin();

	_desc.app->Update();
	_desc.app->Render();

	GRAPHICS->RenderEnd();
}

#endif // First