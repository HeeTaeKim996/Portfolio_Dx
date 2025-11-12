#pragma once
#include "RenderManager.h"
#include "Renderer.h"

class Model;

struct AnimTransform
{
	using TransformArrayType = array<Matrix, MAX_MODEL_TRANSFORMS>;

	array<TransformArrayType, MAX_MODEL_KEYFRAMES> transforms;
};


enum class AnimPass
{
	DEFAULT = 0,
	IK = 1,
	WHITE_KEYFRAME = 2
};

struct TempBoneData
{
	TransformBase world;
	TransformBase local;
	int parent;
	vector<int> childrens;
};

struct IK_LookData
{
	bool isAssigned = false;
	Vec3 lookingPos;
#if 0
	float ratios[6] = {
		1.0f,  // Head
		0.85f, // Neck
		0.70f, // Spine
		0.55f, // Spine01
		0.40f, // Spine02
		0.25f  // Hips
	};
#else
	float ratios[6] = {
		0.50f,  // Head
		0.45f, // Neck
		0.40f, // Spine
		0.35f, // Spine01
		0.30f, // Spine02
		0.25f  // Hips
	};
#endif
};

struct UnderRotateData
{
	enum
	{
		FORWARD = 0,
		RIGHT = 1,
		LEFT = 2,
		BACKWARD = 3
	};


	bool isAssigned = false;

	float diffDegree;
	float crossY;
	map<int, int> animIndexMapping; // ※ FORWARD, RIGHT, LEFT, BACKWARD 순으로, 플레이하는 애니매이션을 매핑해야 함

	float ratios[2] =
	{
		0.66f,	// UnderSpine [ Spine02 ]
		0.33f,	// MiddleSpine [ Spine01 ]
	};
};


class ModelAnimator : public Renderer
{
	using Super = Renderer;
public:
	ModelAnimator(shared_ptr<Shader> shader);
	~ModelAnimator();

	virtual void Awake() override;
	virtual void Start() override;

	virtual void Update() override;
	virtual void Render() override;


	void SetModel(shared_ptr<Model> model);
	void SetPass(uint8 pass) { _pass = pass; }




private:
	void CreateTexture();
	void CreateAnimationTransform(uint32 index);

private:
	void AnimationRender();
	void ModelRender();
	void IKRender();
	void Test2_UnderRotateRender();

private:
	// Base
	void UpdateTweenDesc();
	vector<TempBoneData> GetTempBoneData(bool calcLocal = true);





public:
	void Play(int animIndex);
	void CrossFade(int animIndex, float lerpTime = 1.f);
	void Crossfade_SyncElapsedTime(int animIndex, float lerpTime = 1.f);

	void SetIKData(IK_LookData ikData) { _ikLookData = ikData; }
	void SetUnderRotateData(UnderRotateData underRotateData) { _underRotateData = underRotateData; }

	void RemoveIKData() { _ikLookData = IK_LookData(); }
	void Remove_Test2_UnderRotateData() { _underRotateData = UnderRotateData(); }


public:
	virtual void LoadXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent) override;
	virtual void WriteXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent) override;
	virtual shared_ptr<Component> Clone() const override;


private:
	vector<AnimTransform> _animTransforms;
	ComPtr<ID3D11Texture2D> _texture;
	ComPtr<ID3D11ShaderResourceView> _srv;

private:
	//KeyframeDesc _keyframeDesc;
	TweenDesc _tweenDesc;


public:
	bool isPlayingAnimation = false;
	float speed = 1.f;


private:
	uint8 _pass = 0;
	shared_ptr<Model> _model;


	// Only Upper IK
	IK_LookData _ikLookData;
	int _ikHeadIndex = -1;
	Vec3 _headMeshPos;


	// UnerRotate
	UnderRotateData _underRotateData;
	int _underRotate_Hip_Index = -1;
	int _underRotate_LowSpine_Index = -1;
};

