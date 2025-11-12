#ifndef _GLOBAL_FX_
#define _GLOBAL_FX_

/*---------------------
       ConstBuffer
---------------------*/
cbuffer GlobalBuffer
{
    matrix V;
    matrix P;
    matrix VP;
    matrix VInv;
};

cbuffer TransformBuffer
{
    matrix W;
};





/*-------------------------
         VertexData
-------------------------*/
struct Vertex
{
    float4 position : POSITION;
};

struct VertexTextureData
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VertexColor
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct VertexTextureNormal
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VertexTextureNormalTanget
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VertexTextureNormalTangetBlend
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 blendIndices : BLEND_INDICES;
    float4 blendWeights : BLEND_WEIGHTS;
};


/*-------------------------
        VertexOutput
-------------------------*/
struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCORRD;
    float3 normal : NORMAL;
};

struct MeshOutput
{
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION1;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

/*------------------------
        SamplerState
------------------------*/
SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    /*  ○ MIN_MAG_MIP_LINEAR
         - Minification     : 축소시 4개 텍셀을 선형 보간
         - Magnification    : 마찬가지로 확대시 4개 택셀을 선형 보간
         - MipMap 단계       : 텍스쳐를 미리 여러 해상도에서의 텍스쳐를 미리 저장하고, 렌더링시 해당 해상도와 가장 가까운
                               미리 준비된 텍스쳐를 사용하는 방법
         - MIP_LINEAR        : 만약 렌더링이 (550, 550) 이고, 미리 준비된 게 (500, 500), (600, 600) 이라면,
                               두 해상도의 텍스쳐를 선형 보간하여 처리.

        - MIN_MAG_MIP_LINEAR    : 위에 설명한 MIN, MAG, MIPMAP 을 할 때, 계단 현상이 발생할 수 있는데, 이를 방지하고자,
                                  선형 보간하여 계단 현상을 방지
    */
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState PointSampler
{
    Filter = MIN_MAG_MIP_POINT;
    /*  ○ MIN_MAG_MIP_POINT
         - 앞선 MIN, MAG, MIP 을 선형보간 하지 않고, 가장 가까운 단일 텍셀값을 사용. 
         - 계단 현상이 발생할 수 있지만, 연산 성능이 좋음
    */
    AddressU = Wrap;
    AddressV = Wrap;
};
/*
    - AddressU, V 는 대부분 같은 쌍으로 설정 (굳이 구분해서 외울 필요 없음)
    - Wrap      : 샘플링하는 좌표가 지정한 UV 좌표를 벗어나는 경우, x' = x % maxX , y' = y % maxY, 로 x', y'를 
                  변환하여 UV 매핑. ( 아직 이해를 덜한건지, maxX == maxY == 1 )
    - Mirr      :  uv 좌표를 벗어나는 경우, 앞선 것과 대칭이 되게? 처리. (수식은 정확히 모르겠지만, 돌려보면 암)
    - Clamp     : 디펄트. UV 좌표를 벗어나는 경우, 이상하게(?) 그림
    - Border    : UV 좌표를 벗어나는 경우, 지정한 색상인 BorderColor 대로 그림
*/


/*--------------------------
       RasterizerState
--------------------------*/
RasterizerState FillModeWireFrame
{
    FillMode = WireFrame;
};

RasterizerState RS_Sky
{
    FrontCounterClockwise = true;
    /*  - v1 = p1 - p0, v2 = p2 - p0 일 때, v1 X v2 외적벡터와 카메라 방향의 내적이 양이냐에 따른 렌더링 결과 그거.. 
        - 디펄트가 반시계이고, FronCounterClockwiseTrue 로 할시, 시계방향으로의 외적 (v1, v2외적 순서 바꾸기..) 으로 렌더링 유무 결정 
          (Sky 렌더링 때 사용)
    */
};


/*-----------------
       Macro
-----------------*/
#define PASS_VP(name, vs, ps)                       \
pass name                                           \
{                                                   \
    SetVertexShader(CompileShader(vs_5_0, vs()));   \
    SetPixelShader(CompileShader(ps_5_0, ps()));    \
}


#define PASS_RS_VP(name, rs, vs, ps)                \
pass name                                           \
{                                                   \
    SetRasterizerState(rs);                         \
    SetVertexShader(CompileShader(vs_5_0, vs()));   \
    SetPixelShader(CompileShader(ps_5_0, ps()));    \
}


/*------------------
      Function
------------------*/
float3 CameraPosition()
{
    return VInv._41_42_43;
}



#endif
/*
    ○ 헤더가드 와 #praga once
     - 위 #ifndef #define #endif 를 헤더가드 문법이라 한다 (#pragma once 가 나오기 전까지 통용되는 문법)
     - #include 는 해당 클래스 파일의 텍스트를 단순하게 '복붙' 하는 역할을 한다.
     - B, C 클래스에서 각각 A클래스를 #include 하고, Main 에서 B, C 클래스를 #include 한다면,
       A 클래스의 텍스트가 main 에 두번 복붙되기 때문에, 오류가 발생한다.
     - #praga once 가 나오기 전, 위처럼 헤더가드를 하여, 해당 문제를 방지했다. #pragma once 는 헤더가드를 하지 않아도,
       해당 문제를 방지해준다
     - fx 에서 헤더 가드를 하는 이유는, fx 에서 #pragma once 를 사용 못하기 때문
*/
