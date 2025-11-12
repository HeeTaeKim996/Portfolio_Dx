#include "pch.h"
#include "myQuaternion.h"
#include "myMatrix3x3.h"
#include "myEulerAngles.h"


const myQuaternion myQuaternion::Identity;


myQuaternion::myQuaternion() : X(0.f), Y(0.f), Z(0.f), W(1.f)
{
}

myQuaternion::myQuaternion(const myVec3& axis, float degree)
{
	FromAxisAngle(axis, degree);
}

myQuaternion::myQuaternion(const myEulerAngles& eulerAngles)
{
	FromEulerAngles(eulerAngles);
}

myQuaternion::myQuaternion(const myVec3& lookingVector, const myVec3& upVector)
{
	FromLookingVector(lookingVector, upVector);
}

myQuaternion::myQuaternion(const myMatrix3x3& InMatrix)
{
	FromMatrix(InMatrix);
}



void myQuaternion::FromAxisAngle(const myVec3& InAxis, float inAngleDegree)
{
	float sin = 0.f, cos = 0.f;
	Math::GetSinCos(sin, cos, inAngleDegree * 0.5f);
	W = cos;
	X = sin * InAxis.X;
	Y = sin * InAxis.Y;
	Z = sin * InAxis.Z;
}

void myQuaternion::FromEulerAngles(const myEulerAngles& InEulerAngles)
{
	float cy = 0.f, sy = 0.f, cp = 0.f, sp = 0.f, cr = 0.f, sr = 0.f;
	Math::GetSinCos(sy, cy, InEulerAngles.Yaw * 0.5f);
	Math::GetSinCos(sp, cp, InEulerAngles.Pitch * 0.5f);
	Math::GetSinCos(sr, cr, InEulerAngles.Roll * 0.5f);

#ifdef YAW_PITCH_ROLL
	W = cy * cp * cr + sy * sp * sr;
	X = cy * cr * sp + cp * sy * sr;
	Y = cp * cr * sy - cy * sp * sr;
	Z = cy * cp * sr - cr * sy * sp;
#elif defined(ROLL_PITCH_YAW)
	W = cy * cp * cr - sy * sp * sr;
	X = cy * cr * sp - cp * sy * sr;
	Y = cp * cr * sy + cy * sp * sr;
	Z = cy * cp * sr + cr * sy * sp;
#else
	assert(0); // �÷��� �ּ� 1�� �ʿ�
#endif
}

//#include "iostream"  // For Debug
void myQuaternion::FromMatrix(const myMatrix3x3& InMatrix)
{
	float root = 0.f;
	float trace = InMatrix[0][0] + InMatrix[1][1] + InMatrix[2][2];


	//{ // DEBUG
	//	std::cout << (float)InMatrix[0].SizeSquared() << std::endl;
	//	std::cout << (float)InMatrix[1].SizeSquared() << std::endl;
	//	std::cout << (float)InMatrix[2].SizeSquared() << std::endl;
	//}

	const float eps = 1e-4f; 

	if (!Math::EqualsInTolerance(InMatrix[0].SizeSquared(), 1.f, eps) ||
		!Math::EqualsInTolerance(InMatrix[1].SizeSquared(), 1.f, eps) ||
		!Math::EqualsInTolerance(InMatrix[2].SizeSquared(), 1.f, eps))
	{
		*this = myQuaternion::Identity;
		return;
	}

	if (trace > 0.f)
	{
		root = sqrtf(trace + 1.f);
		W = 0.5f * root;
		root = 0.5f / root;

		X = (InMatrix[2][1] - InMatrix[1][2]) * root;
		Y = (InMatrix[0][2] - InMatrix[2][0]) * root;
		Z = (InMatrix[1][0] - InMatrix[0][1]) * root;
	}
	else
	{
		BYTE i = 0;

		if (InMatrix[1][1] > InMatrix[0][0]) i = 1;
		if (InMatrix[2][2] > InMatrix[1][1]) i = 2;


		static const BYTE next[3] = { 1, 2, 0 };
		BYTE j = next[i];
		BYTE k = next[j];

		root = sqrtf(InMatrix[i][i] - InMatrix[j][j] - InMatrix[k][k] + 1.f);

		float* qt[3] = { &X, &Y, &Z };
		*qt[i] = 0.5f * root;

		root = 0.5f / root;

		*qt[j] = (InMatrix[j][i] + InMatrix[i][j]) * root;
		*qt[k] = (InMatrix[k][i] + InMatrix[i][k]) * root;

		W = (InMatrix[k][j] - InMatrix[j][k]) * root;
	}
}

void myQuaternion::FromLookingVector(const myVec3& lookingVector, const myVec3& upVector)
{
	myVec3 localX, localY, localZ;

	localZ = lookingVector.GetNormalize();
	
	if (Math::Abs(localZ.Y) >= (1.f - SMALL_NUMBER))
	{
		localX = myVec3::UnitX;
	}
	else
	{
		localX = upVector.Cross(localZ).GetNormalize();
	}
	localY = localZ.Cross(localX).GetNormalize();

	FromMatrix(myMatrix3x3(localX, localY, localZ));
}




myQuaternion myQuaternion::operator-() const
{
	return myQuaternion(-X, -Y, -Z, -W);
}

