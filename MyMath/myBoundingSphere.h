#pragma once

#include "myVec3.h"

class myRay;
struct ContactInfo;
class myBoundingOrientedBox;
class myBoundingCapsule;

class myBoundingSphere
{
public:
	myBoundingSphere() {}

	void ComputeAABB(myVec3& min, myVec3& max) const;

	bool Intersects(const myRay& ray, float& contactInfo) const;

	bool Intersects(const myVec3& minAABB, const myVec3& maxAABB) const;
	bool Intersects(const myBoundingSphere& otherSphere, ContactInfo& contactInfo) const;
	bool Intersects(const myBoundingOrientedBox& obb, ContactInfo& contactInfo) const;
	bool Intersects(const myBoundingCapsule& capsule, ContactInfo& contactInfo) const;

public:
	myVec3 LosestPoint() const;



public:
	myVec3 center;
	float radius;
};