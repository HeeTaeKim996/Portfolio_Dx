#include "pch.h"
#include "myVec3.h"
#include "myMatrix3x3.h"
#include "myVec4.h"

const myVec3 myVec3::UnitX = myVec3(1, 0, 0);
const myVec3 myVec3::UnitY = myVec3(0, 1, 0);
const myVec3 myVec3::UnitZ = myVec3(0, 0, 1);
const myVec3 myVec3::One = myVec3(1, 1, 1);
const myVec3 myVec3::Zero = myVec3(0, 0, 0);


myVec3 myVec3::operator+(const myVec3& InVector) const
{
	return myVec3(X + InVector.X, Y + InVector.Y, Z + InVector.Z);
}

myVec3 myVec3::operator-(const myVec3& InVector) const
{
	return myVec3(X - InVector.X, Y - InVector.Y, Z - InVector.Z);
}

myVec3 myVec3::operator*(float InScale) const
{
	return myVec3(X * InScale, Y * InScale, Z * InScale);
}

myVec3 myVec3::operator/(float InScale) const
{
	assert(InScale != 0.f);

	return myVec3(X / InScale, Y / InScale, Z / InScale);
}

myVec3& myVec3::operator+=(const myVec3& InVector)
{
	X += InVector.X;
	Y += InVector.Y;
	Z += InVector.Z;

	return *this;
}

myVec3& myVec3::operator-=(const myVec3& InVector)
{
	X -= InVector.X;
	Y -= InVector.Y;
	Z -= InVector.Z;

	return *this;
}

myVec3& myVec3::operator*=(float InScale)
{
	X *= InScale;
	Y *= InScale;
	Z *= InScale;

	return *this;
}

myVec3& myVec3::operator/=(float InScale)
{
	assert(InScale != 0.f);

	float invScale = 1.f / InScale;

	X *= invScale;
	Y *= invScale;
	Z *= invScale;

	return *this;
}

bool myVec3::operator==(const myVec3& v) const
{
	return X == v.X && Y == v.Y && Z == v.Z;
}

bool myVec3::operator!=(const myVec3& v) const
{
	return X != v.X || Y != v.Y || Z != v.Z;
}


myVec3 myVec3::operator*(const myMatrix3x3 InMatrix) const
{
	myMatrix3x3 tMat = InMatrix.Transpose();

	return myVec3(Dot(tMat[0]), Dot(tMat[1]), Dot(tMat[2]));
}


myVec3& myVec3::operator*=(const myMatrix3x3 InMatrix)
{
	myMatrix3x3 tMat = InMatrix.Transpose();
	
	myVec3 ret(Dot(tMat[0]), Dot(tMat[1]), Dot(tMat[2]));

	*this = ret;
	return *this;
}

myVec3 myVec3::operator*(const myVec3& InVector) const
{
	return myVec3(X * InVector.X, Y * InVector.Y, Z * InVector.Z);
}

myVec3 myVec3::operator/(const myVec3& InVector) const
{
	return myVec3(X / InVector.X, Y / InVector.Y, Z / InVector.Z);
}




float myVec3::Size() const
{
	return sqrtf(SizeSquared());
}

float myVec3::SizeSquared() const
{
	return X * X + Y * Y + Z * Z;
}

void myVec3::Normalize()
{
	*this = GetNormalize();
}


myVec3 myVec3::GetNormalize() const
{
	float squareSum = SizeSquared();
	if (squareSum == 1.f)
	{
		return *this;
	}
	else if (squareSum == 0.f)
	{
		return myVec3(0, 0, 0);
	}

	float invLength = Math::InvSqrt(squareSum);
	return myVec3(X * invLength, Y * invLength, Z * invLength);
}

float myVec3::Dot(const myVec3& InVector) const
{
	return X * InVector.X + Y * InVector.Y + Z * InVector.Z;
}

myVec3 myVec3::Cross(const myVec3& InVector) const
{
	return myVec3(
		Y * InVector.Z - Z * InVector.Y,
		Z * InVector.X - X * InVector.Z,
		X * InVector.Y - Y * InVector.X
	);
}

myVec3 myVec3::Lerp(const myVec3& start, const myVec3& end, float t)
{
	t = t < 0.f ? 0.f : t > 1.f ? 1.f : t;
	
	return start * (1- t) + end * t;
}

myVec4 myVec3::ToVec4(float lastValue)
{
	return myVec4(X, Y, Z, lastValue);
}

myVec3 myVec3::Project(const myVec3& vector, const myVec3& onto)
{
	myVec3 normalizedOnto = onto.GetNormalize();

	return normalizedOnto * vector.Dot(normalizedOnto);
}

myVec3 myVec3::Project_AssumeNormal(const myVec3& vector, const myVec3& NORMALIZED_onto)
{
	return NORMALIZED_onto * vector.Dot(NORMALIZED_onto);
}

myVec3 myVec3::ClosestPoint(const myVec3& vecStart, const myVec3& vecEnd, const myVec3& otherPoint)
{
	myVec3 u = (vecEnd - vecStart);
	myVec3 v = otherPoint - vecStart;

	float t = u.Dot(v) / u.SizeSquared();

	t = t > 1.f ? 1.f : t < 0.f ? 0.f : t;
	
	return vecStart + u * t;
}

void myVec3::ClosestPointSegments(const myVec3& P0, const myVec3& P1, const myVec3& Q0, const myVec3& Q1, myVec3& pClosest, 
	myVec3& qClosest)
{
	myVec3 u = P1 - P0;
	myVec3 v = Q1 - Q0;
	myVec3 w = P0 - Q0;

	float uDotu = u.Dot(u);
	float vDotv = v.Dot(v);
	float uDotv = u.Dot(v);

	float uDotw = u.Dot(w);
	float vDotw = v.Dot(w);

	float uuvv_uvuv = uDotu * vDotv - uDotv * uDotv;

	float s;
	float t;
	if (uuvv_uvuv > 1e-8f) 
	{
		s = (uDotv * vDotw - vDotv * uDotw) / uuvv_uvuv;
		t = (uDotu * vDotw - uDotv * uDotw) / uuvv_uvuv;
	}
	else
	{
		s = 0;
		t = vDotw / vDotv;
	}

	s = s >= 1.f ? 1.f : s <= 0.f ? 0.f : s;
	t = t >= 1.f ? 1.f : t <= 0.f ? 0.f : t;

	pClosest = P0 + u * s;
	qClosest = Q0 + v * t;
}
