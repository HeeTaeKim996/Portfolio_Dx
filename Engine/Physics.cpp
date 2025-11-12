#include "pch.h"
#include "Physics.h"
#include "SphereCollider.h"
#include "Transform.h"
#include "GameObject.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "Collision.h"
#include "Rigidbody.h"
#include "TimeManager.h"


void Physics::Initialize()
{
	proxies.clear();
	freeList.clear();
}

void Physics::Update()
{
	PhysicsUpdate();
	IntersectUpdate();
	CollisionUpdate();
}













































/*-------------------------
		   Proxy
-------------------------*/
uint32 Physics::CreateProxy(BaseCollider& collider)
{

	if (freeList.empty())
	{
		uint32 id = proxies.size();

		static uint32 limitInt = PROXY_ID_IS_NOT_ASSIGNED & 0x00'FF'FF'FF;
		assert(id < limitInt); 
		/* ※ Collider 에서 proxyId 가 할당되지 않았음으로, UINT32_MAX 를 사용한다.
			 id 가 UINT32_MAX 와 중복되는 걸 방지하기 위해, limitInt 보다 작은 id (24) 가 할당돼야 한다.
			 만약 위 assert가 발동된다면, uint32 가 아닌 uint64로 id 관리 필요 */
		ClProxy proxy(&collider, collider.GetColliderType(), id, collider.HasRigidbody());
		proxy.layerMask = collider.GetLayermask();

		proxies.push_back(proxy);

		return id;
	}
	else
	{
		uint32 id = freeList.back();
		freeList.pop_back();

		uint32 generation = (id >> 24) + 1;
		uint32 index = id & 0x00'FF'FF'FF;

		assert(proxies[index].type == ColliderType::End);

		id = (generation << 24) + index;
		ClProxy proxy(&collider, collider.GetColliderType(), id, collider.HasRigidbody());

		proxies[index] = proxy;

		return id;
	}

	{ // DEBUG
		if (proxies.size() > 2)
			assert(0);
	}

}



void Physics::RemoveProxy(uint32 id)
{
	uint8 generation = id >> 24;
	uint32 index = id & 0x00'FF'FF'FF;

	assert(proxies[index].GetId() == id);

	proxies[index] = ClProxy();

	freeList.push_back(id);
}

bool Physics::GetProxy(uint32 id, ClProxy*& proxy)
{
	uint32 index = id & 0x00'FF'FF'FF;

	if (proxies[index].GetId() != id)
	{
		return false;
	}

	proxy = &proxies[index];
	return true;
}


#if 0 // Obsolate
ClProxy& Physics::GetProxy(uint32 id)
{
	uint32 index = id & 0x00'FF'FF'FF;
	
	if (proxies[index].GetId() != id)
	{
		return ClProxy();
	}

	return proxies[index];
}
#endif





















/*-------------------------
		  Collision
-------------------------*/
void Physics::IntersectUpdate()
{
	static bool isForwardIteration = false;


	uint32 size = proxies.size();



	for (uint32 i = 0; i < size; i++)
	{
		ClProxy& aProxy = proxies[i];
		if (aProxy.type == ColliderType::End || aProxy.isDirty == false)

			continue;

		bool didMove = false;


		for (uint32 j = 0; j < size; j++)
		{
			if (i == j)
				continue;

			ClProxy& bProxy = proxies[j];
			if (bProxy.type == ColliderType::End)
				continue;

			if (!aProxy.hasRigidbody && !bProxy.hasRigidbody)
				continue;


			if (CompareAABB(aProxy, bProxy))
			{
				if (NarrowPhase(aProxy, bProxy))
				{
					didMove = true;
				}
			}
		}


		if (didMove == false)
		{
			aProxy.isDirty = false;
		}

	}
}


bool Physics::CompareAABB(const ClProxy& a, const ClProxy& b)
{
	if (a.AABB.min.X > b.AABB.max.X || a.AABB.max.X < b.AABB.min.X) return false;
	if (a.AABB.min.Y > b.AABB.max.Y || a.AABB.max.Y < b.AABB.min.Y) return false;
	if (a.AABB.min.Z > b.AABB.max.Z || a.AABB.max.Z < b.AABB.min.Z) return false;



	return true;
}

