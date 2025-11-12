#include "00. Global.fx"
#include "00. Light.fx"


#define MAX_MODEL_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500



struct KeyframeDesc
{
    int animIndex;
    uint currentFrame;
    uint nextFrame;
    float ratio;
    float sumTime;
    float speed;
    float2 padding;
};

struct TweenFrameDesc
{
    float tweenDuration;
    float tweenRatio;
    float tweenSumTime;
    float padding;
    KeyframeDesc currentDesc;
    KeyframeDesc reservedDesc;
};

cbuffer TweenBuffer
{
    TweenFrameDesc TweenFrames;
};


cbuffer BoneBuffer
{
    matrix BoneTransforms[MAX_MODEL_TRANSFORMS];
};


uint BoneIndex;
Texture2DArray TransformMap;















float4 PS_WHITE(MeshOutput input) : SV_TARGET
{
    return float4(1, 1, 1, 1);
}
float4 PS_RED(MeshOutput input) : SV_TARGET
{
    return float4(1, 0, 0, 1);
}
float4 PS_BLUE(MeshOutput input) : SV_TARGET
{
    return float4(0, 0, 1, 1);
}
float4 PS_GREEN(MeshOutput input) : SV_TARGET
{
    return float4(0, 1, 0, 1);
}






















Matrix GetAnimationMatrix(VertexTextureNormalTangetBlend input)
{
    float indices[4] =
    {
        input.blendIndices.x, input.blendIndices.y, input.blendIndices.z,
        input.blendIndices.w
    };
    float weights[4] =
    {
        input.blendWeights.x, input.blendWeights.y, input.blendWeights.z,
        input.blendWeights.w
    };
    
    int animIndex[2];
    int currentFrame[2];
    int nextFrame[2];
    float ratio[2];
    
    animIndex[0] = TweenFrames.currentDesc.animIndex;
    animIndex[1] = TweenFrames.reservedDesc.animIndex;

    currentFrame[0] = TweenFrames.currentDesc.currentFrame;
    currentFrame[1] = TweenFrames.reservedDesc.currentFrame;
    
    nextFrame[0] = TweenFrames.currentDesc.nextFrame;
    nextFrame[1] = TweenFrames.reservedDesc.nextFrame;
    
    ratio[0] = TweenFrames.currentDesc.ratio;
    ratio[1] = TweenFrames.reservedDesc.ratio;

    
    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    
    Matrix transform = 0;
    
    for (int i = 0; i < 4; i++)
    {
        // First
        c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currentFrame[0], animIndex[0], 0));
        c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currentFrame[0], animIndex[0], 0));
        c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currentFrame[0], animIndex[0], 0));
        c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currentFrame[0], animIndex[0], 0));
        Matrix curr = matrix(c0, c1, c2, c3);
        
        n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], animIndex[0], 0));
        n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], animIndex[0], 0));
        n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], animIndex[0], 0));
        n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], animIndex[0], 0));
        Matrix next = matrix(n0, n1, n2, n3);

        
        Matrix result = lerp(curr, next, ratio[0]);


        // Second
        if (animIndex[1] >= 0)
        {
            c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currentFrame[1], animIndex[1], 0));
            c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currentFrame[1], animIndex[1], 0));
            c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currentFrame[1], animIndex[1], 0));
            c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currentFrame[1], animIndex[1], 0));
            Matrix curr = matrix(c0, c1, c2, c3);
        
            n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], animIndex[1], 0));
            n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], animIndex[1], 0));
            n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], animIndex[1], 0));
            n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], animIndex[1], 0));
            Matrix next = matrix(n0, n1, n2, n3);

            Matrix nextResult = lerp(curr, next, ratio[1]);
            
            result = lerp(result, nextResult, TweenFrames.tweenRatio);
        }
        
        
        transform += mul(weights[i], result);
        // 행렬은 분배 법칙이 성립하니, += 로도 가능 [ (a+b+c+d)v = av +bv + cv + dv ]
    }

    return transform;
}


MeshOutput VS_ModelAnimator(VertexTextureNormalTangetBlend input)
{
    MeshOutput output;
    Matrix animationMatrix = GetAnimationMatrix(input);
    
    output.position = mul(input.position, animationMatrix);
    output.position = mul(output.position, W);
    output.worldPosition = output.position.xyz; // 순서 주의. W 적용후의 pos

    output.position = mul(output.position, VP);
    //output.position = mul(output.position, V);
    //output.position = mul(output.position, P);

    output.uv = input.uv;
    
    output.normal = mul(input.normal, (float3x3) animationMatrix);
    output.normal = mul(output.normal, (float3x3) W); 

    output.tangent = mul(input.tangent, (float3x3) animationMatrix);
    output.tangent = mul(output.tangent, (float3x3) W);
    
    return output;
    
    
    // ※ RS 단계에서 자동으로 퍼스펙티브 보정을 하기 때문에, PS에 input 으로 들어올 때 이미 normalized가 깨진다.
    //   따라서 여기서 normalized를 굳이 하지 않는다.
}


Matrix GetAnimationMatrix_IK(VertexTextureNormalTangetBlend input)
{
    float indices[4] =
    {
        input.blendIndices.x, input.blendIndices.y, input.blendIndices.z, input.blendIndices.w
    };
    
    float weights[4] =
    {
      input.blendWeights.x, input.blendWeights.y, input.blendWeights.z, input.blendWeights.w  
    };

    
    Matrix ret = 0;
    for (int i = 0; i < 4; i++)
    {
        ret += mul(weights[i], BoneTransforms[indices[i]]);
    }
    
    return ret;
}


