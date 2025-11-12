#pragma once
#include "BaseCollider.h"

class CapsuleCollider : public BaseCollider
{
	using Super = BaseCollider;
public:
	CapsuleCollider(shared_ptr<Shader> shader);
	virtual ~CapsuleCollider();

	virtual void Render() override;

	virtual void UpdateColliderTransform(shared_ptr<Transform> transform) override;




	virtual bool Intersects(const myRay& ray, OUT float& distance) override;

	virtual bool Intersects(SphereCollider* otherSphere, ContactInfo& penerationDepth) override;
	virtual bool Intersects(BoxCollider* otherBox, ContactInfo& penetrationDepth) override;
	virtual bool Intersects(CapsuleCollider* otherCapsule, ContactInfo& peneDepth) override;


public:
	virtual const Vec3& GetCenter() override { return (_boundingCapsule.bottom + _boundingCapsule.top) * 0.5f; }
	virtual Vec3 GetLowestPoint() const override;

	myBoundingCapsule& GetBoundingCapsule() { return _boundingCapsule; }
	void SetRotation(Vec3 eulerAngles) { _capsuleQuaternion = Quaternion(eulerAngles); }
	void SetRadius(float radius) { _radius = radius; }
	void SetHalfHeight(float halfHeight) { _halfHeight = halfHeight; }

	float& GetRadius() { return _radius; }
	float& GetHalfHeight() { return _halfHeight; }
	Quaternion& GetQuaternion() { return _capsuleQuaternion; }

public:
	virtual shared_ptr<Component> Clone() const override;

private:
	myBoundingCapsule _boundingCapsule;
	float _halfHeight = 40.f;
	float _radius;
	Quaternion _capsuleQuaternion;
private:
	shared_ptr<Mesh> tempAdditionalMesh;
};