bool Physics::NarrowPhase(const ClProxy& a, const ClProxy& b)
{
	ColliderType aType = a.type;
	ColliderType bType = b.type;

	ContactInfo contactInfo;


	if (aType == ColliderType::Sphere)
	{
		/*
			- 주의. collisionInfo 의 normal이 뒤집히지 않도록, aCollider 에서 Intersects 함수 호출 필요
			- myMath 에서 this <- other 방향으로 normal을 정함
		*/
		SphereCollider* aSphere = static_cast<SphereCollider*>(a.ptr);
		if (bType == ColliderType::Sphere)
		{
			SphereCollider* bSphere = static_cast<SphereCollider*>(b.ptr);
			if (!aSphere->Intersects(bSphere, contactInfo))
				return false;
		}
		else if (bType == ColliderType::OBB)
		{
			BoxCollider* bObb = static_cast<BoxCollider*>(b.ptr);
			if (!aSphere->Intersects(bObb, contactInfo))
				return false;
		}
		else if (bType == ColliderType::Capsule)
		{
			CapsuleCollider* bCapsule = static_cast<CapsuleCollider*>(b.ptr);
			if(!aSphere->Intersects(bCapsule, contactInfo))
				return false;
		}
	}
	else if (aType == ColliderType::OBB)
	{
		BoxCollider* aBox = static_cast<BoxCollider*>(a.ptr);
		if (bType == ColliderType::Sphere)
		{
			SphereCollider* bSphere = static_cast<SphereCollider*>(b.ptr);
			if (!aBox->Intersects(bSphere, contactInfo))
				return false;
		}
		else if (bType == ColliderType::OBB)
		{
			BoxCollider* bBox = static_cast<BoxCollider*>(b.ptr);
			if (!aBox->Intersects(bBox, contactInfo))
				return false;
		}
		else if (bType == ColliderType::Capsule)
		{
			CapsuleCollider* bCapsule = static_cast<CapsuleCollider*>(b.ptr);
			if (!aBox->Intersects(bCapsule, contactInfo))
				return false;
		}
	}
	else if (aType == ColliderType::Capsule)
	{
		CapsuleCollider* aCapsule = static_cast<CapsuleCollider*>(a.ptr);
		if (bType == ColliderType::Sphere)
		{
			SphereCollider* bSphere = static_cast<SphereCollider*>(b.ptr);
			if (!aCapsule->Intersects(bSphere, contactInfo))
				return false;
		}
		else if (bType == ColliderType::OBB)
		{
			BoxCollider* bBox = static_cast<BoxCollider*>(b.ptr);
			if (!aCapsule->Intersects(bBox, contactInfo))
				return false;
		}
		else if (bType == ColliderType::Capsule)
		{
			CapsuleCollider* bCapsule = static_cast<CapsuleCollider*>(b.ptr);
			if (!aCapsule->Intersects(bCapsule, contactInfo))
				return false;
		}
	}


	uint64 key = ComputeCollisionKey(a, b, contactInfo);
	if (currContactInfos.find(key) == currContactInfos.end())
		currContactInfos[key] = contactInfo;



	if ((contactInfo.penetration > CONTACT_SLOP) == false)
		return false;


	Vec3 aTob = (b.ptr->GetCenter() - a.ptr->GetCenter()).GetNormalize();
	aTob *= (contactInfo.penetration - CONTACT_SLOP);

	float ratio;

	if (!b.hasRigidbody)
	{
		ratio = 1.f;
	}
	else if (!a.hasRigidbody)
	{
		ratio = 0.f;
	}
	else
	{
		int aMass = a.ptr->GetRigiMass();
		int bMass = b.ptr->GetRigiMass();

		if (aMass == 0 && bMass == 0)
		{
			ratio = 0.5f;
		}
		else
		{
			ratio = static_cast<float>(bMass) / (aMass + bMass);
		}
	}



	Vec3 aMoveVec = -aTob * ratio;
	Vec3 bMoveVec = aTob * (1.f - ratio);


	shared_ptr<Transform> aTransform = a.ptr->GetGameObject()->GetTransform();
	shared_ptr<Transform> bTransform = b.ptr->GetGameObject()->GetTransform();

	if (ratio != 0.f && ratio != 1.f)
	{
		aTransform->AddPosition(aMoveVec);
		bTransform->AddPosition(bMoveVec);
	}
	else if (ratio == 1.f)
	{
		aTransform->AddPosition(aMoveVec);
	}
	else
	{
		bTransform->AddPosition(bMoveVec);
	}



	return true;
}

