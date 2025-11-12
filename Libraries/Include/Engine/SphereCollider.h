#pragma once
#include "BaseCollider.h"
class SphereCollider : public BaseCollider
{
	using Super = BaseCollider;
public:
	SphereCollider(shared_ptr<Shader> shader);
	virtual ~SphereCollider();



	virtual void Render() override;

	virtual void UpdateColliderTransform(shared_ptr<Transform> transform) override;

	virtual bool Intersects(const myRay& ray, OUT float& distance) override;
	virtual bool Intersects(SphereCollider* otherSphere, ContactInfo& penetrationDepth) override;
	virtual bool Intersects(BoxCollider* otherBox, ContactInfo& penetrationDepth) override;
	virtual bool Intersects(CapsuleCollider* otherCapsule, ContactInfo& peneDepth) override;

public:
	virtual const Vec3& GetCenter() override { return _boundingSphere.center; }
	virtual Vec3 GetLowestPoint() const override;

	void SetRadius(float radius) { _radius = radius; }
	float& GetRadius() { return _radius; }

	myBoundingSphere& GetBoundingSphere() { return _boundingSphere; }

public:
	virtual shared_ptr<Component> Clone() const override;

private:
	float _radius = 1.f;
	myBoundingSphere _boundingSphere;

};

