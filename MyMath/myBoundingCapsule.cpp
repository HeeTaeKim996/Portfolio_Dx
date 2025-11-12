#include "pch.h"
#include "myBoundingCapsule.h"

#include "myVec3.h"
#include "myRay.h"
#include "myBoundingSphere.h"
#include "myBoundingOrientedBox.h"
#include "ContactInfo.h"

bool myBoundingCapsule::Intersects(const myRay& ray, float& distance) const
{
	myVec3 p0 = ray.origin + ray.dir * radius; 
	// 반지름 만큼 앞쪽을 start로 하여, ray 시작점 뒤쪽에, radius 내에 위치한 콜라이더는 감지 안되도록
	myVec3 p1 = ray.origin + ray.dir * ray.magnitude;


	myVec3 rayClosest;
	myVec3 capsuleClosest;

	myVec3::ClosestPointSegments(p0, p1, bottom, top, rayClosest, capsuleClosest);

	float distSquare = (rayClosest - capsuleClosest).SizeSquared();

	if (distSquare > radius * radius)
		return false;

	distance = (ray.origin - rayClosest).Size();
	return distance <= ray.magnitude;
}

void myBoundingCapsule::ComputeAABB(myVec3& min, myVec3& max) const
{
	min.X = bottom.X <= top.X ? bottom.X : top.X;
	min.Y = bottom.Y <= top.Y ? bottom.Y : top.Y;
	min.Z = bottom.Z <= top.Z ? bottom.Z : top.Z;

	max.X = bottom.X > top.X ? bottom.X : top.X;
	max.Y = bottom.Y > top.Y ? bottom.Y : top.Y;
	max.Z = bottom.Z > top.Z ? bottom.Z : top.Z;

	min.X -= radius;
	min.Y -= radius;
	min.Z -= radius;
	
	max.X += radius;
	max.Y += radius;
	max.Z += radius;
}

bool myBoundingCapsule::Intersects(const myBoundingSphere& otherSphere, ContactInfo& contactInfo) const
{
	myVec3 closest = myVec3::ClosestPoint(bottom, top, otherSphere.center);

	myVec3 diff = closest - otherSphere.center;
	float dist = diff.Size();

	contactInfo.penetration = radius + otherSphere.radius - dist;

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

bool myBoundingCapsule::Intersects(const myBoundingOrientedBox& otherObb, ContactInfo& contactInfo) const
{
	myVec3 aClosest = myVec3::ClosestPoint(bottom, top, otherObb.center);
	myVec3 bClosest = otherObb.ClosestPoint(aClosest);
	
	aClosest = myVec3::ClosestPoint(bottom, top, bClosest); // 이렇게 한번 더 할지는, 엔진마다 상이하다 함 (반반이라 함)

	myVec3 diff = (aClosest - bClosest);
	float sizeSquared = diff.SizeSquared();

	if (sizeSquared > radius * radius)
		return false;

	float size = sqrtf(sizeSquared);
	if (size > 1e-6f)
	{
		contactInfo.normal = diff / size;
	}
	else
	{
		contactInfo.normal = myVec3::UnitY;
	}

	contactInfo.penetration = radius - size;



	return true;
}

bool myBoundingCapsule::Intersects(const myBoundingCapsule& otherCapsule, ContactInfo& contactInfo) const
{
	myVec3 aClosest;
	myVec3 bClosest;

	myVec3::ClosestPointSegments(bottom, top, otherCapsule.bottom, otherCapsule.top, aClosest, bClosest);

	myVec3 diff = (aClosest - bClosest);
	float dist = diff.Size();

	contactInfo.penetration = radius + otherCapsule.radius - dist;
	
	if (dist > 1e-6f)
	{
		contactInfo.normal = diff / dist;
	}
	else
	{
		contactInfo.normal = myVec3::UnitY;
	}


	return contactInfo.penetration > 0.f;
}

bool myBoundingCapsule::Intersects(const myVec3& minAABB, const myVec3& maxAABB) const
{
	myVec3 center = (minAABB + maxAABB) * 0.5f;
	myVec3 closestPoint = myVec3::ClosestPoint(bottom, top, center);

	myVec3 AABBClosest = myVec3(
		closestPoint.X < minAABB.X ? minAABB.X : closestPoint.X > maxAABB.X ? maxAABB.X : closestPoint.X,
		closestPoint.Y < minAABB.Y ? minAABB.Y : closestPoint.Y > maxAABB.Y ? maxAABB.Y : closestPoint.Y,
		closestPoint.Z < minAABB.Z ? minAABB.Z : closestPoint.Z > maxAABB.Z ? maxAABB.Z : closestPoint.Z
	);

	float distSquared = (closestPoint - AABBClosest).SizeSquared();

	return distSquared <= radius * radius;
}

myVec3 myBoundingCapsule::LowestPoint() const
{
	myVec3 lowestLinePoint;
	float diffY = top.Y - bottom.Y;

	if (fabs(diffY) < 1e-4)
	{
		lowestLinePoint = (top + bottom) * 0.5f;
	}
	else if (diffY > 0)
	{
		lowestLinePoint = bottom;
	}
	else
	{
		lowestLinePoint = top;
	}

	return lowestLinePoint - myVec3::UnitY * radius;
}