void Physics::CollisionUpdate()
{
	for (::pair<const uint64, ContactInfo>& p : currContactInfos)
	{
		if (prevContactInfos.find(p.first) == prevContactInfos.end())
		{
			OnCollisionEnter(p.second, p.first);
		}
		else
		{
			OnCollisionStay(p.second, p.first);
		}
	}


	for (::pair<const uint64, ContactInfo>& p : prevContactInfos)
	{
		if (currContactInfos.find(p.first) == currContactInfos.end())
		{
			OnCollisionExit(p.second, p.first);
		}
	}


	prevContactInfos.swap(currContactInfos);
	currContactInfos.clear();
}

uint64 Physics::ComputeCollisionKey(const ClProxy& a, const ClProxy& b, ContactInfo& contactInfo)
{
	uint32 first = a.GetId();
	uint32 second = b.GetId();

	if (first > second)
	{
		uint32 temp = first;
		first = second;
		second = temp;

		contactInfo.normal = -contactInfo.normal;
	}

	return (static_cast<uint64>(first) << 32) | second;
}

void Physics::OnCollisionEnter(const ContactInfo& info, uint64 key)
{
	ClProxy* aProxy = nullptr;
	if (!GetProxy(key >> 32, aProxy))
		return;
	BaseCollider* aCollider = aProxy->ptr;

	ClProxy* bProxy = nullptr;
	if (!GetProxy(static_cast<uint32>(key), bProxy))
		return;
	BaseCollider* bCollider = bProxy->ptr;

	{
		Collision collision;

		collision.normal = info.normal;
		collision.other = static_pointer_cast<BaseCollider>(bCollider->shared_from_this());

		aCollider->OnCollisionEnter(collision);
	}

	{
		Collision collision;

		collision.normal = -info.normal;
		collision.other = static_pointer_cast<BaseCollider>(aCollider->shared_from_this());

		bCollider->OnCollisionEnter(collision);
	}
}

void Physics::OnCollisionStay(const ContactInfo& info, uint64 key)
{
#if 1
	ClProxy* aProxy = nullptr;
	if (!GetProxy(key >> 32, aProxy))
		return;
	BaseCollider* aCollider = aProxy->ptr;

	ClProxy* bProxy = nullptr;
	if (!GetProxy(static_cast<uint32>(key), bProxy))
		return;
	BaseCollider* bCollider = bProxy->ptr;

	{
		Collision collision;

		collision.normal = info.normal;
		collision.other = static_pointer_cast<BaseCollider>(bCollider->shared_from_this());

		aCollider->OnCollisionStay(collision);
	}

	{
		Collision collision;

		collision.normal = -info.normal;
		collision.other = static_pointer_cast<BaseCollider>(aCollider->shared_from_this());

		bCollider->OnCollisionStay(collision);
	}
#endif
}

