#include "pch.h"
#include "myRay.h"
#include "myMatrix4x4.h"
#include "myVec4.h"
#include "myVec3.h"


myRay::myRay(float width, float height, float disNear, float disFar, myMatrix4x4& vInv, const myMatrix4x4& projection, 
	int32 screenX, int32 screenY)
{
	float d_a = projection[0][0];
	float d = projection[1][1];

	// SCreenPos -> NDC
	float ndcX = (+2.0f * screenX / width - 1.f);
	float ndcY = (-2.0f * screenY / height + 1.f);


	// NDC -> View
	float viewX = ndcX / d_a;
	float viewY = ndcY / d;

	myVec3 viewNear = myVec3(viewX * disNear, viewY * disNear, disNear);
	myVec3 worldNear = (viewNear.ToVec4(1.f) * vInv).ToVec3();



	myVec3 camPos = vInv[3].ToVec3();
	origin = camPos;
	dir = (worldNear - camPos).GetNormalize();
	magnitude = disFar;
}

bool myRay::Intersect(const myVec3& minAABB, const myVec3& maxAABB, float& distance) const
{
	float tMin = -FLT_MAX;
	float tMax = FLT_MAX;

	for (int i = 0; i < 3; i++)
	{
		float axisOrigin = origin[i];
		float axisDir = dir[i];
		float axisMinVal = minAABB[i];
		float axisMaxVal = maxAABB[i];

		if (fabsf(axisDir) < 1e-6f)
		{
			if (axisOrigin < axisMinVal || axisOrigin > axisMaxVal)
			{
				return false;
			}
		}
		else
		{
			float t1 = (axisMinVal - axisOrigin) / axisDir;
			float t2 = (axisMaxVal - axisOrigin) / axisDir;


			if (t1 > t2)
			{
				float temp = t1;
				t1 = t2;
				t2 = temp;
			}

			tMin = t1 > tMin ? t1 : tMin;
			tMax = t2 < tMax ? t2 : tMax;

			if (tMin > tMax)
			{
				return false;
			}
		}
	}


	distance = tMin >= 0 ? tMin : tMax;

	return distance >= 0 && distance <= magnitude;
}