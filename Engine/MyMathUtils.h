#pragma once

class MyMathUtils
{
public:
	static myVec3 SimpleVec3ToMyVec3(DirectX::SimpleMath::Vector3 simpleVec3);

	static DirectX::SimpleMath::Vector3 MyVec3ToSimpleVec3(myVec3 myVec3);

	myVec4 SimpleVec4ToMyVec4(DirectX::SimpleMath::Vector4 simpleVec4);

	static DirectX::SimpleMath::Vector4 MyVec4ToSimpleVec4(myVec4 myVec4);

	static myMatrix4x4 SimpleMatrixToMyMatrix(DirectX::SimpleMath::Matrix simpleMatrix);

	static DirectX::SimpleMath::Matrix MyMatrixToSimpleMatrix(myMatrix4x4 myMatrix);
};

