#include "pch.h"
#include "PlayerMovement.h"
#include "Engine/ModelAnimator.h"
#include "Engine/TimeManager.h"
#include "Engine/Scene.h"
#include "Engine/GameObject.h"
#include "CameraScript.h"
#include "Engine/Transform.h"
#include <cmath>
#include "Engine/Rigidbody.h"
#include "Engine/Camera.h"
#include "Engine/BaseCollider.h"
#include "Engine/Physics.h"
#include "CameraScript.h"
#include "Engine/ModelAnimation.h"
#include "Engine/Collision.h"
#include "Goomba.h"


//#define TEST0_NOTHING
//#define TEST1_IK
#define TEST2_UNDER

void PlayerMovement::Awake()
{
	shared_ptr<GameObject> gameObject = _gameObject.lock();
	AnimInitialize(gameObject->Animator());

	_cameraScript = GetGameObject()->GetScene()->FindObjectOfMono<CameraScript>();
	_cameraTransform = _cameraScript.lock()->GetTransform();

	_rigidbody = gameObject->GetRigidbody();
}

void PlayerMovement::Start()
{
	_gameObject.lock()->Animator()->Play(Animation::Idle);
}

void PlayerMovement::Update()
{
	shared_ptr<Transform> lockTransform = _lockTransform.lock();

	if (_isLockOn)
	{

		if (lockTransform != nullptr)
		{
#if defined(TEST1_IK)
			IK_LookData lookData;
			lookData.isAssigned = true;
			lookData.lookingPos = lockTransform->GetPosition();
			_animator.lock()->SetIKData(lookData);
#elif defined(TEST2_UNDER)
			// Nothing
#endif

		}
		else
		{
			_isLockOn = false;
			_lockTransform.reset();
			_cameraScript.lock()->RemoveLockOn();
#if defined(TEST1_IK)
			_animator.lock()->RemoveIKData();
#elif defined(TEST2_UNDER)
			_animator.lock()->Remove_Test2_UnderRotateData();
#endif
		}

	}
	else
	{
		_isLockOn = false;
		_lockTransform.reset();
		_cameraScript.lock()->RemoveLockOn();
#if defined(TEST1_IK)
		_animator.lock()->RemoveIKData();
#elif defined(TEST2_UNDER)
		_animator.lock()->Remove_Test2_UnderRotateData();
#endif
	}








	shared_ptr<Transform> transform = GetTransform();
	myQuaternion currentQuat = transform->GetRotation();


	if (_isLockOn)
	{
#if defined(TEST0_NOTHING)
		Vec3 aimVec = Vec3(transform->GetPosition() - _lockTransform.lock()->GetPosition()).GetNormalize();
		Quaternion aimQuat(aimVec);
		transform->SetRotation(Quaternion::Slerp(transform->GetRotation(), aimQuat, 7.f * DT));


		if (isMoveInvoked)
		{
			float diffAngle = -aimVec.Dot(_movingVec);

			if (diffAngle >= COS_45)
			{
				Crossfade_SyncElapsedTime(Animation::AimingForward);
			}
			else if (diffAngle >= -COS_45)
			{
				Vec3 cross = aimVec.Cross(_movingVec);
				if (cross.Y < 0)
				{
					Crossfade_SyncElapsedTime(Animation::AimingRight);
				}
				else
				{
					Crossfade_SyncElapsedTime(Animation::AimingLeft);
				}
			}
			else
			{
				Crossfade_SyncElapsedTime(Animation::AimingBackward);
			}
		}
		else
		{
			Crossfade(Animation::Idle);
		}

#elif defined(TEST1_IK)
		Quaternion aimQuat;
		myVec3 aimVec = myVec3(transform->GetPosition() - _lockTransform.lock()->GetPosition()).GetNormalize();


		if (isMoveInvoked)
		{
			float diffAngle = -aimVec.Dot(_movingVec);

			if (diffAngle >= COS_45)
			{
				Crossfade_SyncElapsedTime(Animation::AimingForward);
				aimQuat.FromLookingVector(-_movingVec, Vec3::UnitY);
			}
			else if (diffAngle >= -COS_45)
			{
				Vec3 cross = aimVec.Cross(_movingVec);
				if (cross.Y < 0)
				{
					Crossfade_SyncElapsedTime(Animation::AimingRight);

					Vec3 rotatedVec = Quaternion(myVec3(0, 1, 0), -90.f).RotateVector(_movingVec);
					aimQuat.FromLookingVector(rotatedVec);
				}
				else
				{
					Crossfade_SyncElapsedTime(Animation::AimingLeft);

					Vec3 rotatedVec = Quaternion(myVec3(0, 1, 0), 90.f).RotateVector(_movingVec);
					aimQuat.FromLookingVector(rotatedVec);
				}
			}
			else
			{
				Crossfade_SyncElapsedTime(Animation::AimingBackward);
				aimQuat.FromLookingVector(_movingVec);
			}

		}
		else
		{
			Crossfade(Animation::Idle);
		}


		transform->SetRotation(Quaternion::Slerp(transform->GetRotation(), aimQuat, 7.f * DT));

#elif defined(TEST2_UNDER)
		Vec3 aimVec = Vec3(transform->GetPosition() - _lockTransform.lock()->GetPosition()).GetNormalize();
		Quaternion aimQuat(aimVec);
		transform->SetRotation(Quaternion::Slerp(transform->GetRotation(), aimQuat, 7.f * DT));

		
		if (isMoveInvoked)
		{
			_movingSlerpVec = Vec3::Lerp(_movingSlerpVec, _movingVec, 7.f * DT);

			float diffCos = -aimVec.Dot(_movingSlerpVec);
			Vec3 cross = aimVec.Cross(_movingSlerpVec);
			float degree = acosf(diffCos) * RAD_TO_DEG;


			UnderRotateData underRotateData;
			underRotateData.isAssigned = true;
			underRotateData.diffDegree = degree;
			underRotateData.crossY = cross.Y;
			underRotateData.animIndexMapping = {
				{AimingForward, UnderRotateData::FORWARD},
				{AimingRight, UnderRotateData::RIGHT},
				{AimingLeft, UnderRotateData::LEFT},
				{AimingBackward, UnderRotateData::BACKWARD}
			};


			if (diffCos >= COS_45)
			{
				Crossfade_SyncElapsedTime(Animation::AimingForward);
			}
			else if (diffCos >= -COS_45)
			{
				if (cross.Y < 0)
				{
					Crossfade_SyncElapsedTime(Animation::AimingRight);
				}
				else
				{
					Crossfade_SyncElapsedTime(Animation::AimingLeft);
				}
			}
			else
			{
				Crossfade_SyncElapsedTime(Animation::AimingBackward);
			}


			_animator.lock()->SetUnderRotateData(underRotateData);
		}
		else
		{
			Crossfade(Animation::Idle);

			UnderRotateData underRotateData;
			underRotateData.isAssigned = false;

			_animator.lock()->SetUnderRotateData(underRotateData);
		}
#endif
	}
	else
	{
		if (isMoveInvoked)
		{
			Quaternion lookingQuat(-_movingVec);
			float quatAngle = currentQuat.Dot(lookingQuat);
			if (fabs(quatAngle) < 0.9995f)
			{
				transform->SetRotation(myQuaternion::Slerp(currentQuat, lookingQuat, 7.f * DT));
			}
			else
			{
				transform->SetRotation(lookingQuat);
			}

			Crossfade(Animation::Walk);
		}
		else
		{
			Crossfade(Animation::Idle);
		}

	}



	


}

