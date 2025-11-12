matrix World;
matrix View;
matrix Projection;
Texture2D Texture0;

struct VertexInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.position = mul(input.position, World);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);

    output.uv = input.uv;
    
    return output;
}


// Filter = 확대/축소 일어났을 때 중간값을 처리하는 방식
// Address = UV 가 1보다 컸을 때, 나머지 부분을 어떻게 처리할지


SamplerState Sampler0
{
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

float4 PS(VertexOutput input) : SV_TARGET
{    
    return Texture0.Sample(Sampler0, input.uv);
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
        SetRasterizerState(FillModeWireFrame);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}
