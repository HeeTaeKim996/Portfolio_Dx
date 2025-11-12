#include "pch.h"
#include "myBoundingOrientedBox.h"
#include "myBoundingSphere.h"

#include "myRay.h"
#include "ContactInfo.h"
#include "myBoundingCapsule.h"

bool myBoundingOrientedBox::Intersects(const myRay& ray, float& distance) const
{
	float tMin = -FLT_MAX;
	float tMax = FLT_MAX;

	myVec3 c = center - ray.origin;


	for (int i = 0; i < 3; i++)
	{
		float e = axes[i].Dot(c);
		float f = axes[i].Dot(ray.dir);

		if (fabsf(f) > 1e-6f)
		{
			float t1 = (e + halfExtents[i]) / f;
			float t2 = (e - halfExtents[i]) / f;

			if (t1 > t2)
				std::swap(t1, t2);

			tMin = std::max(tMin, t1);
			tMax = std::min(tMax, t2);

			if (tMin > tMax) return false;
		}
		else
		{
			if (fabsf(e) > halfExtents[i])
			{
				return false;
			}
		}

	}


	distance = (tMin >= 0) ? tMin : tMax;

	return distance >= 0 && distance <= ray.magnitude;
	// ※ 거리가 rayClosest - rayOrigin 처럼, 레이 기준 거리를 산출해야 하는데, distnace 는 축별 길이 중 최대 최소이니 문제가 될테지만,
	//   우선 넘어감

}

#if 0 // Obsolate
bool myBoundingOrientedBox::Intersects_AssumeNormal(const myVec3& rayOrigin, const myVec3& rayDir, float& distance)
{
	float tMin = -FLT_MAX;
	float tMax = FLT_MAX;

	myVec3 c = center - rayOrigin;


	for (int i = 0; i < 3; i++)
	{
		float e = axes[i].Dot(c);
		float f = axes[i].Dot(rayDir);

		if (fabsf(f) > 1e-6f)
		{
			float t1 = (e + halfExtents[i]) / f;
			float t2 = (e - halfExtents[i]) / f;

			if (t1 > t2)
				std::swap(t1, t2);

			tMin = std::max(tMin, t1);
			tMax = std::min(tMax, t2);

			if (tMin > tMax) return false;
		}
		else
		{
			if (fabsf(e) > halfExtents[i])
			{
				return false;
			}
		}

	}


	distance = (tMin >= 0) ? tMin : tMax;

	return distance >= 0;
}
#endif

void myBoundingOrientedBox::ComputeAABB(myVec3& min, myVec3& max) const
{
	myVec3 ex = axes[0] * halfExtents.X;
	myVec3 ey = axes[1] * halfExtents.Y;
	myVec3 ez = axes[2] * halfExtents.Z;

	myVec3 r;
	r.X = fabsf(ex.X) + fabsf(ey.X) + fabsf(ez.X);
	r.Y = fabsf(ex.Y) + fabsf(ey.Y) + fabsf(ez.Y);
	r.Z = fabsf(ex.Z) + fabsf(ey.Z) + fabsf(ez.Z);

	min = center - r;
	max = center + r;
}

#if 0 // peneDepth 구하는 부분 오류
bool myBoundingOrientedBox::Intersects_OBB(const myBoundingOrientedBox& b, float& peneDepth)
{
	const float EPS = 1e-6f;

	myVec3 T = center - b.center;

	float tDotA[3] = { T.Dot(axes[0]), T.Dot(axes[1]), T.Dot(axes[2]) };

	float dots[3][3];
	float absDots[3][3];

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			dots[i][j] = axes[i].Dot(b.axes[j]);
			absDots[i][j] = fabs(dots[i][j]) + EPS /* EPS : 안정성용 */;
		}
	}

	float aExts[3] = { halfExtents.X, halfExtents.Y, halfExtents.Z };
	float bExts[3] = { b.halfExtents.X, b.halfExtents.Y, b.halfExtents.Z };

	float minOverlap = FLT_MAX;

	for (int i = 0; i < 3; i++)
	{
		float ra = aExts[i];
		float rb = bExts[0] * absDots[i][0] + bExts[1] * absDots[i][1] + bExts[2] * absDots[i][2];
		float dist = fabs(tDotA[i]);
		float overlap = ra + rb - dist;
		if (overlap < 0) return false;
		minOverlap = ::fmin(minOverlap, overlap);
	}

	for (int j = 0; j < 3; j++)
	{
		float ra = aExts[0] * absDots[0][j] + aExts[1] * absDots[1][j] + aExts[2] * absDots[2][j];
		float rb = bExts[j];
		float dist = fabs(T.Dot(b.axes[j]));
		float overlap = ra + rb - dist;
		if (overlap < 0) return false;
		minOverlap = ::fmin(minOverlap, overlap);
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			float ra = aExts[(i + 1) % 3] * absDots[(i + 2) % 3][j] + aExts[(i + 2) % 3] * absDots[(i + 1) % 3][j];
			float rb = bExts[(j + 1) % 3] * absDots[i][(j + 2) % 3] + bExts[(j + 2) % 3] * absDots[i][(j + 1) % 3];
			float dist = ::fabs(-tDotA[(i + 1) % 3] * dots[(i + 2) % 3][j] + tDotA[(i + 2) % 3] * dots[(i + 1) % 3][j]);
			float overlap = ra + rb - dist;
			if (overlap < 0) return false;
			minOverlap = ::fmin(minOverlap, overlap);
		}
	}

	peneDepth = minOverlap;

	return true;
}
#elif 1 // penetratio 이 예쌍보다 높게 계산됨.. 원인은 모름
bool myBoundingOrientedBox::Intersects_OBB(const myBoundingOrientedBox& b, ContactInfo& contactInfo) const
{
	myVec3 dir = (b.center - center);
	float dist = dir.Size();

	dir.Normalize();

	float projA = halfExtents.X * fabsf(axes[0].Dot(dir)) +
		halfExtents.Y * fabsf(axes[1].Dot(dir)) +
		halfExtents.Z * fabsf(axes[2].Dot(dir));

	float projB = b.halfExtents.X * fabsf(b.axes[0].Dot(dir)) +
		b.halfExtents.Y * fabsf(b.axes[1].Dot(dir)) +
		b.halfExtents.Z * fabsf(b.axes[2].Dot(dir));

	float overlap = projA + projB - dist;
	if (overlap < 0)
		return false;

	contactInfo.normal = dir;
	contactInfo.penetration = overlap;
	return true;
}
#endif

