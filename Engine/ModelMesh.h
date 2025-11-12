#pragma once

class Material;

struct ModelBone
{
	wstring name;
	int32 index;
	int32 parentIndex;
	shared_ptr<ModelBone> parent; // Cache

	Matrix transform; 
	// ※ 효율 너무 안좋다. 추후 직접 만든 TransformBase 로 수정하면 좋지만,
	//   AssimTool 에서 넘길 때 AsBone 에서 Matrix로 넘기기 때문에.. 수정할 게 너무 많아져서, 일단 Matrix 로 사용



	vector<shared_ptr<ModelBone>> children; // Cache
};

struct ModelMesh
{
	void CreateBuffers();


	wstring name;

	// Mesh
	shared_ptr<Geometry<ModelVertexType>> geometry = make_shared<Geometry<ModelVertexType>>();
	shared_ptr<VertexBuffer> vertexBuffer;
	shared_ptr<IndexBuffer> indexBuffer;


	// Material
	wstring materialName = L"";
	shared_ptr<Material> material; // Cache


	// Bones
	int32 boneIndex;
	shared_ptr<ModelBone> bone; // Cache
};



