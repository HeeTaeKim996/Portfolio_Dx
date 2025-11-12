matrix World;
matrix View;
matrix Projection;
Texture2D Texture0;
float3 LightDir;

struct VertexInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.position = mul(input.position, World);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);

    output.uv = input.uv;
    output.normal = mul(input.normal, (float3x3) World); 
    // ※ RS 단계에서 자동으로 퍼스펙티브 보정을 하기 때문에, PS에 input 으로 들어올 때 이미 normalized가 깨진다.
    //   따라서 여기서 normalized를 굳이 하지 않는다.
    
    return output;
}


// Filter = 확대/축소 일어났을 때 중간값을 처리하는 방식
// Address = UV 가 1보다 컸을 때, 나머지 부분을 어떻게 처리할지


SamplerState Sampler0;
/*
    - AddressU, V 는 대부분 같은 쌍으로 설정 (굳이 구분해서 외울 필요 없음)

    - Wrap      : 샘플링하는 좌표가 지정한 UV 좌표를 벗어나는 경우, x' = x % maxX , y' = y % maxY, 로 x', y'를 
                  변환하여 UV 매핑. ( 아직 이해를 덜한건지, maxX == maxY == 1 )
    - Mirr      :  uv 좌표를 벗어나는 경우, 앞선 것과 대칭이 되게? 처리. (수식은 정확히 모르겠지만, 돌려보면 암)
    - Clamp     : 디펄트. UV 좌표를 벗어나는 경우, 이상하게(?) 그림
    - Border    : UV 좌표를 벗어나는 경우, 지정한 색상인 BorderColor 대로 그림
*/

float4 PS(VertexOutput input) : SV_TARGET
{   
    /*  
        ※ VS 의 return 의 정점과, PS의 input 의 정점과 다르다
          Rasterizer 단계에서, barycentric (퍼스펙티브 보정) 보간이 끝난 뒤의 값이다. 
          [ 이득우저 게임수학의 삼각형에서 아핀결합의 내적을 통한 계수 도출 + 투영 보정 보간 ( y = 1 / x 그거..) 
            이 barycentric(퍼스펙티브) 보정의 기본 개념이라 함.. ]
    */
    float3 normal = normalize(input.normal); // RS 에서 보정 때 normalized가 깨지기 때문에, 여기서 normalized 처리
    float3 light = -LightDir;
    
    return float4(1, 1, 1, 1) * dot(light, normal);
}

float4 PS2(VertexOutput input) : SV_TARGET
{
    float3 normal = normalize(input.normal);
    float3 light = -LightDir;

    return Texture0.Sample(Sampler0, input.uv) * dot(light, normal);
}

float4 PS3(VertexOutput input) : SV_TARGET
{
    float3 normal = normalize(input.normal);
    float3 light = -LightDir;

    return Texture0.Sample(Sampler0, input.uv) * dot(light, normal) * 0.5f; // 0.5f 를 추가해, 전체 밝기를 줄임
}

RasterizerState FillModeWireFrame
{
    FillMode = WireFrame;
};

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
    
    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS2()));
    }

    pass P2
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS3()));
    }
}