void PlayerMovement::LateUpdate()
{

}

void PlayerMovement::OnCollisionEnter(const Collision& collision)
{
	if (Layermask::Enemy == collision.other->GetLayermask())
	{
		if (collision.normal.Dot(Vec3::UnitY) > 0.5f)
		{
			collision.other->GetGameObject()->GetMono<Goomba>()->Die();
			shared_ptr<Rigidbody> rigidbody = _rigidbody.lock();
			rigidbody->velocity = Vec3::Zero;
			rigidbody->AddForce(Vec3(0.f, 50.f, 0.f));
		}
	}
}

void PlayerMovement::InvokeMove(int vertical, int horizon, float interpolSpeed/*=1.f*/)
{
	isMoveInvoked = true;

#if 0
	static float invSqrt2 = 1.f / sqrtf(2);
	float interpolation = 1.f;
	if (vertical != 0 && horizon != 0)
	{
		interpolation = invSqrt2;
	}
	else if (vertical == 0 && horizon == 0)
	{
		assert(0);
	}

	interpolation *= interpolSpeed;

	shared_ptr<Transform> transform = _gameObject.lock()->GetTransform();


	Vec3 moveVec = transform->Forward() * vertical * interpolation * _movementSpeed * DT
		+ transform->Right() * horizon * interpolation * _movementSpeed * DT;
	transform->AddPosition(moveVec);
#endif

	float angle = atan2f(static_cast<float>(vertical), static_cast<float>(horizon));
	//angle = angle * 180.f * INV_PI;
	float cos, sin;

	cos = cosf(angle);
	sin = sinf(angle);


	shared_ptr<Transform> cameraTransform = _cameraTransform.lock();
	Vec3 right = -cameraTransform->Right(); 
	Vec3 forward = right.Cross(myVec3::UnitY);


	_movingVec = right * cos + forward * sin;


	GetTransform()->AddPosition(_movingVec * _movementSpeed * interpolSpeed * DT);
}

