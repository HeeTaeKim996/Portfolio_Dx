#pragma once
#if 1

#define WIN32_LEAN_AND_MEAN

#include "Types.h"
#include "Define.h"

// STL
#include <memory>
#include <iostream>
#include <array>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <algorithm>
using namespace std;

// WIN
#include <windows.h>
#include <assert.h>
#include <optional>

// DX
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>
using namespace DirectX;
using namespace Microsoft::WRL;


#include <FX11/d3dx11effect.h>


// Assimp : 리소스 로드를 위한 외부 라이브러리
// ※ 외부 Assimp 라이브러리 다운 받는 간략한 과정(CMake 포함)은, 55.Assimp 강의 참조
#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>
#pragma comment(lib, "Assimp/assimp-vc143-mtd.lib")



// ImGUI // 외브 라이브러리. 강의 60.ImGUI 참조
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"









// Extern Libs
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex/DirectXTex_debug.lib")
#pragma comment(lib, "FX11/Effects11d.lib")
#else
#pragma comment(lib, "DirectXTex/DirectXTex.lib")
#pragma comment(lib, "FX11/Effects11.lib")  // Libraries 의 FX11 에 있는 코드 (강의의 2D -> 3D로 넘어오며 추가)
#endif

// Managers
#include "Game.h"
#include "Graphics.h"
//#include "InputManager.h"
//#include "TimeManager.h"
//#include "ResourceManager.h"
//#include "RenderManager.h"


// Engine
#include "VertexData.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "Shader.h"

#include "GameObject.h"
#include "Transform.h"
#include "Texture.h"
#include "Mesh.h"



// MyMath
#include "MyMath/MyMath.h"
//#pragma comment(lib, "MyMath.lib")
#ifdef _DEBUG
#pragma comment(lib, "MyMath/Debug/MyMath.lib")
#else
#pragma comment(lib, "MyMath/Release/MyMath.lib")
#endif

#include "MyMathUtils.h"
#endif