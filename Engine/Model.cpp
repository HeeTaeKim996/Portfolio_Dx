#include "pch.h"
#include "Model.h"
#include "Utils.h"
#include "FileUtils.h"
#include "tinyxml2.h"
#include <filesystem>
#include "Material.h"
#include "ModelMesh.h"
#include "ResourceManager.h"
#include "ModelAnimation.h"


wstring Model::MODEL_PATH = Utils::ToWString(RESOURCE_MODEL_PATH);
wstring Model::TEXTURE_PATH = Utils::ToWString(RESOURCE_TEXTURE_PATH);

Model::Model(wstring modelName) : Super(ResourceType::Model)
{
	_name = modelName;
	ReadModel();
	ReadMaterial();
}

Model::~Model()
{
}













































void Model::ReadMaterial()
{
	wstring fullPath = TEXTURE_PATH + _name + L"/" + _name + L".xml";
	filesystem::path parentPath = filesystem::path(fullPath).parent_path();

	tinyxml2::XMLDocument* document = new tinyxml2::XMLDocument();
	tinyxml2::XMLError error = document->LoadFile(Utils::ToString(fullPath).c_str());
	assert(error == tinyxml2::XML_SUCCESS);

	tinyxml2::XMLElement* root = document->FirstChildElement();
	tinyxml2::XMLElement* materialNode = root->FirstChildElement();

	while (materialNode)
	{
		shared_ptr<Material> material = make_shared<Material>(GAME->GetShader());

		tinyxml2::XMLElement* node = nullptr;

		node = materialNode->FirstChildElement();
		material->SetName(Utils::ToWString(node->GetText()));

		// Diffuse Texture
		node = node->NextSiblingElement();
		if (node->GetText())
		{
			wstring textureStr = Utils::ToWString(node->GetText());
			if (textureStr.length() > 0)
			{
				shared_ptr<Texture> texture = RESOURCES->GetOrAddTexture(textureStr, (parentPath / textureStr).wstring());
				material->SetDiffuseMap(texture);
			}
		}

		// Specular Texture
		node = node->NextSiblingElement();
		if (node->GetText())
		{
			wstring textureStr = Utils::ToWString(node->GetText());
			if (textureStr.length() > 0)
			{
				shared_ptr<Texture> texture = RESOURCES->GetOrAddTexture(textureStr, (parentPath / textureStr).wstring());
				material->SetSpecularMap(texture);
			}
		}

		// Normal Texture
		node = node->NextSiblingElement();
		if (node->GetText())
		{
			wstring textureStr = Utils::ToWString(node->GetText());
			if (textureStr.length() > 0)
			{
				shared_ptr<Texture> texture = RESOURCES->GetOrAddTexture(textureStr, (parentPath / textureStr).wstring());
				material->SetNormalMap(texture);
			}
		}


		// Ambient
		{
			node = node->NextSiblingElement();
			
			Color color;
			color.x = node->FloatAttribute("R");
			color.y = node->FloatAttribute("G");
			color.z = node->FloatAttribute("B");
			color.w = node->FloatAttribute("A");

			material->GetMaterialDesc().ambient = color;
		}

		// Diffuse
		{
			node = node->NextSiblingElement();

			Color color;
			color.x = node->FloatAttribute("R");
			color.y = node->FloatAttribute("G");
			color.z = node->FloatAttribute("B");
			color.w = node->FloatAttribute("A");

			material->GetMaterialDesc().diffuse = color;
		}

		// Specular
		{
			node = node->NextSiblingElement();

			Color color;
			color.x = node->FloatAttribute("R");
			color.y = node->FloatAttribute("G");
			color.z = node->FloatAttribute("B");
			color.w = node->FloatAttribute("A");

			material->GetMaterialDesc().specular = color;
		}

		// Emissive
		{
			node = node->NextSiblingElement();

			Color color;
			color.x = node->FloatAttribute("R");
			color.y = node->FloatAttribute("G");
			color.z = node->FloatAttribute("B");
			color.w = node->FloatAttribute("A");

			material->GetMaterialDesc().emissive = color;
		}

		_materials.push_back(material);

		// NextMaterial
		materialNode = materialNode->NextSiblingElement();
	}

	BindCacheInfo();
}
















































