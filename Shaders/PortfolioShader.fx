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
    }

    return transform;
}


MeshOutput VS_ModelAnimator(VertexTextureNormalTangetBlend input)
{
    MeshOutput output;
    Matrix animationMatrix = GetAnimationMatrix(input);
    
    output.position = mul(input.position, animationMatrix);
    output.position = mul(output.position, W);
    output.worldPosition = output.position.xyz; 

    output.position = mul(output.position, VP);
    //output.position = mul(output.position, V);
    //output.position = mul(output.position, P);

    output.uv = input.uv;
    
    output.normal = mul(input.normal, (float3x3) animationMatrix);
    output.normal = mul(output.normal, (float3x3) W); 

    output.tangent = mul(input.tangent, (float3x3) animationMatrix);
    output.tangent = mul(output.tangent, (float3x3) W);
    
    return output;
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
    output.worldPosition = output.position.xyz; 

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
    
    float4 viewPos = mul(float4(input.position.xyz, 0), V);
    float4 clipPos = mul(viewPos, P);
    
    output.position = clipPos.xyzw;
    output.position.z = output.position.w * 0.999999f;
   
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