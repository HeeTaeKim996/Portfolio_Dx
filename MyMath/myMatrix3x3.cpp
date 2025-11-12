#include "pch.h"
#include "myMatrix3x3.h"
#include "myVec3.h"
#include "myMatrix4x4.h"
#include "myVec4.h"

const myMatrix3x3 myMatrix3x3::Identity;
myMatrix3x3::myMatrix3x3() : Arrays{ myVec3(1.f, 0, 0), myVec3(0, 1.f, 0), myVec3(0, 0, 1.f) }
{
}

myMatrix3x3::myMatrix3x3(const myVec3& array0, const myVec3& array1, const myVec3& array2)
	: Arrays{ array0, array1, array2 }
{
}

const myVec3& myMatrix3x3::operator[](BYTE index) const
{
	assert(index < 3);
	return Arrays[index];
}

myVec3& myMatrix3x3::operator[](BYTE index)
{
	assert(index < 3);
	return Arrays[index];
}

myMatrix3x3 myMatrix3x3::operator*(float scalar) const
{
	return myMatrix3x3(
		Arrays[0] * scalar,
		Arrays[1] * scalar,
		Arrays[2] * scalar
	);
}

myMatrix3x3 myMatrix3x3::operator*(const myMatrix3x3& InMatrix) const
{
	myMatrix3x3 tMat = InMatrix.Transpose();
	return myMatrix3x3(
		myVec3(Arrays[0].Dot(tMat[0]), Arrays[0].Dot(tMat[1]), Arrays[0].Dot(tMat[2])),
		myVec3(Arrays[1].Dot(tMat[0]), Arrays[1].Dot(tMat[1]), Arrays[1].Dot(tMat[2])),
		myVec3(Arrays[2].Dot(tMat[0]), Arrays[2].Dot(tMat[1]), Arrays[2].Dot(tMat[2]))
	);
}


void myMatrix3x3::SetIdentity()
{
	*this = myMatrix3x3::Identity;
}

myMatrix3x3 myMatrix3x3::Transpose() const
{
	return myMatrix3x3(
		myVec3(Arrays[0].X, Arrays[1].X, Arrays[2].X),
		myVec3(Arrays[0].Y, Arrays[1].Y, Arrays[2].Y),
		myVec3(Arrays[0].Z, Arrays[1].Z, Arrays[2].Z)
	);
}

myMatrix4x4 myMatrix3x3::ToMatrix4x4()
{
	return myMatrix4x4{
		Arrays[0].ToVec4(),
		Arrays[1].ToVec4(),
		Arrays[2].ToVec4(),
		myVec4(0, 0, 0, 1)
	};
}
