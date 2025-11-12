#include "00. Global.fx"

float4 LightAmbient;
float4 MaterialAmbient;

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




Texture2D Texture0;


/*  ○ Ambient (주변광/환경광)
     - 수많은 반사를 거쳐서, 광원이 불분명한 빛
*/

float4 PS(VertexOutput input) : SV_TARGET
{
    float4 color = LightAmbient * MaterialAmbient;
    
    //return color;
    return Texture0.Sample(LinearSampler, input.uv) * color;
}



technique11 T0
{
    PASS_VP(P0, VS, PS)
}