#define MODEL_READ_WRITE_IN_BINARY_DATA
//#define MODEL_READ_WRITE_IN_XML
void Model::ReadModel()
{
#ifdef MODEL_READ_WRITE_IN_BINARY_DATA
	wstring fullPath = MODEL_PATH + _name + L"/" + _name + L".mesh";

	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(fullPath, FileMode::Read);

	// Bones
	{
		const uint32 count = file->Read<uint32>();
		for (uint32 i = 0; i < count; i++)
		{
			shared_ptr<ModelBone> bone = make_shared<ModelBone>();

			bone->name = Utils::ToWString(file->Read<string>());
			bone->index = file->Read<int32>();
			bone->parentIndex = file->Read<int32>();
			bone->transform = file->Read<Matrix>();

			_bones.push_back(bone);
		}
	}


	// Mesh
	{
		const uint32 count = file->Read<uint32>();
		for (uint32 i = 0; i < count; i++)
		{
			shared_ptr<ModelMesh> mesh = make_shared<ModelMesh>();

			mesh->name = Utils::ToWString(file->Read<string>());
			mesh->boneIndex = file->Read<int32>();
			mesh->materialName = Utils::ToWString(file->Read<string>());

			// VertexData
			{
				const uint32 count = file->Read<uint32>();
				vector<ModelVertexType> vertices;
				vertices.resize(count);

				file->Read((void*)vertices.data(), sizeof(ModelVertexType) * count);
				mesh->geometry->AddVertices(vertices);

				
			}

			// IndexData
			{
				const uint32 count = file->Read<uint32>();

				vector<uint32> indices;
				indices.resize(count);

				file->Read((void*)indices.data(), sizeof(uint32) * count);
				mesh->geometry->AddIndices(indices);
			}

			mesh->CreateBuffers();

			_meshes.push_back(mesh);
		}
	}

	BindCacheInfo();
#elif defined(MODEL_READ_WRITE_IN_XML)
	wstring fullPath = MODEL_PATH + _name + L"/" + _name + L"_Mesh.xml";

	XmlDocument doc;
	tinyxml2::XMLError error = doc.LoadFile(Utils::ToString(fullPath).c_str());
	assert(error == tinyxml2::XML_SUCCESS);

	XmlElement* rootElement = doc.FirstChildElement("Model");
	
	XmlElement* boneDatasElement = rootElement->FirstChildElement("BonesData");

	XmlElement* boneElement = boneDatasElement->FirstChildElement("Bone");
	while (boneElement)
	{
		shared_ptr<ModelBone> bone = make_shared<ModelBone>();

		XmlElement* boneNameElement = boneElement->FirstChildElement("BoneName");
		bone->name = Utils::ToWString(boneNameElement->GetText());
		uint32 index;
		boneElement->QueryUnsignedAttribute("Index", &index);
		bone->index = index;
		boneElement->QueryUnsignedAttribute("ParentsIndex", &index);
		bone->parentIndex = index;

		XmlElement* boneTransformElement = boneElement->FirstChildElement("BoneTransform");
		Matrix matrix;
		for (int i = 0; i < 4; i++)
		{
			XmlElement* rowElement = boneTransformElement->FirstChildElement(("Row" + std::to_string(i)).c_str());
			for (int j = 0; j < 4; j++)
			{
				matrix[i][j] = rowElement->FloatAttribute(("Col" + std::to_string(j)).c_str());
			}
		}
		bone->transform = matrix;


		_bones.push_back(bone);

		boneElement = boneElement->NextSiblingElement("Bone");
	}


	XmlElement* meshDatasElement = rootElement->FirstChildElement("MeshDatasElement");

	XmlElement* meshElement = meshDatasElement->FirstChildElement("Mesh");
	while (meshElement)
	{
		shared_ptr<ModelMesh> mesh = make_shared<ModelMesh>();

		XmlElement* meshNameElement = meshElement->FirstChildElement("MeshName");
		mesh->name = Utils::ToWString(meshNameElement->GetText());

		uint32 index;
		meshElement->QueryUnsignedAttribute("BoneIndex", &index);
		mesh->boneIndex = index;

		XmlElement* materialNameElement = meshElement->FirstChildElement("MaterialName");
		mesh->materialName = Utils::ToWString(materialNameElement->GetText());


		XmlElement* vertexDatasElement = meshElement->FirstChildElement("VertexDatas");
		XmlElement* vertexElement = vertexDatasElement->FirstChildElement("Vertex");

		vector<ModelVertexType> vertices;
		while (vertexElement)
		{
			ModelVertexType vertex;

			XmlElement* pos = vertexElement->FirstChildElement("Pos");
			vertex.position.X = pos->FloatAttribute("X");
			vertex.position.Y = pos->FloatAttribute("Y");
			vertex.position.Z = pos->FloatAttribute("Z");

			XmlElement* uv = vertexElement->FirstChildElement("UV");
			vertex.uv.x = uv->FloatAttribute("X");
			vertex.uv.y = uv->FloatAttribute("Y");

			XmlElement* normal = vertexElement->FirstChildElement("Normal");
			vertex.normal.X = normal->FloatAttribute("X");
			vertex.normal.Y = normal->FloatAttribute("Y");
			vertex.normal.Z = normal->FloatAttribute("Z");

			XmlElement* tanget = vertexElement->FirstChildElement("Tangent");
			vertex.tangent.X = tanget->FloatAttribute("X");
			vertex.tangent.Y = tanget->FloatAttribute("Y");
			vertex.tangent.Z = tanget->FloatAttribute("Z");

			XmlElement* blendIndices = vertexElement->FirstChildElement("BlendIndices");
			vertex.blendIndices.X = blendIndices->FloatAttribute("X");
			vertex.blendIndices.Y = blendIndices->FloatAttribute("Y");
			vertex.blendIndices.Z = blendIndices->FloatAttribute("Z");
			vertex.blendIndices.W = blendIndices->FloatAttribute("W");

			XmlElement* blendWeights = vertexElement->FirstChildElement("BlendWeights");
			vertex.blendWeights.X = blendWeights->FloatAttribute("X");
			vertex.blendWeights.Y = blendWeights->FloatAttribute("Y");
			vertex.blendWeights.Z = blendWeights->FloatAttribute("Z");
			vertex.blendWeights.W = blendWeights->FloatAttribute("W");

			vertices.push_back(vertex);


			vertexElement = vertexElement->NextSiblingElement("Vertex");
		}

		mesh->geometry->AddVertices(vertices);

		XmlElement* indexDatasElement = meshElement->FirstChildElement("IndexDatas");

		XmlElement* indicesInfo = indexDatasElement->FirstChildElement("IndicesInfo");

		uint32 remaining;
		indicesInfo->QueryUnsignedAttribute("Count", &remaining);

		int rowCount;
		indicesInfo->QueryAttribute("RowCount", &rowCount);

		XmlElement* indices = indexDatasElement->FirstChildElement("Indices");
		XmlElement* indexRow = indices->FirstChildElement("Row");

		vector<uint32> indicesVec;
		while (indexRow && rowCount > 0)
		{
			for (int i = 0; i < rowCount && remaining > 0; i++, remaining--)
			{
				uint32 index;
				indexRow->QueryUnsignedAttribute(("Index" + std::to_string(i)).c_str(), &index);
				indicesVec.push_back(index);
			}

			indexRow = indexRow->NextSiblingElement("Row");
		}

		mesh->geometry->AddIndices(indicesVec);


		mesh->CreateBuffers();
		_meshes.push_back(mesh);
	}

	BindCacheInfo();
#else
	assert(0);
#endif

}















































