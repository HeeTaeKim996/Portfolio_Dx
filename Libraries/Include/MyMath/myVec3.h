#pragma once

class myMatrix3x3;
class myVec4;

class myVec3
{
public:
	myVec3() : X(0.f), Y(0.f), Z(0.f) {}
	myVec3(float InX, float InY, float InZ) : X(InX), Y(InY), Z(InZ) {}

	float& operator[](BYTE InIndex) { assert(InIndex < 3); return (&X)[InIndex]; }
	const float& operator[](BYTE InIndex) const { assert(InIndex < 3); return (&X)[InIndex]; }

	myVec3 operator+(const myVec3& InVector) const;
	myVec3 operator-(const myVec3& InVector) const;
	myVec3 operator*(float InScale) const;
	myVec3 operator/(float InScale) const;
	myVec3& operator+=(const myVec3& InVector);
	myVec3& operator-=(const myVec3& InVector);
	myVec3& operator*=(float InScale);
	myVec3& operator/=(float InScale);
	bool operator==(const myVec3& InVector) const;
	bool operator!=(const myVec3& InVector) const;

	myVec3 operator*(const myMatrix3x3 InMatrix) const;
	myVec3& operator*=(const myMatrix3x3 InMatrix);

	myVec3 operator-() const { return myVec3(-X, -Y, -Z); }
	myVec3 operator*(const myVec3& InVector) const;
	myVec3 operator/(const myVec3& InVector) const;
	
	float Size() const;
	float SizeSquared() const;
	void Normalize();
	myVec3 GetNormalize() const;
	float Dot(const myVec3& InVector) const;
	myVec3 Cross(const myVec3& InVector) const;

	static myVec3 Lerp(const myVec3& start, const myVec3& end, float t);
	myVec4 ToVec4(float lastValue = 0);

public:
	static myVec3 Project(const myVec3& vector, const myVec3& onto);
	static myVec3 Project_AssumeNormal(const myVec3& vector, const myVec3& NORMALIZED_onto);
	static myVec3 ClosestPoint(const myVec3& vecStart, const myVec3& vecEnd, const myVec3& otherPoint);
	static void ClosestPointSegments(const myVec3& P0, const myVec3& P1, const myVec3& Q0, const myVec3& Q1, myVec3& pClosest, myVec3& qClosest);

public:
	// static
	const static myVec3 UnitX;
	const static myVec3 UnitY;
	const static myVec3 UnitZ;
	const static myVec3 One;
	const static myVec3 Zero;

	
public:
	float X, Y, Z;

	
};