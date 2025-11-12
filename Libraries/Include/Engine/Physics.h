#pragma once

#include "BaseCollider.h"
#include <set>

struct Collision;

class Physics
{
private:
	Physics() {}

public:
	static Physics* GetInstance()
	{
		static Physics instance;
		return &instance;
	}

public:
	void Initialize();
	void Update();


	/* Proxy */
public:
	uint32 CreateProxy(BaseCollider& collider );
	void RemoveProxy(uint32 id);
	
	bool GetProxy(uint32 id, ClProxy*& proxy);


private:
	vector<ClProxy> proxies;
	vector<uint32> freeList; 




	/* Collision */
public:
	void IntersectUpdate();
	bool CompareAABB(const ClProxy& a, const ClProxy& b);
	bool NarrowPhase(const ClProxy& a, const ClProxy& b);


	void CollisionUpdate();

private:
	uint64 ComputeCollisionKey(const ClProxy& a, const ClProxy& b, ContactInfo& contactInfo);
	void OnCollisionEnter(const ContactInfo& info, uint64 key);
	void OnCollisionStay(const ContactInfo& info, uint64 key);
	void OnCollisionExit(const ContactInfo& info, uint64 key);

private:
	unordered_map<uint64, ContactInfo> prevContactInfos;
	unordered_map<uint64, ContactInfo> currContactInfos;





	/* Physics */
public:
	void PhysicsUpdate();

	bool Raycast(const myRay& ray, BaseCollider* retCollider, Layermask layermask = Layermask::ALL);
	BaseCollider* CapsuleCast(const myRay& ray, float radius, Layermask layerMask = Layermask::ALL);
	vector<::pair<BaseCollider*, ContactInfo>> Sphere(const myVec3& center, const float& radius, Layermask layerMask = Layermask::ALL);

private:
	bool UpdateRigidbody(shared_ptr<Rigidbody> rigidbody);
};