myQuaternion myQuaternion::operator*(const myQuaternion& InQuaternion) const
{
	myQuaternion ret;

	myVec3 v1(X, Y, Z), v2(InQuaternion.X, InQuaternion.Y, InQuaternion.Z);

	ret.W = W * InQuaternion.W - v1.Dot(v2);
	myVec3 imaginaryPart = v1 * InQuaternion.W + v2 * W + v1.Cross(v2);
	ret.X = imaginaryPart.X;
	ret.Y = imaginaryPart.Y;
	ret.Z = imaginaryPart.Z;

	return ret;
}

myQuaternion myQuaternion::operator*=(const myQuaternion& InQuaternion)
{
	float w0 = W;
	myVec3 v1(X, Y, Z), v2(InQuaternion.X, InQuaternion.Y, InQuaternion.Z);

	W = w0 * InQuaternion.W - v1.Dot(v2);
	myVec3 imaginaryPart = v1 * InQuaternion.W + v2 * w0 + v1.Cross(v2);
	X = imaginaryPart.X;
	Y = imaginaryPart.Y;
	Z = imaginaryPart.Z;

	return *this;
}

myVec3 myQuaternion::operator*(const myVec3& InVector) const
{
	return RotateVector(InVector);
}

bool myQuaternion::operator==(const myQuaternion& InQuaternion) const
{
	return X == InQuaternion.X && Y == InQuaternion.Y && Z == InQuaternion.Z && W == InQuaternion.W;
}

myQuaternion myQuaternion::Slerp(const myQuaternion& startQuaternion, myQuaternion endQuaternion,
	float InRatio)
{
	float dot = startQuaternion.X * endQuaternion.X + startQuaternion.Y * endQuaternion.Y
		+ startQuaternion.Z * endQuaternion.Z + startQuaternion.W * endQuaternion.W;

	if (dot < 0.0f)
	{
		endQuaternion = -endQuaternion;
		dot = -dot;
	}

	float alpha = 1.f, beta = 0.f;
	if (dot > 0.9995f)
	{
		alpha = 1.0f - InRatio;
		beta = InRatio;
	}
	else
	{
		const float theta = acosf(dot);
		const float invSin = 1.f / sinf(theta);
		alpha = sinf((1.f - InRatio) * theta) * invSin;
		beta = sinf(InRatio * theta) * invSin;
	}

	myQuaternion ret;

	ret.X = alpha * startQuaternion.X + beta * endQuaternion.X;
	ret.Y = alpha * startQuaternion.Y + beta * endQuaternion.Y;
	ret.Z = alpha * startQuaternion.Z + beta * endQuaternion.Z;
	ret.W = alpha * startQuaternion.W + beta * endQuaternion.W;

	return ret;
}

float myQuaternion::Dot(const myQuaternion& q) const
{
	return X * q.X + Y * q.Y + Z * q.Z + W * q.W;
}

myVec3 myQuaternion::RotateVector(const myVec3& v) const
{
	myVec3 r(X, Y, Z);
	myVec3 t = v.Cross(r) * 2.f;
	myVec3 result = v + t * W + t.Cross(r);
	return result;
}

void myQuaternion::Normalize()
{
	const float squareSum = X * X + Y * Y + Z * Z + W * W;

	if (squareSum >= SMALL_NUMBER)
	{
		const float scale = 1.f / sqrtf(squareSum);

		X *= scale;
		Y *= scale;
		Z *= scale;
		W *= scale;
	}
	else
	{
		*this = myQuaternion::Identity;
	}
}



myEulerAngles myQuaternion::ToEulerAngles() const
{
#ifdef YAW_PITCH_ROLL
	myEulerAngles retEuler;

	float sinRcosP = 2 * (W * Z + X * Y);
	float cosRcosP = 1 - 2 * (X * X + Z * Z);
	retEuler.Roll = Math::Rad2Deg(atan2(sinRcosP, cosRcosP));

	float halfSinP = (W * X - Y * Z);
	float asinThreshold = 0.4999995f;
	if (halfSinP < -asinThreshold)
	{
		retEuler.Pitch = -90.f;
	}
	else if (halfSinP > asinThreshold)
	{
		retEuler.Pitch = 90.f;
	}
	else
	{
		retEuler.Pitch = Math::Rad2Deg(asinf((2.f * halfSinP)));
	}

	float sinYcosP = 2 * (W * Y + X * Z);
	float cosYcosP = 1 - 2 * (X * X + Y * Y);
	retEuler.Yaw = Math::Rad2Deg(atan2(sinYcosP, cosYcosP));

	return retEuler;
#elif defined(ROLL_PITCH_YAW)
	assert(0); // W X Y Z ������ - - + + �϶��� euler->quat �� ������� �� ���� ������ �ʿ��ѵ�, ���� �ȱ���..
#else
	assert(0); // �÷��� �ּ� 1�� �ʿ�
#endif
}

myMatrix3x3 myQuaternion::ToRotateMatrix() const
{
	return myMatrix3x3(
		RotateVector(myVec3(1, 0, 0)), 
		RotateVector(myVec3(0, 1, 0)), 
		RotateVector(myVec3(0, 0, 1)));
}



bool myQuaternion::IsUnitQuaternion() const
{
	float size = sqrtf(X * X + Y * Y + Z * Z + W * W);
	if (Math::EqualsInTolerance(size, 1.f))
	{
		return true;
	}

	return false;
}