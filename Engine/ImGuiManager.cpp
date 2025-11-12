#include "pch.h"
#include "ImGuiManager.h"
#include "Game.h"
#include "TimeManager.h"
#include "Scene.h"
#include "PrefabManager.h"
#include "SceneManager.h"
#include "Camera.h"
#include "InputManager.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"

void ImGuiManager::Init()
{
	IMGUI_CHECKVERSION();	
	ImGui::CreateContext();	
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();	
	//ImGui::StyleColorsLight();

	 //Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(GAME->GetGameDesc().hWnd);	
	ImGui_ImplDX11_Init(DEVICE.Get(), DC.Get());
}

void ImGuiManager::Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::Render()
{
	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiManager::PostPlayButton()
{
	const GameDesc& gameDesc = GAME->GetGameDesc();

	ImGui::SetNextWindowPos(ImVec2(gameDesc.width / 2.f, 0), ImGuiCond_Always,
		ImVec2(0.7f, 0.f));

	Vec2 size = Vec2(gameDesc.width / 15.f, gameDesc.height / 30.f);
	ImGui::SetNextWindowSize(ImVec2(size.x, size.y));

	if (ImGui::Begin("Play_Buttons", nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoCollapse))
	{
		const GameState& gameState = GAME->GetGameState();
		if (gameState == GameState::Edit)
		{
			ImGui::SetCursorPos(ImVec2(size.x * 0.12f, size.y * 0.2f));
			if (ImGui::Button("Play"))
			{
				GAME->StartPlayScene();
			}
		}
	}

	ImGui::End();
}

bool ImGuiManager::PostPausePanel()
{

	bool didGameExitInvoked = false;

	const GameDesc& gameDesc = GAME->GetGameDesc();

	ImGui::SetNextWindowPos(ImVec2(gameDesc.width / 2.f, gameDesc.height / 2.f), ImGuiCond_Always,
		ImVec2(0.7f, 0.f));

	Vec2 size = Vec2(gameDesc.width / 10.f, gameDesc.height / 7.f);
	ImGui::SetNextWindowSize(ImVec2(size.x, size.y));




	if (ImGui::Begin("Pause_ResumPanel", nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoCollapse))
	{
		if (ImGui::Button("Resume"))
		{
			TIME->SetTimeScale(reservedTimeScale);
			GAME->isGamePaused = false;

			if (GAME->isPlaySceneCursorHided)
			{
				INPUT->SetCursorState(true);
			}
		}

		if (GAME->isPlaySceneCursorHided)
		{
			if (ImGui::Button("ShowCursor"))
			{
				GAME->isPlaySceneCursorHided = false;
			}
		}
		else
		{
			if (ImGui::Button("HideCursor"))
			{
				GAME->isPlaySceneCursorHided = true;
			}
		}

		if (ImGui::Button("Exit"))
		{
			GAME->StartEditScene();

			GAME->isGamePaused = false;

			didGameExitInvoked = true;
		}
	}

	ImGui::End();

	if (didGameExitInvoked)
	{
		return true;
	}
	else
	{
		return false;
	}

}

void ImGuiManager::UpdateEditCursorPanel()
{
	static shared_ptr<GameObject> go = nullptr;

	if (INPUT->GetButtonDown(KEY_TYPE::LBUTTON))
	{
		go = nullptr;

		int32 mouseX = INPUT->GetMousePos().x;
		int32 mouseY = INPUT->GetMousePos().y;

		// Picking
		shared_ptr<Scene> scene = GAME->GetCurrentScene();
		shared_ptr<GameObject> pickedObj = scene->RayPick(mouseX, mouseY);
		if (pickedObj)
		{
			go = pickedObj;
		}
	}

	if (go)
	{
		SetEditSelectedObject(go);
	}
}






void ImGuiManager::PostEditSaveButton()
{
	const GameDesc& gameDesc = GAME->GetGameDesc();
	Vec2 size(gameDesc.width * 0.04f, gameDesc.height * 0.04f);
	Vec2 pos(gameDesc.width / 2.f + size.y * 1.f, 0);

	ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y));
	ImGui::SetNextWindowSize(ImVec2(size.x, size.y));
	if (ImGui::Begin("Edit Save Button", nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse))
	{
		if (ImGui::Button("Save"))
		{
			SCENE->SaveScene();
		}
	}

	ImGui::End();
}

