#ifndef _GLOBAL_LIGHT_FX
#define _GLOBAL_LIGHT_FX
#include "00. Global.fx"


/*-----------------
       Struct
-----------------*/
struct LightDesc
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 emissive;
    float3 direction;
    float padding;
};

struct MaterialDesc
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 emissive;
};




/*------------------------
       ConstantBuffer
-------------------------*/
cbuffer LightBuffer
{
    LightDesc GlobalLight;
};

cbuffer MaterialBuffer
{
    MaterialDesc Material;
};



/*--------------
       SRV
---------------*/

Texture2D DiffuseMap;
Texture2D SpecularMap;
Texture2D NormalMap;


/*-----------------
      Function
-----------------*/

float4 ComputeLight(float3 normal, float2 uv, float3 worldPosition)
{
    float4 ambientColor = 0;
    float4 diffuseColor = 0;
    float4 specularColor = 0;
    float4 emissiveColor = 0;
    
    // Ambient
    {
        float4 color = GlobalLight.ambient * Material.ambient;
        ambientColor = DiffuseMap.Sample(LinearSampler, uv) * color;
    }
    
    // Diffuse
    {
        float4 sampleColor = DiffuseMap.Sample(LinearSampler, uv);
        float value = saturate(dot(-GlobalLight.direction, normalize(normal)));
        diffuseColor = sampleColor * value * GlobalLight.diffuse * Material.diffuse;
    }
    
    // Specular
    {
        if (dot(normal, -GlobalLight.direction) > 0)
        {
            
            float3 R = GlobalLight.direction + 2 * dot(-GlobalLight.direction, normal) * normal;
            R = normalize(R);

            float3 E = normalize(-V._41_42_43 - worldPosition);
        
            float value = saturate(dot(R, E));
            float specular = pow(value, 10);
        
            specularColor = GlobalLight.specular * Material.specular * specular;
        }
    }
    
    // Emissive
    {
        float3 E = normalize(-V._41_42_43 - worldPosition);
        float3 viewNormal = mul(normal, (float3x3)V);
        
        float value = saturate(dot(E, viewNormal));
        value = 1.0f - value;
        
        value = smoothstep(0.0f, 1.0f, value);
        value = pow(value, 5);
        
        emissiveColor = GlobalLight.emissive * Material.emissive * value;
    }
    
    
    return ambientColor + diffuseColor + specularColor + emissiveColor;
}


void ComputeNormalMapping(inout float3 normal, float3 tangent, float2 uv) // ※ intout==reftValue(cpp)==ref(C#)
{
    float4 map = NormalMap.Sample(LinearSampler, uv);
    if (any(map.rgb) == false)
    {
        return;
    }
    
    float3 N = normalize(normal); // ※ PS 로 넘어올 때 normalize 돼서 오지만, 혹시 모르니 normalize 한다 함(강의)
    float3 T = normalize(tangent);
    float3 B = normalize(cross(N, T)); // ※ 외적 순서 주의 N 이 z, T 가 x이니, Y 인 B는 저 순서로 도출
    float3x3 TBN = float3x3(T, B, N);

    float3 tangentSpaceNormal = 2.f * map.rgb - 1.f; 
    // 픽셀 저장 rgb 가 unsigned에 최적화됐기에, 그에 맞춰 데이터가 저장됨. -1 _ 1 사이로 변환 필요

    float3 worldNormal = mul(tangentSpaceNormal, TBN); 
    
    normal = worldNormal;
}


#endif