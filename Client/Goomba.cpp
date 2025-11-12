#include "pch.h"
#include "Goomba.h"
#include "Engine/TimerManager.h"
#include "PlayerMovement.h"
#include "Engine/Rigidbody.h"
#include "Engine/BaseCollider.h"
#include "Engine/Physics.h"
#include "Engine/TimeManager.h"
#include "Engine/Transform.h"
#include "Engine/Scene.h"
#include "Engine/ModelAnimator.h"

void Goomba::Awake()
{
	shared_ptr<GameObject> gameObject = GetGameObject();
	AnimInitialize(gameObject->Animator());

}

void Goomba::Start()
{
	Crossfade(Animation::Idle);
}

void Goomba::Update()
{
	if (_dead) return;

	shared_ptr<GameObject> gameObject = GetGameObject();
	shared_ptr<Transform> transform = gameObject->GetTransform();

	if (_player.expired())
	{
		if (!isLayderOn)
		{
			StartDetectPlayer();
		}

		Crossfade(Animation::Idle);
	}
	else
	{
		Crossfade(Animation::Walk);

		assert(transform != nullptr);

		Vec3 toVec = (transform->GetPosition() - _player.lock()->GetTransform()->GetPosition());
		toVec.Y = 0;
		toVec.Normalize();

		Quaternion aimQuat(toVec);
		transform->SetRotation(Quaternion::Slerp(transform->GetRotation(), aimQuat, 7.f * DT));

		transform->AddPosition(-toVec * 15.f * DT);
	}
}

void Goomba::Die()
{
	_dead = true;
	GetGameObject()->GetCollider()->Disable();
	Vec3 scale = GetTransform()->GetScale();
	scale.Y *= 0.1f;
	GetTransform()->SetScale(scale);
	_animator.lock()->isPlayingAnimation = false;

	DoTimer(1000, &Goomba::Destry);
}

void Goomba::Destry()
{
	GetGameObject()->GetScene()->DestroyGameObject(GetGameObject());
}

void Goomba::StartDetectPlayer()
{
	if (_player.expired() && !isLayderOn && !_dead)
	{
		isLayderOn = true;
		DetectPlayer();
	}
}

void Goomba::DetectPlayer()
{

	if (_player.expired() && isLayderOn)
	{
		vector<::pair<BaseCollider*, ContactInfo>> detected = PHYSICS->Sphere(GetTransform()->GetPosition(), 50.f, Layermask::Player);

		if (detected.size() > 0)
		{
			_player = detected[0].first->GetGameObject()->GetMono<PlayerMovement>();

			assert(_player.lock() != nullptr);
			isLayderOn = false;
		}
		else
		{
			DoTimer(500, &Goomba::DetectPlayer);
		}

	}
	else
	{
		isLayderOn = false;
	}
}

shared_ptr<Component> Goomba::Clone() const
{
	shared_ptr<Goomba> copy = make_shared<Goomba>(*this);

	return copy;
}
