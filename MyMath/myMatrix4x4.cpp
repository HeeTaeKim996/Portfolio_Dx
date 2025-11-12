#include "pch.h"
#include "myMatrix4x4.h"
#include "myMatrix3x3.h"
#include "myQuaternion.h"
#include "myEulerAngles.h"

const myMatrix4x4 myMatrix4x4::Identity;

myMatrix4x4::myMatrix4x4()
	: Arrays{ myVec4(1, 0, 0, 0), myVec4(0, 1, 0, 0), myVec4(0, 0, 1, 0), myVec4(0, 0, 0, 1)}
{
}

myMatrix4x4::myMatrix4x4(const myVec4& array0, const myVec4& array1, const myVec4& array2, const myVec4& array3)
	: Arrays { array0, array1, array2, array3}
{
}

myMatrix4x4::myMatrix4x4(const float* firstAddress)
{
	Arrays[0] = myVec4(*firstAddress, *(firstAddress + 1), *(firstAddress + 2), *(firstAddress + 3));
	Arrays[1] = myVec4(*(firstAddress + 4), *(firstAddress + 5), *(firstAddress + 6), *(firstAddress + 7));
	Arrays[2] = myVec4(*(firstAddress + 8), *(firstAddress + 9), *(firstAddress + 10), *(firstAddress + 11));
	Arrays[3] = myVec4(*(firstAddress + 12), *(firstAddress + 13), *(firstAddress + 14), *(firstAddress + 15));
}

myMatrix4x4::myMatrix4x4(const myVec3& scale, const myQuaternion& rotation, const myVec3& translation)
{
	myMatrix3x3 rotMat = rotation.ToRotateMatrix();
	
	rotMat[0] *= scale.X;
	rotMat[1] *= scale.Y;
	rotMat[2] *= scale.Z;


	Arrays[0] = myVec4(rotMat[0], 0);
	Arrays[1] = myVec4(rotMat[1], 0);
	Arrays[2] = myVec4(rotMat[2], 0);
	Arrays[3] = myVec4(translation, 1);
}

const myVec4& myMatrix4x4::operator[](BYTE index) const
{
	assert(index < 4);
	return Arrays[index];
}

myVec4& myMatrix4x4::operator[](BYTE index)
{
	assert(index < 4);
	return Arrays[index];
}

myMatrix4x4 myMatrix4x4::operator*(float scalar) const
{
	return myMatrix4x4(
		Arrays[0] * scalar,
		Arrays[1] * scalar,
		Arrays[2] * scalar,
		Arrays[3] * scalar
	);
}

myMatrix4x4 myMatrix4x4::operator*(const myMatrix4x4& InMatrix) const
{
	myMatrix4x4 tMat = InMatrix.Transpose();
	return myMatrix4x4(
		myVec4(Arrays[0].Dot(tMat[0]), Arrays[0].Dot(tMat[1]), Arrays[0].Dot(tMat[2]), Arrays[0].Dot(tMat[3])),
		myVec4(Arrays[1].Dot(tMat[0]), Arrays[1].Dot(tMat[1]), Arrays[1].Dot(tMat[2]), Arrays[1].Dot(tMat[3])),
		myVec4(Arrays[2].Dot(tMat[0]), Arrays[2].Dot(tMat[1]), Arrays[2].Dot(tMat[2]), Arrays[2].Dot(tMat[3])),
		myVec4(Arrays[3].Dot(tMat[0]), Arrays[3].Dot(tMat[1]), Arrays[3].Dot(tMat[2]), Arrays[3].Dot(tMat[3]))
	);
}

myMatrix4x4 myMatrix4x4::operator+(const myMatrix4x4& InMatrix) const
{
	return myMatrix4x4(
			myVec4(Arrays[0][0] + InMatrix[0][0], Arrays[0][1] + InMatrix[0][1], Arrays[0][2] + InMatrix[0][2], Arrays[0][3] + InMatrix[0][3]),
			myVec4(Arrays[1][0] + InMatrix[1][0], Arrays[1][1] + InMatrix[1][1], Arrays[1][2] + InMatrix[1][2], Arrays[1][3] + InMatrix[1][3]),
			myVec4(Arrays[2][0] + InMatrix[2][0], Arrays[2][1] + InMatrix[2][1], Arrays[2][2] + InMatrix[2][2], Arrays[2][3] + InMatrix[2][3]),
			myVec4(Arrays[3][0] + InMatrix[3][0], Arrays[3][1] + InMatrix[3][1], Arrays[3][2] + InMatrix[3][2], Arrays[3][3] + InMatrix[3][3])
		);
}

void myMatrix4x4::SetIdentity()
{
	*this = myMatrix4x4::Identity;
}

myMatrix4x4 myMatrix4x4::Transpose() const
{
	return myMatrix4x4(
		myVec4(Arrays[0].X, Arrays[1].X, Arrays[2].X, Arrays[3].X),
		myVec4(Arrays[0].Y, Arrays[1].Y, Arrays[2].Y, Arrays[3].Y),
		myVec4(Arrays[0].Z, Arrays[1].Z, Arrays[2].Z, Arrays[3].Z),
		myVec4(Arrays[0].W, Arrays[1].W, Arrays[2].W, Arrays[3].W)
	);
}

void myMatrix4x4::Decompose(myVec3& scale, myQuaternion& rotation, myVec3& translation)
{
	// OUT Pos
	translation = Arrays[3].ToVec3();

	myMatrix3x3 scaleRotMatrix(Arrays[0].ToVec3(), Arrays[1].ToVec3(), Arrays[2].ToVec3());

	// Out Scale
	const float squareSumX = scaleRotMatrix[0].SizeSquared();
	const float squareSumY = scaleRotMatrix[1].SizeSquared();
	const float squareSumZ = scaleRotMatrix[2].SizeSquared();
	scale.X = squareSumX > SMALL_NUMBER ? sqrtf(squareSumX) : SMALL_NUMBER;
	scale.Y = squareSumY > SMALL_NUMBER ? sqrtf(squareSumY) : SMALL_NUMBER;
	scale.Z = squareSumZ > SMALL_NUMBER ? sqrtf(squareSumZ) : SMALL_NUMBER;


	// Out Rotation
	myMatrix3x3 rotMatrix;
	rotMatrix[0] = scaleRotMatrix[0] / scale.X;
	rotMatrix[1] = scaleRotMatrix[1] / scale.Y;
	rotMatrix[2] = scaleRotMatrix[2] / scale.Z;
	rotation.FromMatrix(rotMatrix);


}

myMatrix4x4 myMatrix4x4::Lerp(const myMatrix4x4& a, const myMatrix4x4& b, float t)
{
	return a * (1 - t) + b * t;
}
