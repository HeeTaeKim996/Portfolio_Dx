#pragma once
#include "ConstantBuffer.h"

class Shader;

struct GlobalDesc
{
	Matrix V = Matrix::Identity;
	Matrix P = Matrix::Identity;
	Matrix VP = Matrix::Identity;
	Matrix VInv = Matrix::Identity;
};

struct TransformDesc
{
	Matrix W = Matrix::Identity;
};



// Light
struct LightDesc
{
	Color ambient = Color(1.f, 1.f, 1.f, 1.f);
	Color diffuse = Color(1.f, 1.f, 1.f, 1.f);
	Color specular = Color(1.f, 1.f, 1.f, 1.f);
	Color emissive = Color(1.f, 1.f, 1.f, 1.f);

	Vec3 direction;
	float padding;
};

struct MaterialDesc
{
	Color ambient = Color(0.f, 0.f, 0.f, 1.f);
	Color diffuse = Color(1.f, 1.f, 1.f, 1.f);
	Color specular = Color(0.f, 0.f, 0.f, 1.f);
	Color emissive = Color(0.f, 0.f, 0.f, 1.f);
};




struct BoneDesc
{
	Matrix transforms[MAX_MODEL_TRANSFORMS];
};



// Animation
struct KeyframeDesc
{
	int32 animIndex = 0;
	uint32 currentFrame = 0;
	uint32 nextFrame = 0;
	float ratio = 0.f;
	float sumTime = 0.f;
	float speed = 1.f;
	Vec2 padding;
};

struct TweenDesc
{
	TweenDesc()
	{
		currentDesc.animIndex = 0;
		reservedDesc.animIndex = -1;
	}


	void ClearReservedAnim()
	{
		reservedDesc.animIndex = -1;
		reservedDesc.currentFrame = 0;
		reservedDesc.nextFrame = 0;
		reservedDesc.sumTime = 0;
		tweenSumTime = 0.f;
		tweenRatio = 0.f;
	}

	bool HasReserve()
	{
		return reservedDesc.animIndex != -1;
	}


	float tweenDuration = 1.0f;
	float tweenRatio = 0.f;
	float tweenSumTime = 0.f;
	float padding = 0.f;
	KeyframeDesc currentDesc;
	KeyframeDesc reservedDesc;
};



class RenderManager
{
	//DECLARE_SINGLE(RenderManager);
private:
	RenderManager() {}
public:
	static RenderManager* GetInstance()
	{
		static RenderManager instance;
		return &instance;
	}

public:
	void Init(shared_ptr<Shader> shader);
	void Update(); // TEMP

	void PushGlobalData(const Matrix& view, const Matrix& projection, const Matrix& VInv);
	void PushTransformData(const TransformDesc& desc);
	void PushLightData(const LightDesc& desc);
	void PushMaterialData(const MaterialDesc& desc);
	void PushBoneData(const BoneDesc& desc);
	void PushKeyframeData(const KeyframeDesc& desc);
	void PushTweenData(const TweenDesc& desc);

private:
	shared_ptr<Shader> _shader;

	GlobalDesc _globalDesc;
	shared_ptr<ConstantBuffer<GlobalDesc>> _globalBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> _globalEffectBuffer;

	TransformDesc _transformDesc;
	shared_ptr<ConstantBuffer<TransformDesc>> _transformBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> _transformEffectBuffer;


	LightDesc _lightDesc;
	shared_ptr<ConstantBuffer<LightDesc>> _lightBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> _lightEffectBuffer;


	MaterialDesc _materialDesc;
	shared_ptr<ConstantBuffer<MaterialDesc>> _materialBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> _materialEffectBuffer;


	BoneDesc _boneDesc;
	shared_ptr<ConstantBuffer<BoneDesc>> _boneBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> _boneEffectBuffer;


	KeyframeDesc _keyframeDesc;
	shared_ptr<ConstantBuffer<KeyframeDesc>> _keyframeBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> _keyframeEffectBuffer;


	TweenDesc _tweenDesc;
	shared_ptr<ConstantBuffer<TweenDesc>> _tweenBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> _tweenEffectBuffer;
};


