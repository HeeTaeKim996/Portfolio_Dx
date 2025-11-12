#include "pch.h"
#include "BaseCollider.h"
#include "Mesh.h"
#include "Transform.h"
#include "Physics.h"
#include "GameObject.h"
#include "Rigidbody.h"

BaseCollider::BaseCollider(ColliderType colliderType, shared_ptr<Shader> shader)
	: Super(ComponentType::Collider), _colliderType(colliderType), _shader(shader)
{
	_mesh = nullptr; // 상속에서 정의
}

BaseCollider::~BaseCollider()
{
}


void BaseCollider::OnEnable()
{
	_layermask = GetGameObject()->GetLayermask();


	proxyId = PHYSICS->CreateProxy(*this);
	shared_ptr<Transform> transform = GetGameObject()->GetTransform();
	assert(transform != nullptr);
	transform->InitializeCollider(static_pointer_cast<BaseCollider>(shared_from_this()));


	if (shared_ptr<Rigidbody> rigidbody = GetGameObject()->GetRigidbody())
	{
		_rigidbody = rigidbody;
		//ClProxy& proxy = PHYSICS->GetProxy(proxyId);

		ClProxy* proxy = nullptr;
		if (!PHYSICS->GetProxy(proxyId, proxy))
			return;

		proxy->hasRigidbody = true;
	}
	
}

void BaseCollider::OnDisable()
{
	PHYSICS->RemoveProxy(proxyId);
	proxyId = PROXY_ID_IS_NOT_ASSIGNED;
	shared_ptr<Transform> transform = GetGameObject()->GetTransform();
	assert(transform != nullptr);
	transform->RemoveCollider();

	if (shared_ptr<Rigidbody> rigidbody = _rigidbody.lock())
	{
		rigidbody->RemoveCollider();
	}
}

void BaseCollider::OnCollisionEnter(const Collision& collision)
{	
	GetGameObject()->OnCollisionEnter(collision);
}

void BaseCollider::OnCollisionStay(const Collision& collision)
{
	GetGameObject()->OnCollisionStay(collision);
}

void BaseCollider::OnCollisionExit(const Collision& collision)
{
	GetGameObject()->OnCollisionExit(collision);
}

void BaseCollider::RegisterRigidbody(shared_ptr<Rigidbody> rigidbody)
{
	if (!_rigidbody.lock())
	{
		_rigidbody = rigidbody;
		if (proxyId != PROXY_ID_IS_NOT_ASSIGNED)
		{
			//ClProxy& proxy = PHYSICS->GetProxy(proxyId);
			ClProxy* proxy;
			if (!PHYSICS->GetProxy(proxyId, proxy))
				return;
			proxy->hasRigidbody = true;
		}
	}
}

void BaseCollider::RemoveRigidbody()
{
	_rigidbody.reset();

	if (proxyId != PROXY_ID_IS_NOT_ASSIGNED)
	{
		//ClProxy& proxy = PHYSICS->GetProxy(proxyId);
		ClProxy* proxy;
		if (!PHYSICS->GetProxy(proxyId, proxy))
			return;
		proxy->hasRigidbody = false;
	}
}