bool myBoundingOrientedBox::Intersects_Sphere(const myBoundingSphere& sphere, ContactInfo& contactInfo) const
{
#if 0 // Obsolate
	myVec3 T = sphere.center - center;

	float dToAxis[3];
	for (int i = 0; i < 3; i++)
	{
		dToAxis[i] = T.Dot(axes[i]);
	}

	float closestToSphere[3];
	for (int i = 0; i < 3; i++)
	{
		if (dToAxis[i] > halfExtents[i]) closestToSphere[i] = halfExtents[i];
		else if (dToAxis[i] < -halfExtents[i]) closestToSphere[i] = -halfExtents[i];
		else closestToSphere[i] = dToAxis[i];
	}


	myVec3 closestPoint_ToSphere_FromObb
		= center + axes[0] * closestToSphere[0] + axes[1] * closestToSphere[1] + axes[2] * closestToSphere[2];

	myVec3 diff = sphere.center - closestPoint_ToSphere_FromObb;
	float diffSquare = diff.SizeSquared();

	if (diffSquare > sphere.radius * sphere.radius) return false;

	peneDepth = sphere.radius - sqrtf(diffSquare);
	return true;
#endif
	myVec3 closestPoint_ToSphere_FromObb = ClosestPoint(sphere.center);

	myVec3 diff = closestPoint_ToSphere_FromObb - sphere.center;
	float diffSquare = diff.SizeSquared();

	if (diffSquare > sphere.radius * sphere.radius) return false;

	float dist = sqrtf(diffSquare);
	contactInfo.penetration = sphere.radius - dist;



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

bool myBoundingOrientedBox::Intersects(const myBoundingCapsule& capsule, ContactInfo& contactInfo) const
{
	myVec3 capsuleClosest = myVec3::ClosestPoint(capsule.bottom, capsule.top, center);
	myVec3 obbClosest = ClosestPoint(capsuleClosest);

	capsuleClosest = myVec3::ClosestPoint(capsule.bottom, capsule.top, obbClosest); // 이렇게 한번 더 할지는, 엔진마다 상이하다 함 (반반이라 함)

	myVec3 diff = (obbClosest - capsuleClosest);
	float sizeSquared = diff.SizeSquared();

	if (sizeSquared > capsule.radius * capsule.radius)
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

	contactInfo.penetration = capsule.radius - size;



	return true;
}

myVec3 myBoundingOrientedBox::ClosestPoint(const myVec3& point) const
{
	myVec3 T = point - center;

	float dToAxis[3];
	for (int i = 0; i < 3; i++)
	{
		dToAxis[i] = T.Dot(axes[i]);
	}

	float closestToPoints[3];
	for (int i = 0; i < 3; i++)
	{
		if (dToAxis[i] > halfExtents[i]) closestToPoints[i] = halfExtents[i];
		else if (dToAxis[i] < -halfExtents[i]) closestToPoints[i] = -halfExtents[i];
		else closestToPoints[i] = dToAxis[i];
	}

	return center + axes[0] * closestToPoints[0] + axes[1] * closestToPoints[1] + axes[2] * closestToPoints[2];
}

myVec3 myBoundingOrientedBox::LowestPoint() const
{
	const float sign[2] = { -1.f, 1.f };

	myVec3 lowest = myVec3::Zero;
	float minY = FLT_MAX;

	int cumulateCount = 0;

	for (int sx = 0; sx < 2; sx++)
		for (int sy = 0; sy < 2; sy++)
			for (int sz = 0; sz < 2; sz++)
			{
				myVec3 p = center
					+ axes[0] * sign[sx] * halfExtents.X
					+ axes[1] * sign[sy] * halfExtents.Y
					+ axes[2] * sign[sz] * halfExtents.Z;

				float diff = p.Y - minY;


				if (p.Y < minY - 1e-4)
				{
					cumulateCount = 1;
					lowest = p;
					minY = p.Y;
				}
				else if (fabs(p.Y - minY) <= 1e-4)
				{
					lowest = (lowest * cumulateCount + p) / ++cumulateCount;
					minY = lowest.Y;
				}
			}

	return lowest;
}

