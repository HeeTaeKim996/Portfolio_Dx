#pragma once

#include "myVec3.h"
#include "myMatrix3x3.h"

class myBoundingSphere;
class myRay;
struct ContactInfo;
class myBoundingCapsule;

class myBoundingOrientedBox
{
public:
	myBoundingOrientedBox() {}

	bool Intersects(const myRay& ray, float& distance) const;

	void ComputeAABB(myVec3& min, myVec3& max) const;
	
	bool Intersects_OBB(const myBoundingOrientedBox& otherObb, ContactInfo& peneDepth) const;
	
	bool Intersects_Sphere(const myBoundingSphere& sphere, ContactInfo& peneDepth) const;
	bool Intersects(const myBoundingCapsule& capsule, ContactInfo& contactInfo) const;

public:
	myVec3 ClosestPoint(const myVec3& point) const;
	myVec3 LowestPoint() const;

public:
	myVec3 center;
	myVec3 halfExtents;
	myMatrix3x3 axes;
};

