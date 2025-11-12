#pragma once

class myVec3;
class myMatrix4x4;

class myVec4
{
public:
	myVec4() : X(0.f), Y(0.f), Z(0.f), W(0.f) {}
	myVec4(float InX, float InY, float InZ, float InW) : X(InX), Y(InY), Z(InZ), W(InW) {}
	myVec4(myVec3 InVec3, float InW);

	float& operator[](BYTE index) { assert(index < 4); return (&X)[index]; }
	const float& operator[](BYTE index) const { assert(index < 4); return (&X)[index]; }

	myVec4 operator+(const myVec4& InVector) const;
	myVec4 operator-(const myVec4& InVector) const;
	myVec4 operator*(float InScale) const;
	myVec4 operator/(float InScale) const;

	myVec4& operator+=(const myVec4& InVector);
	myVec4& operator-=(const myVec4& InVector);
	myVec4 operator*=(float InScale);
	myVec4 operator/=(float InScale);

	myVec4 operator*(const myMatrix4x4 InMatrix) const;
	myVec4& operator*=(const myMatrix4x4 InMatrix);

	bool operator==(const myVec4& InVector) const;
	bool operator!=(const myVec4& InVector) const;

	float Size() const;
	float SizeSquared() const;
	void Normalize();
	myVec4 GetNormalize() const;
	float Dot(const myVec4& InVector) const;
	myVec3 ToVec3() const;
	

public:
	// static
	const static myVec4 UnitX;
	const static myVec4 UnitY;
	const static myVec4 UnitZ;
	const static myVec4 UnitW;


public:
	float X, Y, Z, W;
};

