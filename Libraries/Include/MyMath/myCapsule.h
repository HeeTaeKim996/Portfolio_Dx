#pragma once
#include "myVec3.h"

class myRay;
class myBoundingOrientedBox;

class myCapsule
{
public:
	bool Intersects(const myRay& ray, float& distance);

	void ComputeAABB(myVec3& min, myVec3& max) const;

	bool Intersects_OBB(const myBoundingOrientedBox& otherObb, float& peneDepth);


public:
	myVec3 bottom;
	myVec3 top;
	float radius;
};

