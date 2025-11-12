#pragma once

#include "myVec3.h"

class myMatrix4x4;

class myRay
{
	using uint32 = unsigned long;
	using int32 = long;
public:
	myRay(float width, float height, float disNear, float disFar, myMatrix4x4& vInv, const myMatrix4x4& projection, int32 screenX, int32 screenY);
	myRay(myVec3 InOrigin, myVec3 InDir, float InMagnitude) : origin(InOrigin), dir(InDir), magnitude(InMagnitude) {}



public:
	bool Intersect(const myVec3& minAABB, const myVec3& maxAABB, float& distance) const;

public:
	myVec3 origin;
	myVec3 dir;
	float magnitude;
};