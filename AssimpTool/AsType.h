#pragma once

//using VertexType = VertexTextureNormalTangentBlendData; // 기존 코드
using VertexType = ModelVertexType; // 수정 ( Read <-> Write 타입 동일 보장 )

struct asBone
{
	string name;
	int32 index = -1;
	int32 parent = -1;
	Matrix transform;
};

struct asMesh
{
	string name;
	aiMesh* mesh;
	vector<VertexType> vertices;
	vector<uint32> indices;

	int32 boneIndex;
	string materialName;
};

struct asMaterial
{
	string name;
	Color ambient;
	Color diffuse;
	Color specular;
	Color emissive;
	string diffuseFile;
	string specularFile;
	string normalFile;
};


// Animation

struct asBlendWeight
{
	myVec4 indices = myVec4(0, 0, 0, 0);
	myVec4 weights = myVec4(0, 0, 0, 0);

	void Set(uint32 index, uint32 boneIndex, float weight)
	{
		indices[index] = boneIndex;
		weights[index] = weight;
	}
};

struct asBoneWeight
{
	using Pair = pair<int32, float>;
	vector<Pair> boneWeights;

	void AddWeights(uint32 boneIndex, float weight)
	{
		if (weight <= 0.f)
			return;
		
		auto findIt = std::find_if(boneWeights.begin(), boneWeights.end(),
			[weight](const Pair& p) { return weight > p.second; });

		boneWeights.insert(findIt, Pair(boneIndex, weight)); // ※ 코드 좋다. 유용하고..
	}


	asBlendWeight GetBlendWeight()
	{
		asBlendWeight blendWeight;

		if (boneWeights.size() > 4)
			assert(0);

		for (uint32 i = 0; i < boneWeights.size(); i++)
		{
			blendWeight.Set(i, boneWeights[i].first, boneWeights[i].second);
		}

		return blendWeight;
	}



	
	void Normalize()
	{
		if (boneWeights.size() > 4)
			boneWeights.resize(4);

		float totalWeight = 0.f;
		for (const Pair& item : boneWeights)
			totalWeight += item.second;

		float invTotalWeight = 1.f / totalWeight;
		for (Pair& item : boneWeights)
			item.second *= invTotalWeight;
	}
};


struct asKeyframeData
{
	float time;
	Vec3 scale;
	Quaternion rotation;
	Vec3 translation;

};
struct asKeyframe
{
	string boneName;
	vector<asKeyframeData> transforms;
};

struct asAnimation
{
	string name;
	uint32 frameCount;
	float frameRate;
	float duration;
	vector<shared_ptr<asKeyframe>> keyframes;

};