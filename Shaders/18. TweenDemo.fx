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


MeshOutput VS(VertexTextureNormalTangetBlend input)
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
    output.normal = mul(input.normal, (float3x3) W);
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
