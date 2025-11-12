#include "pch.h"
#include "Rigidbody.h"
#include "BaseCollider.h"
#include "Collision.h"
#include "TimeManager.h"
#include "Physics.h"


Rigidbody::Rigidbody() : Super(ComponentType::Rigidbody)
{
}

Rigidbody::~Rigidbody()
{
}

void Rigidbody::OnEnable()
{
	if (shared_ptr<BaseCollider> collider = GetGameObject()->GetCollider())
	{
		collider->RegisterRigidbody(static_pointer_cast<Rigidbody>(shared_from_this()));
		RegisterCollider(collider);
	}
}

void Rigidbody::OnDisable()
{
	if (shared_ptr<BaseCollider> collider = GetGameObject()->GetCollider())
	{
		collider->RemoveRigidbody();
	}
}

void Rigidbody::Update()
{
	if (useGravity)
	{
		if (!_isGrounded)
		{
			if (velocity.Y > TERMINAL_FALL_SPEED)
			{
				velocity.Y -= GRAVITY * DT;
				if (velocity.Y < TERMINAL_FALL_SPEED) velocity.Y = TERMINAL_FALL_SPEED;
			}
		}
		else
		{
			if (!CheckIsGrounded())
			{
				_isGrounded = false;
			}
		}
	}
}

void Rigidbody::OnCollisionEnter(const Collision& collision)
{
	if (useGravity && !_isGrounded)
	{
		if (Vec3::UnitY.Dot(collision.normal) > 0.5f) // ¡Ø cos(60) == 0.5f;
		{
			_isGrounded = true;
			velocity.Y = 0;
		}
	}
}

void Rigidbody::OnCollisionStay(const Collision& collision)
{
}

void Rigidbody::OnCollisionExit(const Collision& collision)
{
}

void Rigidbody::RegisterCollider(shared_ptr<BaseCollider> collider)
{
	_collider = collider;
}

void Rigidbody::RemoveCollider()
{
	_collider.reset();
}



bool Rigidbody::CheckIsGrounded()
{
	if (shared_ptr<BaseCollider> collider = _collider.lock())
	{
		Vec3 lowestPoint = collider->GetLowestPoint() - myVec3::UnitY * CONTACT_SLOP;
		
		myRay ray(lowestPoint, -myVec3::UnitY, 13.f);

		BaseCollider* retCollider = nullptr;
		if (PHYSICS->Raycast(ray, retCollider))
		{
			assert(retCollider != collider.get()); // DEBUG

			return true;
		}
	}
	else
	{
		//assert(0);
	}

	return false;
}




shared_ptr<Component> Rigidbody::Clone() const
{
	shared_ptr<Rigidbody> copy = make_shared<Rigidbody>(*this);

	return copy;
}
