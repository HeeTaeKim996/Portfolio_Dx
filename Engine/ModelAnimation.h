#pragma once



struct ModelKeyframeData
{
	float time;
	Vec3 scale;
	Quaternion rotation;
	Vec3 translation;
};

struct ModelKeyframe
{
	wstring boneName;
	vector<ModelKeyframeData> transforms;
};

struct ModelAnimation
{
	inline shared_ptr<ModelKeyframe> GetKeyframe(const wstring& name)
	{
		auto it = keyframes.find(name);
		if (it == keyframes.end())
		{
			return nullptr;
		}
		return it->second;
	}

	wstring name;
	float duration = 0.f;
	float frameRate = 0.f;
	uint32 frameCount = 0;
	unordered_map<wstring, shared_ptr<ModelKeyframe>> keyframes;
	bool isLoop = true;

	// ※ wstring 으로 검색하니 효율이 너무 안좋은데, unordered_map 이 아닌 vector 로 하고, index 로 검색하도록 할 수
	//   없으려나, 어차피 Model 에서도 Bone 은 Index 로 관리되고, AssimTool 프로젝트에서 애니매이션 저장할 때에도
	//   본의 저장 순서는 node->mChildren(?) 순서로 애니매이션이 저장되니, ModelAnimation 의 keyframes 에 저장되는
	//   애니매이션의 순서나, 본의 인덱스 순서나 같을 텐데
	//   ※ 우선 강의대로 했지만, 추후 정리한다면, 다른 방식으로 접근해보자
};

