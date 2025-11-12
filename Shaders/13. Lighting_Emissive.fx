#include "00. Global.fx"

float4 MaterialEmissive;

MeshOutput VS(VertexTextureNormal input)
{
    MeshOutput output;
    output.position = mul(input.position, W);
    output.worldPosition = output.position; // 순서 주의. W 적용후의 pos
    output.position = mul(output.position, VP);

    output.uv = input.uv;
    output.normal = mul(input.normal, (float3x3) W); 
    // ※ RS 단계에서 자동으로 퍼스펙티브 보정을 하기 때문에, PS에 input 으로 들어올 때 이미 normalized가 깨진다.
    //   따라서 여기서 normalized를 굳이 하지 않는다.
    
    return output;
}







/*  ○ Emissive (외곽선)
     - 빛과는 무관하지만, 림라이트 라 불림
*/

float4 PS(MeshOutput input) : SV_TARGET
{
    float3 cameraPosition = -V._41_42_43;
    float3 E = normalize(cameraPosition - input.worldPosition);
    
    float3 temp = mul(input.normal, (float3x3)V);
    
    float value = saturate(dot(E, temp));
    float emissive = 1.0f - value;
    
    emissive = smoothstep(0.0f, 1.0f, emissive);
    // ※ SmoothStep : 수리 공식은 알 필요 없고, bezier와 유사한 형태의, 부드럽게 연결하는 곡선
    emissive = pow(emissive, 2);
    
    float4 color = MaterialEmissive * emissive;
    
    return color;
}



technique11 T0
{
    PASS_VP(P0, VS, PS)
}
