#pragma once
#include <windows.h>
#include "DirectXMath.h"
#include "SimpleMath.h"
#include "MyMath/MyMath.h"

using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

using Color = DirectX::XMFLOAT4;

using Vec2 = DirectX::SimpleMath::Vector2;
using Vec3 = myVec3;
using Vec4 = myVec4;
using Matrix = myMatrix4x4;
using Quaternion = myQuaternion;
//using Ray = DirectX::SimpleMath::Ray;

using SimpleVec3 = DirectX::SimpleMath::Vector3;
using SimpleVec4 = DirectX::SimpleMath::Vector4;
using SimpleMatrix = DirectX::SimpleMath::Matrix;