void PlayerMovement::InvokeMove(Vec2 screenVec, float interpoalSpeed)
{
	isMoveInvoked = true;

	float cos = screenVec.Dot(Vec2(1.f, 0.f));
	float sin = screenVec.y >= 0 ? sqrtf(1.f - cos * cos) : -sqrtf(1.f - cos * cos);

	shared_ptr<Transform> cameraTransform = _cameraTransform.lock();
	Vec3 right = -cameraTransform->Right(); 
	Vec3 forward = right.Cross(myVec3::UnitY);

	_movingVec = right * cos + forward * sin;

	GetTransform()->AddPosition(_movingVec * _movementSpeed * interpoalSpeed * DT);
}

void PlayerMovement::InformNoMove()
{
	isMoveInvoked = false;
}



void PlayerMovement::InvokeJump()
{
	if (shared_ptr<Rigidbody> rigidbody = _rigidbody.lock())
	{
		if (rigidbody->IsGrouneded())
		{
			rigidbody->AddForce(myVec3(0.f, 80.f, 0.f));
		}
	}
}

void PlayerMovement::OnLockButtonClicked()
{
	if (!_isLockOn)
	{
		const GameDesc& gameDesc = GAME->GetGameDesc();

		float tempNear = 1.f;
		float tempFar = 1000.f;

		myRay ray(gameDesc.width, gameDesc.height, tempNear, tempFar, Camera::S_MatVInv, Camera::S_MatProjection,
			gameDesc.width / 2, gameDesc.height / 2);



		if (BaseCollider* retCollider = PHYSICS->CapsuleCast(ray, 40.f, Layermask::Enemy))
		{
			shared_ptr<Transform> lockTransform = retCollider->GetTransform();;
			_lockTransform = lockTransform;
			_cameraScript.lock()->RegisterLockOn(lockTransform);
			_isLockOn = true;
		}
	}
	else
	{
		_isLockOn = false;
		_lockTransform.reset();
		_cameraScript.lock()->RemoveLockOn();
#if defined(TEST1_IK)
		_animator.lock()->RemoveIKData();
#elif defined(TEST2_UNDER)
		_animator.lock()->Remove_Test2_UnderRotateData();
#endif
	}
}










shared_ptr<Component> PlayerMovement::Clone() const
{
	return make_shared<PlayerMovement>(*this);
}
