#pragma once
#include "myVec3.h"

class myRay;
class myBoundingSphere;
class myBoundingOrientedBox;
struct ContactInfo;

class myBoundingCapsule
{
public:
	bool Intersects(const myRay& ray, float& distance) const;

	void ComputeAABB(myVec3& min, myVec3& max) const;

	bool Intersects(const myBoundingSphere& otherSphere, ContactInfo& peneDepth) const;
	bool Intersects(const myBoundingOrientedBox& otherObb, ContactInfo& peneDepth) const;
	bool Intersects(const myBoundingCapsule& otherCapsule, ContactInfo& peneDepth) const;
	bool Intersects(const myVec3& minAABB, const myVec3& maxAABB) const;

public:
	myVec3 LowestPoint() const;


public:
	myVec3 bottom;
	myVec3 top;
	float radius;
};

