#pragma once

#include "BaseCollider.h"

class BoxCollider : public BaseCollider
{
	using Super = BaseCollider;
public:
	BoxCollider(shared_ptr<Shader> shader);
	virtual ~BoxCollider();


	virtual void Render() override;

	virtual void UpdateColliderTransform(shared_ptr<Transform> transform) override;
	
	virtual bool Intersects(const myRay& ray, OUT float& distance) override;

	virtual bool Intersects(SphereCollider* otherSphere, ContactInfo& penerationDepth) override;
	virtual bool Intersects(BoxCollider* otherBox, ContactInfo& penetrationDepth) override;
	virtual bool Intersects(CapsuleCollider* otherCapsule, ContactInfo& peneDepth) override;
	

public:
	virtual const Vec3& GetCenter() override { return _boundingBox.center; }
	virtual Vec3 GetLowestPoint() const override;
	void SetHalfExtents(Vec3 InHalfExtents) { _halfExtents = InHalfExtents; }
	Vec3& GetHalfExtents() { return _halfExtents; }
	void SetRotation(Vec3 eulerAngles) { _collidersQuaternion = Quaternion(eulerAngles); }
	Quaternion& GetQuaternion() { return _collidersQuaternion; }
	myBoundingOrientedBox& GetBoundingBox() { return _boundingBox; }

public:
	virtual shared_ptr<Component> Clone() const override;


private:
	myBoundingOrientedBox _boundingBox;
	Vec3 _halfExtents = Vec3(0.5f, 0.5f, 0.5f);
	Quaternion _collidersQuaternion;
};