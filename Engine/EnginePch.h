#pragma once
/*----------------------------------------------------------------------------------------------------
	- 속성 Include, Lib 이 매우 복잡.
	- 인프런 DX11강의의 섹션6. DirectX11 3D 의 35. 프로젝트 설정의 0분_15분정도까지가 속성 설정등 정리함
----------------------------------------------------------------------------------------------------*/

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
/*	- ImGUI 에 대한 코드. 게임엔진, 그래픽 렌더링 툴에서 자주 사용하는 경량화 UI 라이브러리.
	- 강의 60.ImGUI 에서 코드 이식을 했지만, 원리는 아얘 모른다. 강의에서 하는 대로 복붙만 함
	- 추후 ImGUI 에 대해 추가 공부 또는 수정에 대비하여 #define.h 에 IMGUI 를 사용한 부분을 Ctrl + , 로 
	  구분을 쉽게 하기 위해, USE_I_M_G_U_I_I_N_D_X_1_1 로 define 함
	- 위 Ctrl + , USE_I_M_G_U_I_I_N_D_X_1_1 와, 98.Utils - extern - ImGUI 내에 외부(강의 60.ImGUI 참조) 에서 받아온
	  소스코드들을 따로 분류했음

*/








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


















































#if 0 // First

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
#include <wrl.h>
#include <DirectXMath.h>
#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>
using namespace DirectX;
using namespace Microsoft::WRL;

#include <FX11/d3dx11effect.h>

// Libs
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex/DirectXTex_debug.lib")
#pragma comment(lib, "FX11/Effects11d.lib")
#else
#pragma comment(lib, "DirectXTex/DirectXTex.lib")
#pragma comment(lib, "FX11/Effects11.lib")
#endif

// Managers
#include "Game.h"
#include "Graphics.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "ResourceManager.h"

#define CHECK(p)	assert(SUCCEEDED(p))
#define GAME		GET_SINGLE(Game)
#define GRAPHICS	GET_SINGLE(Graphics)
#define DEVICE		GRAPHICS->GetDevice()
#define DC			GRAPHICS->GetDeviceContext()
#define INPUT		GET_SINGLE(InputManager)
#define TIME		GET_SINGLE(TimeManager)
#define DT			TIME->GetDeltaTime()
#define RESOURCES	GET_SINGLE(ResourceManager)


// Engine
#include "VertexData.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "Shader.h"
#include "IExecute.h"

#endif // First