#define ANIMATION_READ_WRITE_IN_BINARY_DATA
//#define ANIMATION_READ_WRITE_IN_XML
void Model::ReadAnimation(wstring animationName)
{
#ifdef ANIMATION_READ_WRITE_IN_BINARY_DATA
	wstring fullPath = MODEL_PATH + _name + L"/" + animationName + L".clip";

	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(fullPath, FileMode::Read);

	shared_ptr<ModelAnimation> animation = make_shared<ModelAnimation>();

	animation->name = Utils::ToWString(file->Read<string>());
	animation->duration = file->Read<float>();
	animation->frameRate = file->Read<float>();
	animation->frameCount = file->Read<uint32>();

	uint32 keyframesCount = file->Read<uint32>();
	for (uint32 i = 0; i < keyframesCount; i++)
	{
		shared_ptr<ModelKeyframe> keyframe = make_shared<ModelKeyframe>();
		keyframe->boneName = Utils::ToWString(file->Read<string>());

		uint32 size = file->Read<uint32>();
		if (size > 0)
		{
			keyframe->transforms.resize(size);
			file->Read(keyframe->transforms.data(), sizeof(ModelKeyframeData) * size);
		}

		animation->keyframes[keyframe->boneName] = keyframe;
	}

	_animations.push_back(animation);
#elif defined(ANIMATION_READ_WRITE_IN_XML)
	XmlDocument doc;

	string fileName = ::string(RESOURCE_MODEL_PATH) + "/" + Utils::ToString(animationName)
		+ ".xml";
	tinyxml2::XMLError error = doc.LoadFile(fileName.c_str());
	assert(error == tinyxml2::XML_SUCCESS);

	XmlElement* animDataElement = doc.FirstChildElement("AnimationData");
	XmlElement* summaryElement = animDataElement->FirstChildElement("Summary");
	XmlElement* nameElement = summaryElement->FirstChildElement("Name");

	shared_ptr<ModelAnimation> animation = make_shared<ModelAnimation>();
	animation->name = Utils::ToWString(nameElement->GetText());

	animation->duration = summaryElement->FloatAttribute("duration");
	animation->frameRate = summaryElement->FloatAttribute("frameRate");
	summaryElement->QueryUnsignedAttribute("FrameCount", &animation->frameCount);

	XmlElement* keyframesElement = animDataElement->FirstChildElement("Keyframes");

	XmlElement* keyframeElement = keyframesElement->FirstChildElement("Keyframe");
	while(keyframeElement)
	{
		shared_ptr<ModelKeyframe> keyframe = make_shared<ModelKeyframe>();

		XmlElement* boneNameElement = keyframeElement->FirstChildElement("BoneName");
		keyframe->boneName = Utils::ToWString(boneNameElement->GetText());

		XmlElement* keyframeDatasElement = keyframeElement->FirstChildElement("KeyframeDatas");

		XmlElement* keyframeDataElement = keyframeDatasElement->FirstChildElement("KeyframeData");
		while (keyframeDataElement)
		{
			ModelKeyframeData keyframeData;
			keyframeData.time = keyframeDataElement->FloatAttribute("Time");
			keyframeData.scale.X = keyframeDataElement->FloatAttribute("ScaleX");
			keyframeData.scale.Y = keyframeDataElement->FloatAttribute("ScaleY");
			keyframeData.scale.Z = keyframeDataElement->FloatAttribute("ScaleZ");

			keyframeData.rotation.X = keyframeDataElement->FloatAttribute("QuaternionX");
			keyframeData.rotation.Y = keyframeDataElement->FloatAttribute("QuaternionY");
			keyframeData.rotation.Z = keyframeDataElement->FloatAttribute("QuaternionZ");
			keyframeData.rotation.W = keyframeDataElement->FloatAttribute("QuaternionW");

			keyframeData.translation.X = keyframeDataElement->FloatAttribute("TranslationX");
			keyframeData.translation.Y = keyframeDataElement->FloatAttribute("TranslationY");
			keyframeData.translation.Z = keyframeDataElement->FloatAttribute("TranslationZ");

			keyframe->transforms.push_back(keyframeData);



			keyframeDataElement = keyframeDataElement->NextSiblingElement("KeyframeData");
		}



		animation->keyframes[keyframe->boneName] = keyframe;

		keyframeElement = keyframeElement->NextSiblingElement("Keyframe");
	}

	_animations.push_back(animation);

#else
	assert(0);
#endif

}

