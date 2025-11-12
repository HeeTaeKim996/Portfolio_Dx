#include "pch.h"
#include "myBoundingSphere.h"
#include "myVec3.h"
#include <cmath>
#include "myRay.h"
#include "ContactInfo.h"
#include "myBoundingOrientedBox.h"
#include "myBoundingCapsule.h"


void myBoundingSphere::ComputeAABB(myVec3& min, myVec3& max) const
{
	min.X = center.X - radius;
	min.Y = center.Y - radius;
	min.Z = center.Z - radius;

	max.X = center.X + radius;
	max.Y = center.Y + radius;
	max.Z = center.Z + radius;

}


bool myBoundingSphere::Intersects(const myRay& ray, float& distance) const
{
	myVec3 v = center - ray.origin;

	myVec3 project = myVec3::Project_AssumeNormal(v, ray.dir);

	myVec3 closestPoint = ray.origin + project;
	float distSquared = (closestPoint - center).SizeSquared();

	if (distSquared > radius * radius)
		return false;

	distance = (closestPoint - ray.origin).Size();
	return distance <= ray.magnitude;
}

#if 0 // Obsolate
bool myBoundingSphere::Intersects_AssumeNomral(const myVec3& rayOrigin, const myVec3& normalDir, float& distance)
{
	myVec3 v = center - rayOrigin;

	myVec3 project = myVec3::Project_AssumeNormal(v, normalDir);

	distance = (v - project).Size();

	return distance <= radius;
}
#endif

bool myBoundingSphere::Intersects(const myVec3& minAABB, const myVec3& maxAABB) const
{
	myVec3 closetInAABB(
		std::max(minAABB.X, std::min(center.X, maxAABB.X)),
		std::max(minAABB.Y, std::min(center.Y, maxAABB.Y)),
		std::max(minAABB.Z, std::min(center.Z, maxAABB.Z))

	);

	float diffSquared = (center - closetInAABB).SizeSquared();

	return diffSquared <= radius * radius;

}

bool myBoundingSphere::Intersects(const myBoundingSphere& otherSphere, ContactInfo& contactInfo) const
{
	float centerDist = (center - otherSphere.center).Size();

	float sumRadius = radius + otherSphere.radius;

	if (centerDist > sumRadius)
	{
		return false;
	}
	else
	{
		contactInfo.penetration = sumRadius - centerDist;
		contactInfo.normal = (center - otherSphere.center).GetNormalize();
		return true;
	}
}

bool myBoundingSphere::Intersects(const myBoundingOrientedBox& obb, ContactInfo& contactInfo) const
{
	myVec3 closestPoint_ToSphere_FromObb = obb.ClosestPoint(center);

	myVec3 diff = center - closestPoint_ToSphere_FromObb;
	float diffSquare = diff.SizeSquared();

	if (diffSquare > radius * radius) return false;

	float dist = sqrtf(diffSquare);
	contactInfo.penetration = radius - dist;



	if (dist > 1e-6f)
	{
		contactInfo.normal = diff / dist;
	}
	else
	{
		contactInfo.normal = myVec3::UnitY;
	}

	return true;
}

bool myBoundingSphere::Intersects(const myBoundingCapsule& capsule, ContactInfo& contactInfo) const
{
	myVec3 capsuleClosest = myVec3::ClosestPoint(capsule.bottom, capsule.top, center);

	myVec3 diff = center - capsuleClosest;
	float dist = diff.Size();

	contactInfo.penetration = capsule.radius + radius - dist;

	if (dist > 1e-6f)
	{
		contactInfo.normal = diff / dist;
	}
	else
	{
		contactInfo.normal = myVec3::UnitY;
	}

	return contactInfo.penetration >= 0.f;
}

myVec3 myBoundingSphere::LosestPoint() const
{
	return myVec3(center.X, center.Y - radius, center.Z);
}