void Physics::OnCollisionExit(const ContactInfo& info, uint64 key)
{
#if 1
	ClProxy* aProxy = nullptr;
	if (!GetProxy(key >> 32, aProxy))
		return;
	BaseCollider* aCollider = aProxy->ptr;

	ClProxy* bProxy = nullptr;
	if (!GetProxy(static_cast<uint32>(key), bProxy))
		return;
	BaseCollider* bCollider = bProxy->ptr;

	{
		Collision collision;

		collision.normal = info.normal;
		collision.other = static_pointer_cast<BaseCollider>(bCollider->shared_from_this());

		aCollider->OnCollisionExit(collision);
	}

	{
		Collision collision;

		collision.normal = -info.normal;
		collision.other = static_pointer_cast<BaseCollider>(aCollider->shared_from_this());

		bCollider->OnCollisionExit(collision);
	}
#endif
}




















































/*--------------------------
		   Physics
---------------------------*/
void Physics::PhysicsUpdate()
{
	uint32 size = proxies.size();

	for (uint32 i = 0; i < size; i++)
	{
		ClProxy& proxy = proxies[i];

		if (!proxy.hasRigidbody)
			continue;

		shared_ptr<Rigidbody> rigidbody = proxy.ptr->GetRigidbody();
		shared_ptr<Transform> transform = proxy.ptr->GetGameObject()->GetTransform();

		if (proxy.isDirty) // Transform 에서 이미 isDirty 처리했다면, Rigidbody 연산 전 포지션 업데이트 필요
		{
			rigidbody->position = transform->GetPosition();
		}


		if (UpdateRigidbody(rigidbody))
		{
			//proxy.isDirty = true; // Rigidbody 로 움직였으므로, Intersects 테스트 필요 
			// ※ 현재 코드에서는 아래 SetPosition 에서 자동 호출
			
			transform->SetPosition(rigidbody->position); 
			// Transofrm 을 굳이 여기서 Update해야 하나 싶지만, 현재 코드에는 UpdateColliderTransofrm 을 하려면 Transform을 갱신해야 돼서,
			// 이렇게 처리함. SetPosition호출시 UpdateColliderTransofrm 자동 호출
		}


		

	}
}

bool Physics::Raycast(const myRay& ray, BaseCollider* retCollider)
{
	float minDist = FLT_MAX;

	for (int i = 0; i < proxies.size(); i++)
	{
		const ClProxy& proxy = proxies[i];
		float distance;
		if (ray.Intersect(proxy.AABB.min, proxy.AABB.max, distance))
		{
			BaseCollider* collider = proxy.ptr;
			if (collider->Intersects(ray, distance))
			{
				if (distance < minDist)
				{
					minDist = distance;
					retCollider = collider;
				}
			}
		}
	}

	return minDist != FLT_MAX;
}

bool Physics::Raycast(const myRay& ray, BaseCollider* retCollider, Layermask layermask)
{
	uint8 layer;
	if (layermask == Layermask::End)
	{
		layer = (((uint8)Layermask::End - 1) << 1) - 1;
	}
	else
	{
		layer = (uint8)layermask;
	}



	float minDist = FLT_MAX;

	for (int i = 0; i < proxies.size(); i++)
	{
		const ClProxy& proxy = proxies[i];

		uint8 proxyLayer = (uint8)proxy.layerMask;
		if ((layer & proxyLayer) == 0)
		{
			continue;
		}

		float distance;
		if (ray.Intersect(proxy.AABB.min, proxy.AABB.max, distance))
		{
			BaseCollider* collider = proxy.ptr;
			if (collider->Intersects(ray, distance))
			{
				if (distance < minDist)
				{
					minDist = distance;
					retCollider = collider;
				}
			}
		}
	}

	return minDist != FLT_MAX;
}