shared_ptr<Material> Model::GetMaterialByName(const wstring& name)
{
	for (const shared_ptr<Material>& material : _materials)
	{
		if (material->GetName() == name)
		{
			return material;
		}
	}

	return nullptr;
}

shared_ptr<ModelMesh> Model::GetMeshByName(const wstring& name)
{
	for (const shared_ptr<ModelMesh>& mesh : _meshes)
	{
		if (mesh->name == name)
		{
			return mesh;
		}
	}

	return nullptr;
}


shared_ptr<ModelBone> Model::GetBoneByName(const wstring& name)
{
	for (const shared_ptr<ModelBone>& bone : _bones)
	{
		if (bone->name == name)
		{
			return bone;
		}
	}

	return nullptr;
}

shared_ptr<ModelAnimation> Model::GetAnimationByName(wstring name)
{
	for (shared_ptr<ModelAnimation>& animation : _animations)
	{
		if (animation->name == name)
		{
			return animation;
		}
	}

	return nullptr;
}

void Model::BindCacheInfo()
{
	for (const shared_ptr<ModelMesh>& mesh : _meshes)
	{
		if (mesh->material == nullptr)
		{
			mesh->material = GetMaterialByName(mesh->materialName);
		}
		if (mesh->bone == nullptr)
		{
			mesh->bone = GetBoneByIndex(mesh->boneIndex);
		}
	}

	if (_root == nullptr && _bones.size() > 0)
	{
		_root = _bones[0];

		for (const shared_ptr<ModelBone>& bone : _bones)
		{
			if (bone->parentIndex >= 0)
			{
				bone->parent = _bones[bone->parentIndex];
				bone->parent->children.push_back(bone);
			}
			else
			{
				bone->parent = nullptr;
			}
		}
	}
}