void ImGuiManager::PostHierachy()
{

	const GameDesc& gameDesc = GAME->GetGameDesc();
	Vec2 size(gameDesc.width * 0.15f, gameDesc.height * 0.6f);
	Vec2 pos(gameDesc.width - size.x, 0);

	ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y));
	ImGui::SetNextWindowSize(ImVec2(size.x, size.y));
	if (ImGui::Begin("Hierachy", nullptr,
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse
	))
	{
		shared_ptr<Scene> scene = GAME->GetCurrentScene();
		vector<shared_ptr<GameObject>> gameObjects = scene->GetRootObjects();

		static int selectedIndex = -1;

		for (int i = 0; i < gameObjects.size(); i++)
		{
			shared_ptr<GameObject>& object = gameObjects[i];

			if (!object)
				continue;

			bool isSelected = (selectedIndex == i);

			string name = "NoName";
			if (object->GetName() != "")
			{
				name = object->GetName();
			}

			if (ImGui::Selectable(name.c_str(), isSelected))
			{
				selectedIndex = i;
				tempIsColliderSelected = false;
			}

			if (isSelected)
			{
				ImGui::SameLine();
				if (ImGui::Button("Delete"))
				{
					scene->DestroyGameObject(object);

					selectedIndex = -1;

					continue;
				}

				ImGui::SameLine();
				if (ImGui::Button("Copy"))
				{
					shared_ptr<Scene> currScene = GAME->GetCurrentScene();
					vector<shared_ptr<GameObject>> copies;
					object->Clone(currScene, copies);

					{
						string copyName = copies[0]->GetName();
						int stack = 0;
						size_t pos = copyName.size() - 1;
						while (pos != std::string::npos)
						{
							if (copyName[pos] == '_')
							{
								stack++;
							}
							if (stack == 2)
							{
								break;
							}
							pos--;
						}

						assert(copyName[pos] == '_');
						copyName.erase(pos);

						copies[0]->SetName(copyName);
					}



					for (shared_ptr<GameObject> co : copies)
					{
						currScene->AddGameObject(co);
					}
					
				}

				if (!tempIsColliderSelected)
				{
					if (shared_ptr<BaseCollider> collider = object->GetCollider())
					{
						if (ImGui::Button("Collider"))
						{
							tempIsColliderSelected = true;
						}
					}
				}
			}
		}

		if (ImGui::IsMouseReleased(0) && !ImGui::IsWindowHovered())
		{
			selectedIndex = -1;
			tempIsColliderSelected = false;
		}

		if (selectedIndex != -1)
		{
			shared_ptr<GameObject> go = gameObjects[selectedIndex];
			SetEditSelectedObject(go);


			if (tempIsColliderSelected)
			{
				shared_ptr<BaseCollider> collider = go->GetCollider();

				ImGui::SetNextWindowSize(ImVec2(300.f, 400.f));
				ImGui::SetNextWindowPos(ImVec2(50.f, 50.f));
				if (ImGui::Begin("ColliderInfos", nullptr,
					ImGuiWindowFlags_NoResize || ImGuiWindowFlags_NoCollapse
				))
				{
					switch (collider->GetColliderType())
					{
					case ColliderType::Sphere:
					{
						shared_ptr<SphereCollider> sphereCollider = static_pointer_cast<SphereCollider>(collider);


						Vec3& offset = collider->GetOffset();
						ImGui::Text("Offset X : %.6f", offset.X);
						ImGui::Text("Offset Y : %.6f", offset.Y);
						ImGui::Text("Offset Z : %.6f", offset.Z);
						ImGui::NewLine();

						ImGui::Text("Radius : %.6f", sphereCollider->GetRadius());


						if (INPUT->GetButtonUp(KEY_TYPE::P))
						{
							FILE* file;
							::fopen_s(&file, "../TempColliderData.csv", "w");

							char offsetVec[512];
							sprintf_s(offsetVec,"Vec3(%.6f, %.6f, %.6f)", offset.X, offset.Y, offset.Z);

							::fprintf(file, "offset :  ,\"%s\" \n", offsetVec);
							::fprintf(file, "radius : , %.6f", sphereCollider->GetRadius());

							::fclose(file);
						}


						break;
					}
					case ColliderType::OBB:
					{
						shared_ptr<BoxCollider> boxCollider = static_pointer_cast<BoxCollider>(collider);

						Vec3& offset = collider->GetOffset();
						ImGui::Text("Offset X : %.6f", offset.X);
						ImGui::Text("Offset Y : %.6f", offset.Y);
						ImGui::Text("Offset Z : %.6f", offset.Z);
						ImGui::NewLine();


						Vec3& halfExtents = boxCollider->GetHalfExtents();
						ImGui::Text("Half X : %.6f", halfExtents.X);
						ImGui::Text("Half Y : %.6f", halfExtents.Y);
						ImGui::Text("Half Z : %.6f", halfExtents.Z);

						ImGui::NewLine();

						Quaternion quat = boxCollider->GetQuaternion();
						ImGui::Text("Quat X : %.6f", quat.X);
						ImGui::Text("Quat Y : %.6f", quat.Y);
						ImGui::Text("Quat Z : %.6f", quat.Z);
						ImGui::Text("Quat W : %.6f", quat.W);



						if (INPUT->GetButtonUp(KEY_TYPE::P))
						{
							FILE* file;
							::fopen_s(&file, "../TempColliderData.csv", "w");


							char offsetVec[512];
							sprintf_s(offsetVec, "Vec3(%.6f, %.6f, %.6f)", offset.X, offset.Y, offset.Z);
							::fprintf(file, "offset :  ,\"%s\" \n", offsetVec);

							char halfExtentsString[512];
							sprintf_s(halfExtentsString, "Vec3(%.6f, %.6f, %.6f)", halfExtents.X, halfExtents.Y, halfExtents.Z);
							::fprintf(file, "halfExtents : ,\"%s\" \n", halfExtentsString);

							char quatString[512];
							sprintf_s(quatString, "Vec4(%.6f, %.6f, %.6f, %.6f)", quat.X, quat.Y, quat.Z, quat.W);
							::fprintf(file, "Quat : ,\"%s\" \n", quatString);


							::fclose(file);
						}
						break;
					}
					case ColliderType::Capsule:
					{
						shared_ptr<CapsuleCollider> capsuleCollider = static_pointer_cast<CapsuleCollider>(collider);

						Vec3& offset = collider->GetOffset();
						ImGui::Text("Offset X : %.6f", offset.X);
						ImGui::Text("Offset Y : %.6f", offset.Y);
						ImGui::Text("Offset Z : %.6f", offset.Z);
						ImGui::NewLine();



						ImGui::Text("Radius : %.6f", capsuleCollider->GetRadius());

						ImGui::NewLine();

						ImGui::Text("HalfHeight : 6.f", capsuleCollider->GetHalfHeight());

						ImGui::NewLine();

						Quaternion quat = capsuleCollider->GetQuaternion();
						ImGui::Text("Quat X : %.6f", quat.X);
						ImGui::Text("Quat Y : %.6f", quat.Y);
						ImGui::Text("Quat Z : %.6f", quat.Z);
						ImGui::Text("Quat W : %.6f", quat.W);
						




						if (INPUT->GetButtonUp(KEY_TYPE::P))
						{
							FILE* file;
							::fopen_s(&file, "../TempColliderData.csv", "w");


							char offsetVec[512];
							sprintf_s(offsetVec, "Vec3(%.6f, %.6f, %.6f)", offset.X, offset.Y, offset.Z);
							::fprintf(file, "offset :  ,\"%s\" \n", offsetVec);

							::fprintf(file, "Radius : , %.6f.f \n", capsuleCollider->GetRadius());

							char quatString[512];
							sprintf_s(quatString, "Vec4(%.6f, %.6f, %.6f, %.6f)", quat.X, quat.Y, quat.Z, quat.W);
							::fprintf(file, "Quat : ,\"%s\" \n", quatString);


							::fclose(file);
						}


						break;
					}
					}
				}
				ImGui::End();
			}
		}

	}
	ImGui::End();
}

