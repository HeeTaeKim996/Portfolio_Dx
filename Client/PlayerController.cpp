#include "pch.h"
#include "PlayerController.h"
#include "PlayerMovement.h"
#include "Engine/InputManager.h"
#include "Engine/Collision.h"
#include "Engine/BaseCollider.h"
#include "Engine/GameObject.h"
#include "Engine/Transform.h"
#include "Engine/TimeManager.h"

void PlayerController::Awake()
{
	shared_ptr<GameObject> gameObject = _gameObject.lock();

	_playerMovement = gameObject->GetComponent<PlayerMovement>();
}

void PlayerController::Start()
{
}

void PlayerController::Update()
{
	Vec2 screenVec = Vec2::Zero;
	if (!INPUT->IsCursorHided() && TIME->GetTimeScale() != 0.f/* 임시로 사용.ESCAPE로 설정 들어갈시, 보이지 않기 위해*/)
	{
		ImVec2 center(350.f, 700.f);
		screenVec = DrawVirtualPad(center, 200);





		if (screenVec.LengthSquared() > 0.04f)
		{
			screenVec.Normalize();


			{ // DEBUG

				if (ImGui::Begin("PlayerController_DEBUG"))
				{
					ImGui::Text("X : %.6f", screenVec.x);
					ImGui::Text("Y : %.6f", screenVec.y);
				}

				ImGui::End();
			}
			_playerMovement.lock()->InvokeMove(screenVec);

		}
		else
		{
			screenVec = Vec2::Zero;
		}
	}


	int horizon = 0, vertical = 0;
	
	if (INPUT->GetButton(KEY_TYPE::W))
	{
		vertical = 1;
	}
	else if (INPUT->GetButton(KEY_TYPE::S))
	{
		vertical = -1;
	}
	if (INPUT->GetButton(KEY_TYPE::D))
	{
		horizon = 1;
	}
	else if (INPUT->GetButton(KEY_TYPE::A))
	{
		horizon = -1;
	}

	if (vertical != 0 || horizon != 0)
	{
		_playerMovement.lock()->InvokeMove(vertical, horizon);
	}
	else if(screenVec == Vec2::Zero)
	{
		_playerMovement.lock()->InformNoMove();
	}





	if (INPUT->GetButtonDown(KEY_TYPE::SPACE))
	{
		_playerMovement.lock()->InvokeJump();
	}

	if (INPUT->GetButtonDown(KEY_TYPE::RBUTTON))
	{
		_playerMovement.lock()->OnLockButtonClicked();
	}
}

void PlayerController::LateUpdate()
{
}

void PlayerController::OnCollisionEnter(const Collision& collision)
{
	 // DEBUG
}

void PlayerController::OnCollisionExit(const Collision& collision)
{
	
#if 0 // DEBUG Obsolate
	shared_ptr<BaseCollider> collider = collision.other;

	shared_ptr<Transform> otherTransform = collider->GetGameObject()->GetTransform();

	shared_ptr<Transform> transform = GetGameObject()->GetTransform();

	FILE* file;

	fopen_s(&file, "../TempDebug.csv", "w");

	fprintf(file, "%.6f, %.6f, %.6f\n", transform->GetPosition().X, transform->GetPosition().Y, transform->GetPosition().Z);
	fprintf(file, "%.6f, %.6f, %.6f\n", otherTransform->GetPosition().X, otherTransform->GetPosition().Y, 
		otherTransform->GetPosition().Z);
	fprintf(file, "%.6f, %.6f, %.6f\n", collision.normal.X, collision.normal.Y, collision.normal.Z);

	fclose(file);
#endif
}

Vec2 PlayerController::DrawVirtualPad(const ImVec2& center, float radius)
{
	ImDrawList* draw_list = ImGui::GetForegroundDrawList();
	ImU32 colCircle = IM_COL32(255, 255, 255, 100);
	ImU32 colKnob = IM_COL32(255, 255, 255, 200);

	// 배경 원
	draw_list->AddCircleFilled(center, radius, colCircle, 64);

	ImVec2 mousePos = ImGui::GetIO().MousePos;
	bool mouseDown = ImGui::IsMouseDown(0);

	static bool isActive = false;
	static ImVec2 offset = ImVec2(0, 0);

	if (mouseDown)
	{
		if (!isActive)
		{
			// 첫 클릭이 원 내부일 때만 활성화
			float distSq = (mousePos.x - center.x) * (mousePos.x - center.x) +
				(mousePos.y - center.y) * (mousePos.y - center.y);
			if (distSq <= radius * radius) isActive = true;
		}
	}
	else
	{
		isActive = false;
		offset = ImVec2(0, 0);
	}

	if (isActive)
	{
		offset.x = mousePos.x - center.x;
		offset.y = mousePos.y - center.y;

		float len = sqrtf(offset.x * offset.x + offset.y * offset.y);
		if (len > radius)
		{
			offset.x = offset.x / len * radius;
			offset.y = offset.y / len * radius;
		}
	}

	// 손잡이
	draw_list->AddCircleFilled(ImVec2(center.x + offset.x, center.y + offset.y),
		20.0f, colKnob, 32);

	// 출력 벡터 (0~1 범위, Y는 위가 +)
	Vec2 result(offset.x / radius, -offset.y / radius);
	return result;
}

shared_ptr<Component> PlayerController::Clone() const
{
	return make_shared<PlayerController>(*this);
}


