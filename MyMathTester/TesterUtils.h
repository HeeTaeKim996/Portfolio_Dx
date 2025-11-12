#pragma once
#include "TesterPch.h"
#include "MyMath/myVec3.h"
#include "assert.h"
#include "MyMath/myMatrix3x3.h"
#include "MyMath/myVec4.h"
#include "MyMath/myMatrix4x4.h"
#include "MyMath/myEulerAngles.h"
#include "MyMath/myQuaternion.h"
#include "MyMath/TransformBase.h"
#include "TesterUtils.h"

using namespace std;


static void PrintMatrix3x3(myMatrix3x3 matrix)
{
	cout << matrix[0][0] << "  /   " << matrix[0][1] << "  /   " << matrix[0][2] << endl;
	cout << matrix[1][0] << "  /   " << matrix[1][1] << "  /   " << matrix[1][2] << endl;
	cout << matrix[2][0] << "  /   " << matrix[2][1] << "  /   " << matrix[2][2] << endl;
	printf("\n");
}

static void PrintMatrix4x4(myMatrix4x4 matrix)
{
	for (int i = 0; i < 4; i++)
	{
		printf("%10.6f, %10.6f, %10.6f, %10.6f \n", matrix[i][0], matrix[i][1], matrix[i][2], matrix[i][3]);
	}
	printf("\n");
}

static void PrintQuaternion(myQuaternion quat)
{
	cout << quat.W << "  /   " << quat.X << "  /   " << quat.Y << "  /   " << quat.Z << endl;
	printf("\n");
}

static void PrintEulerAngles(myEulerAngles euler)
{
	cout << "Yaw : " << euler.Yaw << "/  Pitch : " << euler.Pitch << "/  Roll : " << euler.Roll << endl;
	printf("\n");
}


static void PrintVec3(myVec3 vec3)
{
	cout << vec3.X << "  /   " << vec3.Y << "  /   " << vec3.Z << endl;
}

static void PrintTransformBase(TransformBase transformBase)
{
	cout << "S" << endl;
	PrintVec3(transformBase.GetScale());

	cout << "R" << endl;
	PrintQuaternion(transformBase.GetRotation());

	cout << "T" << endl;
	PrintVec3(transformBase.GetPosition());
}



static void PrintSimpleMatrix(DirectX::SimpleMath::Matrix mat)
{
	printf("%10.6f, %10.6f, %10.6f, %10.6f \n", mat._11, mat._12, mat._13, mat._14);
	printf("%10.6f, %10.6f, %10.6f, %10.6f \n", mat._21, mat._22, mat._23, mat._24);
	printf("%10.6f, %10.6f, %10.6f, %10.6f \n", mat._31, mat._32, mat._33, mat._34);
	printf("%10.6f, %10.6f, %10.6f, %10.6f \n", mat._41, mat._42, mat._43, mat._44);
	printf("\n");
}