void ImGuiManager::PostPrefabPanel()
{
	const GameDesc& gameDesc = GAME->GetGameDesc();
	Vec2 size(gameDesc.width * 0.15f, gameDesc.height * 0.4f);
	Vec2 pos(gameDesc.width - size.x, gameDesc.height - size.y);

	ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y));
	ImGui::SetNextWindowSize(ImVec2(size.x, size.y));
	if (ImGui::Begin("Prefabs", nullptr,
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
	{
		vector<shared_ptr<GameObject>> prefabs = PREFAB->GetAllPrefabs();

		static int selectedIndex = -1;

		for (int i = 0; i < prefabs.size(); i++)
		{
			shared_ptr<GameObject>& prefab = prefabs[i];

			if (!prefab)
				continue;

			bool isSelected = (selectedIndex == i);

			string name = "NoName";
			if (prefab->GetName() != "")
			{
				name = prefab->GetName();
			}

			if (ImGui::Selectable(name.c_str(), isSelected))
			{
				selectedIndex = i;
			}

			if (isSelected)
			{
				ImGui::SameLine();
				if (ImGui::Button("Instantiate"))
				{
					assert(Camera::readyRender);

					Matrix vInv = Camera::S_MatVInv;

					Vec3 cameraPos = Vec3(vInv[3][0], vInv[3][1], vInv[3][2]);
					Vec3 cameraLookDirecon = Vec3(vInv[2][0], vInv[2][1], vInv[2][2]);

					Vec3 instanPos = cameraPos + cameraLookDirecon * 60.f;


					shared_ptr<Scene> scene = GAME->GetCurrentScene();
					scene->Instantiate(prefab->GetName(), instanPos);

					break;
				}
			}
		}

	}

	ImGui::End();
}

void ImGuiManager::SetEditSelectedObject(shared_ptr<GameObject> go)
{
	GAME->SetEditControllingObject(go);

	shared_ptr<Transform> transform = go->GetTransform();
	if (transform)
	{
		GAME->_editPointer->GetTransform()->SetPosition(transform->GetPosition());
		GAME->_editPointer->GetTransform()->SetRotation(transform->GetRotation());
	}
}




