#pragma once
#include "Component.h"

#include "MeshRenderer.h"


class Transform;
class SphereCollider;
class BoxCollider;
class CapsuleCollider;
class Rigidbody;

enum class ColliderType
{
	Sphere,
	AABB,
	OBB,
	Capsule,

	End
};


struct ClAABB
{
	Vec3 min;
	Vec3 max;
};




struct ClProxy
{
	ClProxy() : type(ColliderType::End) {}
	ClProxy(BaseCollider* InPtr, ColliderType InType, uint32 InId, bool InHasRigidbody) 
		: ptr(InPtr), type(InType), id(InId), hasRigidbody(InHasRigidbody) {}

	BaseCollider* ptr;
	ColliderType type;
	ClAABB AABB;
	bool isDirty = true;
	bool hasRigidbody = false;
	Layermask layerMask = Layermask::None;

public:
	uint8 GetGeneration() { return id >> 24; }
	uint32 GetIndex() { return id & 0x00'FF'FF'FF; }
	uint32 GetId() const { return id; }

private:
	uint32 id;
};


enum : uint32
{
	PROXY_ID_IS_NOT_ASSIGNED = UINT32_MAX,
};



class BaseCollider : public Component
{
	using Super = Component;
public:
	BaseCollider(ColliderType colliderType, shared_ptr<Shader> shader);
	virtual ~BaseCollider();


	virtual void OnEnable() final;
	virtual void OnDisable() final;


	virtual void Render() {}

	virtual void OnCollisionEnter(const Collision& collision);
	virtual void OnCollisionStay(const Collision& collision);
	virtual void OnCollisionExit(const Collision& collision);


public:
	virtual void UpdateColliderTransform(shared_ptr<Transform> transform) =0;

	virtual bool Intersects(const myRay& ray, OUT float& distance) = 0;
	virtual bool Intersects(SphereCollider* otherSphere, ContactInfo& penetrationDepth) = 0;
	virtual bool Intersects(BoxCollider* otherBox, ContactInfo& penetrationDepth) = 0;
	virtual bool Intersects(CapsuleCollider* otherCapsule, ContactInfo& peneDepth) = 0;


public:
	virtual const Vec3& GetCenter() = 0;
	virtual Vec3 GetLowestPoint() const = 0;
	void RegisterRigidbody(shared_ptr<Rigidbody> rigidbody);
	void RemoveRigidbody();

public:
	ColliderType GetColliderType() { return _colliderType; }

	void SetOffset(Vec3 offset) { _offset = offset; }
	Vec3& GetOffset() { return _offset; }

	bool HasRigidbody() { return (_rigidbody.lock() != nullptr); }
	void SetRigiMass(uint32 mass) { _rigidbodyMass = mass; }
	uint32 GetRigiMass() { return _rigidbodyMass; }

	shared_ptr<Rigidbody> GetRigidbody() { return _rigidbody.lock(); }

	Layermask GetLayermask() { return _layermask; }

public:
	virtual shared_ptr<Component> Clone() const = 0;
protected:
	ColliderType _colliderType;
	Vec3 _offset;
	shared_ptr<Mesh> _mesh;
	shared_ptr<Shader> _shader;
	uint32 proxyId = PROXY_ID_IS_NOT_ASSIGNED;
	uint32 _rigidbodyMass = 50;
	weak_ptr<Rigidbody>  _rigidbody;
	Layermask _layermask;
};