MeshOutput VS_ModelAnimator_IK(VertexTextureNormalTangetBlend input)
{
    MeshOutput output;
    Matrix animMatrix = GetAnimationMatrix_IK(input);
    
    output.position = mul(input.position, animMatrix);
    
    output.position = mul(output.position, W);
    output.worldPosition = output.position.xyz; // 순서 주의. W 적용후의 pos

    output.position = mul(output.position, VP);

    output.uv = input.uv;
    
    output.normal = mul(input.normal, (float3x3) animMatrix);
    output.normal = mul(output.normal, (float3x3) W);

    output.tangent = mul(input.tangent, (float3x3) animMatrix);
    output.tangent = mul(output.tangent, (float3x3) W);
    
    return output;
}


float4 PS_ModelAnimator(MeshOutput input) : SV_TARGET
{
    ComputeNormalMapping(input.normal, input.tangent, input.uv);
    
    float4 color = ComputeLight(input.normal, input.uv, input.worldPosition);

    //float4 color = DiffuseMap.Sample(LinearSampler, input.uv);
    
    return color;
}

technique11 T0
{
    PASS_VP(P0, VS_ModelAnimator, PS_ModelAnimator)
    PASS_VP(P1, VS_ModelAnimator_IK, PS_ModelAnimator)
    
    PASS_RS_VP(P2, FillModeWireFrame, VS_ModelAnimator, PS_WHITE)
}

























MeshOutput VS_ModelRenderer(VertexTextureNormalTanget input)
{
    MeshOutput output;
    
    output.position = mul(input.position, BoneTransforms[BoneIndex]);
    
   
    
    output.position = mul(output.position, W);
    output.worldPosition = output.position.xyz;
    
    output.position = mul(output.position, VP);
    
    output.uv = input.uv;
    
    
    output.normal = mul(input.normal, (float3x3)BoneTransforms[BoneIndex]);
    output.normal = mul(output.normal, (float3x3) W);
    
    output.tangent = mul(input.tangent, (float3x3) BoneTransforms[BoneIndex]);
    output.tangent = mul(output.tangent, (float3x3) W);
    
    return output;
}

/*
float4 PS_ModelRenderer(MeshOutput input) : SV_TARGET
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.uv);
    
    float value = dot(-GlobalLight.direction, normalize(input.normal));
    color = color * value * Material.diffuse;
    
    return color;
}
*/
float4 PS_ModelRenderer(MeshOutput input) : SV_TARGET
{
    ComputeNormalMapping(input.normal, input.tangent, input.uv);
    
    return ComputeLight(input.normal, input.uv, input.worldPosition);
}


technique11 T1
{
    PASS_VP(P0, VS_ModelRenderer, PS_ModelRenderer)
}






























MeshOutput VS_MeshRenderer(VertexTextureNormalTanget input)
{
    MeshOutput output;
    output.position = mul(input.position, W);
    output.worldPosition = output.position.xyz;
    
    output.position = mul(output.position, VP);
    
    output.uv = input.uv;
    output.normal = mul(input.normal, (float3x3) W);
    output.tangent = mul(input.tangent, (float3x3) W);

    return output;
}

float4 PS_MeshRenderer(MeshOutput input) : SV_TARGET
{
    ComputeNormalMapping(input.normal, input.tangent, input.uv);
    
    float4 color = ComputeLight(input.normal, input.uv, input.worldPosition);

    return color;
}





VertexTextureData VS_Sky(VertexTextureNormalTanget input)
{
    VertexTextureData output;
    
    // ※ 카메라의 위치를 기준으로 Sky Sphere 가 고정되기 때문에, WorldMatrix 를 연산할 필요 없음
    //   [ Local -> (World 생략) -> View -> Projection ]
    
    float4 viewPos = mul(float4(input.position.xyz, 0), V);
    float4 clipPos = mul(viewPos, P);
    
    output.position = clipPos.xyzw;
    output.position.z = output.position.w * 0.999999f;
    /* z = w 를 함으로써, 스카이는 항상 절두체의 원평면과 일치하도록 하여, 스카이가 항상 가장 뒷면에서 렌더링되도록 함
    ※ 특정 RasterizerState 에서, 원평면과 일치할 때에 렌더링되지 않는 옵션도 있기 때문에, 어떤 DepthStencilState에서도 Sky 가 그려지기 
       위해, 0.999999f 를 곱함 (야매. 효율 좋게 하기 위해서는 DepthStencilState 를 건드리는 게 좋다 함. )
    ※ DepthStencilState 를 FX에서 SetDepthSencilState 를 하면 오류가 났었음.. 어떻게 수정해야할지 몰라 일단 야매로 0.999999f 곱함
    ※ NDC 의 Z 의 최댓값이 1일 때에 해당  */
    
    output.uv = input.uv;
    
    return output;
}

float4 PS_Sky(VertexTextureData input) : SV_TARGET
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.uv);
    
    return color;
}






technique11 T2
{
    PASS_VP(P0, VS_MeshRenderer, PS_MeshRenderer)
    PASS_VP(P1, VS_MeshRenderer, PS_RED)
    PASS_VP(P2, VS_MeshRenderer, PS_BLUE)
    PASS_VP(P3, VS_MeshRenderer, PS_GREEN)
    PASS_VP(P4, VS_MeshRenderer, PS_WHITE)
    PASS_RS_VP(P5, RS_Sky, VS_Sky, PS_Sky)
    PASS_RS_VP(P6, FillModeWireFrame, VS_MeshRenderer, PS_GREEN)
}


