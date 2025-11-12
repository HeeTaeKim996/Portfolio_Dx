#include "00. Global.fx"

float3 LightDir;
float4 LightDiffuse;
float4 MaterialDiffuse;
Texture2D DiffuseMap;

VertexOutput VS(VertexTextureNormal input)
{
    VertexOutput output;
    output.position = mul(input.position, W);
    output.position = mul(output.position, VP);

    output.uv = input.uv;
    output.normal = mul(input.normal, (float3x3) W); 
    // ※ RS 단계에서 자동으로 퍼스펙티브 보정을 하기 때문에, PS에 input 으로 들어올 때 이미 normalized가 깨진다.
    //   따라서 여기서 normalized를 굳이 하지 않는다.
    
    return output;
}







/*  ○ Diffuse(분산광)
     - 빛의 각도에 따라 밝기가 다른 (람버르트 공식 (내적))

*/

float4 PS(VertexOutput input) : SV_TARGET
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.uv);
    
    float value = dot(-LightDir, normalize(input.normal));
    color = color * value * LightDiffuse * MaterialDiffuse;
    
    return color;
}



technique11 T0
{
    PASS_VP(P0, VS, PS)
}
