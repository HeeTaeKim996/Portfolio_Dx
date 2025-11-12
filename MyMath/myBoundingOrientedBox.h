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
	/* 
		- 첫번째 코드는 15개의 축을 기준으로 비교해서, Intersected 여부는 정확히 감지하지만, Penetration 을 계산하는 부분에 오류가 있다.
		  ( 애초에 min 으로 계산하는 부분에 오류가 있다 )
		- 두번째 코드는 단순하면서, Intersected 도 나름 큰 문제 없이 감지하는 듯 싶지만, peneDepth 가 더 높게 계산된다 
		  ( 원인을 이해 못했음. 축 L에 투영된, 원점에서의 최대 길이 2개를 구하고, 해당길이 2개 - distance 면 적당히 밀어내야 정상일 듯 싶은데, 돌려보면 더 멀리 밀려남 )

		==>> 첫번째는 peneDepth를 구하는데 큰 오류가 있고, 두번째는 peneDepth 가 더 크게 계산 되는 문제가 있지만, 두번째는 불완전하게 작동은 하기에, 두번째을 방법 사용함
	
	*/
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

