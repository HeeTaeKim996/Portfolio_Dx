#pragma once
#include "MyMath/pch.h"
#include <iostream>

#include "MyMath/MyMath.h"

#pragma comment(lib, "MyMath.lib")


#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>

#include "Engine/SimpleMath.h"
#include "Engine/SimpleMath.inl"




using SimpleMatrix = DirectX::SimpleMath::Matrix;
using SimpleQuaternion = DirectX::SimpleMath::Quaternion;