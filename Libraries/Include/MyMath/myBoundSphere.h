#pragma once

#include "myVec3.h"

class myBoundSphere
{
public:
	myBoundSphere(float radius) : _radius(radius) {}

	bool Intersects(const myVec3& rayOrigin, const myVec3& rayDirection, float& distance);
	bool Intersects_AssumeNomral(const myVec3& rayOrigin, const myVec3& NORMALIZED_rayDirection, float& distance);

	bool Intersects(const myBoundSphere& otherSphere, float& distance);


private:
	myVec3 _center;
	float _radius;
};

