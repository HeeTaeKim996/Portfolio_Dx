#include "pch.h"
#include "myVec4.h"
#include "myVec3.h"
#include "myMatrix4x4.h"


const myVec4 myVec4::UnitX = myVec4(1, 0, 0, 0);
const myVec4 myVec4::UnitY = myVec4(0, 1, 0, 0);
const myVec4 myVec4::UnitZ = myVec4(0, 0, 1, 0);
const myVec4 myVec4::UnitW = myVec4(0, 0, 0, 1);



myVec4::myVec4(myVec3 InVec3, float InW)
	: X(InVec3.X), Y(InVec3.Y), Z(InVec3.Z), W(InW)
{
}

myVec4 myVec4::operator+(const myVec4& InVector) const
{
	return myVec4(X + InVector.X, Y + InVector.Y, Z + InVector.Z, W + InVector.W);
}

myVec4 myVec4::operator-(const myVec4& InVector) const
{
	return myVec4(X - InVector.X, Y - InVector.Y, Z - InVector.Z, W - InVector.W);
}

myVec4 myVec4::operator*(float InScale) const
{
	return myVec4(X * InScale, Y * InScale, Z * InScale, W * InScale);
}

myVec4 myVec4::operator/(float InScale) const
{
	assert(InScale != 0.f);

	float invScale = 1.f / InScale;
	return myVec4(X * invScale, Y * invScale, Z * invScale, W * invScale);
}

myVec4& myVec4::operator+=(const myVec4& InVector)
{
	X += InVector.X;
	Y += InVector.Y;
	Z += InVector.Z;
	W += InVector.W;

	return *this;
}

myVec4& myVec4::operator-=(const myVec4& InVector)
{
	X -= InVector.X;
	Y -= InVector.Y;
	Z -= InVector.Z;
	W -= InVector.W;

	return *this;
}

myVec4 myVec4::operator*=(float InScale)
{
	X *= InScale;
	Y *= InScale;
	Z *= InScale;
	W *= InScale;

	return *this;
}

myVec4 myVec4::operator/=(float InScale)
{
	float invScale = 1.f / InScale;

	X *= invScale;
	Y *= invScale;
	Z *= invScale;
	W *= invScale;

	return *this;
}

myVec4 myVec4::operator*(const myMatrix4x4 InMatrix) const
{
	myMatrix4x4 tMat = InMatrix.Transpose();

	return myVec4(Dot(tMat[0]), Dot(tMat[1]), Dot(tMat[2]), Dot(tMat[3]));
}

myVec4& myVec4::operator*=(const myMatrix4x4 InMatrix)
{
	myMatrix4x4 tMat = InMatrix.Transpose();

	myVec4 ret(Dot(tMat[0]), Dot(tMat[1]), Dot(tMat[2]), Dot(tMat[3]));

	*this = ret;
	return *this;
}

bool myVec4::operator==(const myVec4& v) const
{
	return X == v.X && Y == v.Y && Z == v.Z && W == v.W;
}

bool myVec4::operator!=(const myVec4& v) const
{
	return X != v.X || Y != v.Y || Z != v.Z || W != v.W;
}

float myVec4::Size() const
{
	return sqrtf(SizeSquared());
}

float myVec4::SizeSquared() const
{
	return X * X + Y * Y + Z * Z + W * W;
}

void myVec4::Normalize()
{
	*this = GetNormalize();
}

myVec4 myVec4::GetNormalize() const
{
	float squareSum = SizeSquared();
	if (squareSum == 1.f)
	{
		return *this;
	}
	else if (squareSum == 0.f)
	{
		return myVec4(0, 0, 0, 0);
	}

	float invLength = Math::InvSqrt(squareSum);
	return myVec4(X * invLength, Y * invLength, Z * invLength, W * invLength);
}

float myVec4::Dot(const myVec4& InVector) const
{
	return X * InVector.X + Y * InVector.Y + Z * InVector.Z + W * InVector.W;
}

myVec3 myVec4::ToVec3() const
{
	return myVec3(X, Y, Z);
}
