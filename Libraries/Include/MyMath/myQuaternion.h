#pragma once

#include "myVec3.h"

class myMatrix3x3;
class myEulerAngles;

class myQuaternion
{
public:
	// 积己磊
	myQuaternion();
	myQuaternion(float InX, float InY, float InZ, float InW) : X(InX), Y(InY), Z(InZ), W(InW) {}
	myQuaternion(const myVec3& axis, float degree);
	myQuaternion(const myEulerAngles& eulerAngles);
	myQuaternion(const myVec3& lookingVector, const myVec3& upVector = myVec3::UnitY);
	myQuaternion(const myMatrix3x3& InMatrix);


	// 积己
	void FromAxisAngle(const myVec3& InAxis, float inAngleDegree);
	void FromEulerAngles(const myEulerAngles& InEulerAngles);
	void FromMatrix(const myMatrix3x3& InMatrix);
	void FromLookingVector(const myVec3& lookingVector, const myVec3& upVector = myVec3::UnitY);



	// 窃荐
	myQuaternion operator-() const;
	myQuaternion operator*(const myQuaternion& InQuaternion) const;
	myQuaternion operator*=(const myQuaternion& InQuaternion);
	myVec3 operator*(const myVec3& InVector) const;
	bool operator==(const myQuaternion& InQuaternion) const;

	static myQuaternion Slerp(const myQuaternion& startQuaternion, myQuaternion endQuaternion,
		float InRatio);

	float Dot(const myQuaternion& InQuaternion) const;

	myVec3 RotateVector(const myVec3& InVector) const;
	myQuaternion Inverse() const { return myQuaternion(-X, -Y, -Z, W); }
	void Normalize();
	myEulerAngles ToEulerAngles() const;
	myMatrix3x3 ToRotateMatrix() const;

	bool IsUnitQuaternion() const;
	
	float RealPart() const { return W; }
	myVec3 ImaginaryPart() const { return myVec3(X, Y, Z); }


public:
	const static myQuaternion Identity;

public:
	float X;
	float Y;
	float Z;
	float W;
};

