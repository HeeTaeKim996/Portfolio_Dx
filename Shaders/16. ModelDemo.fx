#include "00. Global.fx"
#include "00. Light.fx"


#define MAX_MODEL_TRANSFORMS 250

cbuffer BoneBuffer
{
    matrix BoneTransforms[MAX_MODEL_TRANSFORMS];
};

uint BoneIndex;


//float4 MaterialEmissive;

MeshOutput VS(VertexTextureNormalTanget input)
{
    MeshOutput output;
    
    output.position = mul(input.position, BoneTransforms[BoneIndex]);
    output.position = mul(output.position, W);
    output.worldPosition = output.position.xyz; // 순서 주의. W 적용후의 pos

    output.position = mul(output.position, VP);
    //output.position = mul(output.position, V);
    //output.position = mul(output.position, P);

    output.uv = input.uv;
    output.normal = mul(input.normal, (float3x3) W); 
    // ※ RS 단계에서 자동으로 퍼스펙티브 보정을 하기 때문에, PS에 input 으로 들어올 때 이미 normalized가 깨진다.
    //   따라서 여기서 normalized를 굳이 하지 않는다.
    output.tangent = mul(input.tangent, (float3x3) W);
    
    return output;
}







float4 PS(MeshOutput input) : SV_TARGET
{
    //ComputeNormalMapping(input.normal, input.tangent, input.uv);
    
    //float4 color = ComputeLight(input.normal, input.uv, input.worldPosition);

    float4 color = DiffuseMap.Sample(LinearSampler, input.uv);
    
    return color;
}


float4 PS_RED(MeshOutput input) : SV_TARGET
{
    return float4(1, 1, 1, 1);

}


technique11 T0
{
    PASS_VP(P0, VS, PS)
    PASS_RS_VP(P1, FillModeWireFrame, VS, PS_RED)
}