BaseCollider* Physics::CapsuleCast(const myRay& ray, float radius, Layermask layerMask/*=ALL*/)
{
	uint8 layer = (uint8)layerMask;

	myBoundingCapsule capsule;
	capsule.bottom = ray.origin;
	capsule.top = ray.origin + ray.dir * ray.magnitude;
	capsule.radius = radius;

	BaseCollider* retCollider = nullptr;
	float maxDist = -FLT_MAX;

	for (int i = 0; i < proxies.size(); i++)
	{
		const ClProxy& proxy = proxies[i];

		uint8 proxyLayer = (uint8)proxy.layerMask;
		if ((layer & proxyLayer) == 0)
		{
			continue;
		}


		if(capsule.Intersects(proxy.AABB.min, proxy.AABB.max))
		{
			BaseCollider* collider = proxy.ptr;

			ContactInfo contactInfo;
			switch (collider->GetColliderType())
			{
			case ColliderType::Sphere:
			{
				SphereCollider* sphereCollider = static_cast<SphereCollider*>(collider);
				myBoundingSphere& boundingSphere = sphereCollider->GetBoundingSphere();

				if (capsule.Intersects(boundingSphere, contactInfo))
				{
					if (contactInfo.penetration > maxDist)
					{
						maxDist = contactInfo.penetration;
						retCollider = collider;
					}
				}


				break;
			}
			case ColliderType::OBB:
			{
				BoxCollider* boxCollider = static_cast<BoxCollider*>(collider);
				myBoundingOrientedBox& obb = boxCollider->GetBoundingBox();

				if (capsule.Intersects(obb, contactInfo))
				{
					if (contactInfo.penetration > maxDist)
					{
						maxDist = contactInfo.penetration;
						retCollider = collider;
					}
				}

				break;
			}
			case ColliderType::Capsule:
			{
				CapsuleCollider* capsuleCollider = static_cast<CapsuleCollider*>(collider);
				myBoundingCapsule& otherCapsule = capsuleCollider->GetBoundingCapsule();

				if (capsule.Intersects(otherCapsule, contactInfo))
				{
					if (contactInfo.penetration > maxDist)
					{
						maxDist = contactInfo.penetration;
						retCollider = collider;
					}
				}


				break;
			}
			default:
			{
				assert(0);
			}
			}
		}	
	}

	return retCollider;
}

vector<::pair<BaseCollider*, ContactInfo>> Physics::Sphere(const myVec3& center, const float& radius, Layermask layerMask/*=Layermask::ALL*/)
{
	uint8 layer = (uint8)layerMask;

	myBoundingSphere sphere;
	sphere.center = center;
	sphere.radius = radius;

	vector<::pair<BaseCollider*, ContactInfo>> ret;

	for (int i = 0; i < proxies.size(); i++)
	{
		const ClProxy& proxy = proxies[i];

		uint8 proxyLayer = (uint8)proxy.layerMask;
		if ((layer & proxyLayer) == 0)
			continue;

		if (sphere.Intersects(proxy.AABB.min, proxy.AABB.max))
		{
			BaseCollider* collider = proxy.ptr;

			ContactInfo contactInfo;
			switch (collider->GetColliderType())
			{
			case ColliderType::Sphere:
			{
				SphereCollider* sphereCollider = static_cast<SphereCollider*>(collider);
				myBoundingSphere& otherSphere = sphereCollider->GetBoundingSphere();

				if (sphere.Intersects(otherSphere, contactInfo))
				{
					ret.emplace_back(collider, contactInfo);
				}

				break;
			}

			case ColliderType::OBB:
			{
				BoxCollider* boxCollider = static_cast<BoxCollider*>(collider);
				myBoundingOrientedBox& otherObb = boxCollider->GetBoundingBox();

				if (sphere.Intersects(otherObb, contactInfo))
				{
					ret.emplace_back(collider, contactInfo);
				}

				break;
			}

			case ColliderType::Capsule:
			{
				CapsuleCollider* capsuleCollider = static_cast<CapsuleCollider*>(collider);
				myBoundingCapsule& otherCapsule = capsuleCollider->GetBoundingCapsule();

				if (sphere.Intersects(otherCapsule, contactInfo))
				{
					ret.emplace_back(collider, contactInfo);
				}

				break;
			}

			default:
			{
				assert(0);
			}
			}
		}
	}

	return ret;
}



bool Physics::UpdateRigidbody(shared_ptr<Rigidbody> rigidbody)
{
	Vec3 velocity = rigidbody->velocity;

	if (velocity.SizeSquared() < 1e-6f)
		return false;

	rigidbody->position = rigidbody->position + velocity * DT;
	
	return true;
}

