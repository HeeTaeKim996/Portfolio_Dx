#include "pch.h"
#include "CameraScript.h"
#include "Engine/InputManager.h"
#include "Engine/TimeManager.h"
#include "Engine/tinyxml2.h"
#include "Engine/Utils.h"

#include "Engine/GameObject.h"
#include "Engine/Scene.h"
#include "PlayerMovement.h"

void CameraScript::Awake()
{
	_playerTransform = GetGameObject()->GetScene()->FindObjectOfMono<PlayerMovement>()
		->GetGameObject()->GetTransform();
	_posVec = Vec3(0, 0, -1);
}

void CameraScript::Start()
{
	_mousePos = INPUT->GetMousePos();
}

void CameraScript::Update()
{
	if (shared_ptr<Transform> transform = _lockTransform.lock())
	{
		Vec3 aim = (GetPivotPos(_playerTransform.lock()) - transform->GetPosition()).GetNormalize();

		_posVec = myVec3::Lerp(_posVec, aim, 7.7f * DT);
		 
	}
	else
	{
		Vec2 cursorMoveVec = INPUT->GetCursorMoveVec();
		float diffX = cursorMoveVec.x;
		float diffY = cursorMoveVec.y;


		if (fabs(diffX) > 1e-0)
		{
			myQuaternion quat(myVec3::UnitY, -diffX * 40.f * DT);
			_posVec = quat.RotateVector(_posVec);
		}
		if (fabs(diffY) > 1e-0)
		{
			myQuaternion quat(GetTransform()->Right(), diffY * 40.f * DT);
			_posVec = quat.RotateVector(_posVec);
		}
	}
}

void CameraScript::LateUpdate()
{
	shared_ptr<Transform> transform = GetTransform();
	transform->SetPosition(GetPivotPos(_playerTransform.lock()) + _posVec * _distance
	+ interPolVec);

	myQuaternion quat(-_posVec);
	transform->SetRotation(quat);
}



void CameraScript::RegisterLockOn(shared_ptr<Transform> transform)
{
	_lockTransform = transform;
}

void CameraScript::RemoveLockOn()
{
	_lockTransform.reset();
}

myVec3 CameraScript::GetPivotPos(shared_ptr<Transform> transform)
{
	return transform->GetPosition() + myVec3(0.f, 12.f, 0.f);

}



void CameraScript::LoadXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent)
{
	// 필드멤버 업로드 내용 없음
}

void CameraScript::WriteXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent)
{
	parent->SetText("CameraScript");
}

shared_ptr<Component> CameraScript::Clone() const
{
	shared_ptr<CameraScript> copy = make_shared<CameraScript>(*this);

	return copy;
}
