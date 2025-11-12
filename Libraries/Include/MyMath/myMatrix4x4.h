#pragma once
#include "myVec4.h"

class myVec3;
class myQuaternion;

class myMatrix4x4
{
public:
	myMatrix4x4();
	myMatrix4x4(const myVec4& array0, const myVec4& array1, const myVec4& array2, const myVec4& array3);
	myMatrix4x4(const float* copiedMatrixesFirstIndexAddress);
	myMatrix4x4(const myVec3& scale, const myQuaternion& rotation, const myVec3& translation);
	
	const myVec4& operator[](BYTE index) const;
	myVec4& operator[](BYTE index);

	myMatrix4x4 operator*(float scalar) const;
	myMatrix4x4 operator*(const myMatrix4x4& InMatrix) const;
	myMatrix4x4 operator+(const myMatrix4x4& InMatrix) const;

	

	void SetIdentity();
	myMatrix4x4 Transpose() const;

	void Decompose(myVec3& scale, myQuaternion& rotation, myVec3& translation);
	
	static myMatrix4x4 Lerp(const myMatrix4x4& a, const myMatrix4x4& b, float t);

public:
	const static myMatrix4x4 Identity;

public:
	myVec4 Arrays[4];
};
