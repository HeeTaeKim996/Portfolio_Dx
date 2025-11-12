#pragma once
#include "myVec3.h"

class myMatrix4x4;

class myMatrix3x3
{
public:
	// 생성자
	myMatrix3x3();
	myMatrix3x3(const myVec3& array0, const myVec3& array1, const myVec3& array2);


	// 연산자
	const myVec3& operator[](BYTE index) const;
	myVec3& operator[](BYTE index);

	myMatrix3x3 operator*(float scalar) const;
	myMatrix3x3 operator*(const myMatrix3x3& InMatrix) const;

	void SetIdentity();
	myMatrix3x3 Transpose() const;

	myMatrix4x4 ToMatrix4x4();

public:
	const static myMatrix3x3 Identity;

public:
	// 멤버 변수
	myVec3 Arrays[3];
};

