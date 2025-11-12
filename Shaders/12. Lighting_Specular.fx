#include "00. Global.fx"

float3 LightDir;

float4 LightSpecular;
float4 MaterialSpecular;

Texture2D DiffuseMap;

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







/*  ○ Specular(반사광)
     - 한방향으로 반사되는 빛
     - Phong 방법 사용. (실전에서는 Blinn-Phong 방법을 많이 사용한다 함)
*/

float4 PS(MeshOutput input) : SV_TARGET
{
    // float3 R = reflect(LightDir, input.normal); // -> phong 모델로 반사광 구하는 코드.. (blin phong 아님)
    
    float3 R = LightDir + 2 * dot(input.normal, -LightDir) * input.normal;
    R = normalize(R); // input.normal 이 normalized 되지 않을 수 있으니, normalize 한듯
    
    float3 cameraPosition = -V._41_42_43; // V 의 tx, ty, tz
    float3 E = normalize(cameraPosition - input.worldPosition);
    
    float value = saturate(dot(R, E)); // ※ saturate : clamp(0~1)
    float specular = pow(value, 10);
    
    float4 color = LightSpecular * MaterialSpecular * specular;
    
    return color;
}



technique11 T0
{
    PASS_VP(P0, VS, PS)
}
