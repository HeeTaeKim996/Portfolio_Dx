#include "pch.h"
#include "Converter.h"
#include <filesystem>
#include "Engine/Utils.h"
#include "Engine/tinyxml2.h"
#include "Engine/FileUtils.h"
#include "Engine/MyMathUtils.h"
#include "Engine/tinyxml2.h"

wstring Converter::FBX_PATH = Utils::ToWString(RESOURCE_FBX_PATH);
wstring Converter::MODEL_PATH = Utils::ToWString(RESOURCE_MODEL_PATH);
wstring Converter::TEXTURE_PATH = Utils::ToWString(RESOURCE_TEXTURE_PATH);

Converter::Converter()
{
	_importer = make_shared<Assimp::Importer>();

}

Converter::~Converter()
{

}




void Converter::ReadAssetFile(wstring file)
{
	wstring fileStr = FBX_PATH + file;

	auto p = std::filesystem::path(fileStr);
	assert(std::filesystem::exists(p));

	_scene = _importer->ReadFile(
		Utils::ToString(fileStr),
		aiProcess_ConvertToLeftHanded |
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	assert(_scene != nullptr);
}










/*------------------------
		ModelData
------------------------*/
void Converter::ExportModelData(wstring savePath)
{
	ReadModelData(_scene->mRootNode, -1, -1);
	ReadSkinData();

	{
#if 0
		FILE* file;
		::fopen_s(&file, "../Vertices.csv", "w");

		::fprintf(file, "Index, Name\n");
		for (const shared_ptr<asBone>& bone : _bones)
		{
			string name = bone->name;
			::fprintf(file, "%d,%s\n", bone->index, bone->name.c_str());
		}

		::fprintf(file, "\n");

		for (const shared_ptr<asMesh>& mesh : _meshes)
		{
			fprintf(file, "\n%s\n", mesh->name.c_str());
			fprintf(file,
				"posX, posY, posZ, index1, index2, index3, index4, weight1, weight2, weight3, weight4\n");
			for (UINT i = 0; i < mesh->vertices.size(); i++)
			{
				Vec3 p = mesh->vertices[i].position;
				Vec4 indices = mesh->vertices[i].blendIndices;
				Vec4 weights = mesh->vertices[i].blendWeights;

				::fprintf(file, "%f, %f, %f,", p.X, p.Y, p.Z);
				::fprintf(file, "%f, %f, %f, %f,", indices.X, indices.Y, indices.Z, indices.W);
				::fprintf(file, "%f, %f, %f, %f\n", weights.X, weights.Y, weights.Z, weights.W);
			}
		}

		::fclose(file);
#endif
	}

	wstring finalPath = MODEL_PATH + savePath;
	WriteModelFile(finalPath);
}
void Converter::ReadModelData(aiNode* node, int32 index, int32 parent)
{
	shared_ptr<asBone> bone = make_shared<asBone>();

	bone->index = index;
	bone->parent = parent;
	bone->name = node->mName.C_Str();

	DirectX::SimpleMath::Matrix dxMat(node->mTransformation[0]); 
	Matrix localTransform = MyMathUtils::SimpleMatrixToMyMatrix(dxMat);
	bone->transform = localTransform.Transpose();
	

	Matrix parentWorld = Matrix::Identity;
	if (parent >= 0)
	{
		parentWorld = _bones[parent]->transform;
	}
	bone->transform = bone->transform * parentWorld; 
	_bones.push_back(bone);




	// Mesh
	ReadMeshData(node, index);

	for (uint32 i = 0; i < node->mNumChildren; i++)
	{
		ReadModelData(node->mChildren[i], _bones.size(), index);
	}
}
void Converter::ReadMeshData(aiNode* node, int32 bone)
{
	if (node->mNumMeshes < 1) return;
	
	shared_ptr<asMesh> mesh = make_shared<asMesh>();
	mesh->name = node->mName.C_Str();
	mesh->boneIndex = bone;

	for (uint32 i = 0; i < node->mNumMeshes; i++)
	{
		uint32 index = node->mMeshes[i];
		const aiMesh* srcMesh = _scene->mMeshes[index];

		// MaterialName
		const aiMaterial* material = _scene->mMaterials[srcMesh->mMaterialIndex];
		mesh->materialName = material->GetName().C_Str();

		const uint32 startVertex = mesh->vertices.size();

		// Vertex
		for (uint32 v = 0; v < srcMesh->mNumVertices; v++)
		{
			// Vertex
			VertexType vertex;
			::memcpy(&vertex.position, &srcMesh->mVertices[v], sizeof(Vec3));


			// UV
			if (srcMesh->HasTextureCoords(0))
			{
				::memcpy(&vertex.uv, &srcMesh->mTextureCoords[0][v], sizeof(Vec2));
			}

			// Normal
			if (srcMesh->HasNormals())
			{
				::memcpy(&vertex.normal, &srcMesh->mNormals[v], sizeof(Vec3));
			}



			// Tangent
			if (srcMesh->HasTangentsAndBitangents())
			{
				::memcpy(&vertex.tangent, &srcMesh->mTangents[v], sizeof(Vec3));
			}

			mesh->vertices.push_back(vertex);
		}

		// Index
		for (uint32 f = 0; f < srcMesh->mNumFaces; f++)
		{
			aiFace& face = srcMesh->mFaces[f];

			for (uint32 k = 0; k < face.mNumIndices; k++)
			{
				mesh->indices.push_back(face.mIndices[k] + startVertex);
			}
		}

		_meshes.push_back(mesh);
	}
}
void Converter::ReadSkinData()
{
	for (uint32 i = 0; i < _scene->mNumMeshes; i++)
	{
		aiMesh* srcMesh = _scene->mMeshes[i];
		if (srcMesh->HasBones() == false)
		{
			continue;
		}

		shared_ptr<asMesh> mesh = _meshes[i];

		vector<asBoneWeight> tempVertexBoneWeights;
		tempVertexBoneWeights.resize(mesh->vertices.size());


		// Bone
		for (uint32 b = 0; b < srcMesh->mNumBones; b++)
		{
			aiBone* srcMeshBone = srcMesh->mBones[b];
			uint32 boneIndex = GetBoneIndex(srcMeshBone->mName.C_Str());

			for (uint32 w = 0; w < srcMeshBone->mNumWeights; w++)
			{
				uint32 index = srcMeshBone->mWeights[w].mVertexId;
				float weight = srcMeshBone->mWeights[w].mWeight;

				// TODO
				tempVertexBoneWeights[index].AddWeights(boneIndex, weight);
			}
		}

		for (uint32 v = 0; v < tempVertexBoneWeights.size(); v++)
		{
			tempVertexBoneWeights[v].Normalize();

			asBlendWeight blendWeight = tempVertexBoneWeights[v].GetBlendWeight();
			mesh->vertices[v].blendIndices = blendWeight.indices;
			mesh->vertices[v].blendWeights = blendWeight.weights;
		}
	}
		



}


//#define WRITE_MODENAME_IN_CSV
#define MODEL_READ_WRITE_IN_BINARY_DATA
//#define MODEL_READ_WRITE_IN_XML
void Converter::WriteModelFile(wstring finalPath)
{
#ifdef MODEL_READ_WRITE_IN_BINARY_DATA
	wstring binaryPath = finalPath + L".mesh";
	filesystem::path path = filesystem::path(binaryPath);

	filesystem::create_directory(path.parent_path());


	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(binaryPath, FileMode::Write);

	// Bone Data
	file->Write<uint32>(_bones.size());
	for (const shared_ptr<asBone>& bone : _bones)
	{
		file->Write<string>(bone->name);
		file->Write<int32>(bone->index);
		file->Write<int32>(bone->parent);
		file->Write<Matrix>(bone->transform);
	}


	// Mesh Data
	file->Write<uint32>(_meshes.size());
	for (const shared_ptr<asMesh>& mesh : _meshes)
	{
		file->Write<string>(mesh->name);
		file->Write<int32>(mesh->boneIndex);
		file->Write<string>(mesh->materialName);

		// VertexDatas
		uint32 verticesSize = mesh->vertices.size();
		file->Write<uint32>(verticesSize);
		file->Write(&mesh->vertices[0], sizeof(VertexType) * verticesSize);

		// IndexDatas
		uint32 indicesSize = mesh->indices.size();
		file->Write<uint32>(indicesSize);
		file->Write(&mesh->indices[0], sizeof(uint32) * indicesSize);
	}

#ifdef WRITE_MODENAME_IN_CSV
	string csvSavePath = Utils::ToString(finalPath) + "_MeshBoneInfo.csv";
	FILE* csvFile;
	fopen_s(&csvFile, csvSavePath.c_str(), "w");

	vector<int> order;
	unordered_map<int, string> boneUMap;
	for (const shared_ptr<asBone>& bone : _bones)
	{
		int index = bone->index;


		if (bone->parent == -1)
		{
			order.push_back(index);
			boneUMap.insert(std::make_pair(bone->index, bone->name));
		}
		else
		{
			int parentIndex = bone->parent;
			int parentCount = 1;
			
			string st = ",";
			while(true)
			{
				if (_bones[parentIndex]->parent == -1)
				{
					break;
				}

				parentIndex = _bones[parentIndex]->parent;
				parentCount++;
				st += ",";
			}

			st += _bones[index]->name;

			auto it = std::find(order.begin(), order.end(), parentIndex);

			order.insert(it + 0, index);
			boneUMap.insert(std::make_pair(bone->index, st));
		}
	}

	for (int i : order)
	{
		string st = boneUMap[i];
		fprintf_s(csvFile, "%s \n", st.c_str());
	}

	fclose(csvFile);

#endif

#elif defined(MODEL_READ_WRITE_IN_XML)
	XmlDocument doc;
	tinyxml2::XMLDeclaration* dec = doc.NewDeclaration();
	doc.LinkEndChild(dec);

	XmlElement* rootElement = doc.NewElement("Model");
	doc.LinkEndChild(rootElement);

	XmlElement* bonesDataElement = doc.NewElement("BonesData");
	rootElement->LinkEndChild(bonesDataElement);

	XmlElement* boneElement;

	for (const shared_ptr<asBone>& bone : _bones)
	{
		boneElement = doc.NewElement("Bone");
		bonesDataElement->LinkEndChild(boneElement);

		XmlElement* boneNameElement = doc.NewElement("BoneName");
		boneElement->LinkEndChild(boneNameElement);
		boneNameElement->SetText(bone->name.c_str());

		boneElement->SetAttribute("Index", bone->index);
		boneElement->SetAttribute("ParentsIndex", bone->parent);

		XmlElement* boneTransformElement = doc.NewElement("BoneTransform");
		boneElement->LinkEndChild(boneTransformElement);

		for (int i = 0; i < 4; i++)
		{
			XmlElement* rowElement = doc.NewElement(("Row" + std::to_string(i)).c_str());
			boneTransformElement->LinkEndChild(rowElement);
			for (int j = 0; j < 4; j++)
			{
				rowElement->SetAttribute(("Col" + std::to_string(j)).c_str(), bone->transform[i][j]);
			}
		}
	}

	XmlElement* meshDatasElement = doc.NewElement("MeshDatasElement");
	rootElement->LinkEndChild(meshDatasElement);

	for (const shared_ptr<asMesh>& mesh : _meshes)
	{
		XmlElement* meshElement = doc.NewElement("Mesh");
		meshDatasElement->LinkEndChild(meshElement);

		XmlElement* meshNameElement = doc.NewElement("MeshName");
		meshElement->LinkEndChild(meshNameElement);
		meshNameElement->SetText(mesh->name.c_str());

		meshElement->SetAttribute("BoneIndex", mesh->boneIndex);

		XmlElement* materialNameElement = doc.NewElement("MaterialName");
		meshElement->LinkEndChild(materialNameElement);
		materialNameElement->SetText(mesh->materialName.c_str());

		XmlElement* vertexDatasElement = doc.NewElement("VertexDatas");
		meshElement->LinkEndChild(vertexDatasElement);

		XmlElement* vertexElement;
		for (const VertexTextureNormalTangentBlendData& vertex : mesh->vertices)
		{
			vertexElement = doc.NewElement("Vertex");
			vertexDatasElement->LinkEndChild(vertexElement);

			XmlElement* pos = doc.NewElement("Pos");
			vertexElement->LinkEndChild(pos);
			pos->SetAttribute("X", vertex.position.X);
			pos->SetAttribute("Y", vertex.position.Y);
			pos->SetAttribute("Z", vertex.position.Z);


			XmlElement* uv = doc.NewElement("UV");
			vertexElement->LinkEndChild(uv);
			uv->SetAttribute("X", vertex.uv.x);
			uv->SetAttribute("Y", vertex.uv.y);

			XmlElement* normal = doc.NewElement("Normal");
			vertexElement->LinkEndChild(normal);
			normal->SetAttribute("X", vertex.normal.X);
			normal->SetAttribute("Y", vertex.normal.Y);
			normal->SetAttribute("Z", vertex.normal.Z);

			XmlElement* tangent = doc.NewElement("Tangent");
			vertexElement->LinkEndChild(tangent);
			tangent->SetAttribute("X", vertex.tangent.X);
			tangent->SetAttribute("Y", vertex.tangent.Y);
			tangent->SetAttribute("Z", vertex.tangent.Z);


			XmlElement* blendIndices = doc.NewElement("BlendIndices");
			vertexElement->LinkEndChild(blendIndices);
			blendIndices->SetAttribute("X", vertex.blendIndices.X);
			blendIndices->SetAttribute("Y", vertex.blendIndices.Y);
			blendIndices->SetAttribute("Z", vertex.blendIndices.Z);
			blendIndices->SetAttribute("W", vertex.blendIndices.W);

			XmlElement* blendWeights = doc.NewElement("BlendWeights");
			vertexElement->LinkEndChild(blendWeights);
			blendWeights->SetAttribute("X", vertex.blendWeights.X);
			blendWeights->SetAttribute("Y", vertex.blendWeights.Y);
			blendWeights->SetAttribute("Z", vertex.blendWeights.Z);
			blendWeights->SetAttribute("W", vertex.blendWeights.W);
		}


		XmlElement* indexDatasElement = doc.NewElement("IndexDatas");
		meshElement->LinkEndChild(indexDatasElement);

		XmlElement* indicesInfo = doc.NewElement("IndicesInfo");
		indexDatasElement->LinkEndChild(indicesInfo);

		indicesInfo->SetAttribute("Count", static_cast<uint32>(mesh->indices.size()));
		int rowCount = 10;
		indicesInfo->SetAttribute("RowCount", rowCount);

		XmlElement* indices = doc.NewElement("Indices");
		indexDatasElement->LinkEndChild(indices);
		int i = 0;
		XmlElement* indexRow = nullptr;
		for (const uint32& index : mesh->indices)
		{
			if (i % rowCount == 0)
			{
				indexRow = doc.NewElement("Row");
				indices->LinkEndChild(indexRow);
			}
			
			
			indexRow->SetAttribute(("Index" + std::to_string(i++ % rowCount)).c_str(), index);
		}
	}


	wstring xmlPath = finalPath + L"_Mesh.xml";
	filesystem::path path = filesystem::path(xmlPath);

	filesystem::create_directory(path.parent_path());


	doc.SaveFile(Utils::ToString(xmlPath).c_str());

#else
	assert(0);
#endif

}
























































/*-------------------
	 MaterialData
-------------------*/
void Converter::ExportMaterialData(wstring savePath)
{
	wstring finalPath = TEXTURE_PATH + savePath + L".xml";
	ReadMaterialData();
	WriteMaterialData(finalPath);
}

void Converter::ReadMaterialData()
{
	for (uint32 i = 0; i < _scene->mNumMaterials; i++)
	{
		aiMaterial* srcMaterial = _scene->mMaterials[i];

		shared_ptr<asMaterial> material = make_shared<asMaterial>();

		material->name = srcMaterial->GetName().C_Str();


		{
			aiColor3D color;
			// Ambient
			srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
			material->ambient = Color(color.r, color.g, color.b, 1.f);

			// Diffuse
			srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			material->diffuse = Color(color.r, color.g, color.b, 1.f);

			// Sepcular
			srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
			material->specular = Color(color.r, color.g, color.b, 1.f);
			srcMaterial->Get(AI_MATKEY_SHININESS, material->specular.w);

			// Emissive
			srcMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color);
			material->emissive = Color(color.r, color.g, color.b, 1.f);
		}

		{
			aiString file;

			// Diffuse Texture
			srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
			material->diffuseFile = file.C_Str();

			// Specular Texture
			srcMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
			material->specularFile = file.C_Str();

			// Normal Texture
			srcMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
			material->normalFile = file.C_Str();
		}

		_materials.push_back(material);
	}

}
void Converter::WriteMaterialData(wstring finalPath)
{
	filesystem::path path = filesystem::path(finalPath);

	filesystem::create_directory(path.parent_path()); 

	string folder = path.parent_path().string(); 


	shared_ptr<tinyxml2::XMLDocument> document = make_shared<tinyxml2::XMLDocument>();

	tinyxml2::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	tinyxml2::XMLElement* root = document->NewElement("Materials");
	document->LinkEndChild(root);

	for (shared_ptr<asMaterial> material : _materials)
	{
		tinyxml2::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node);

		tinyxml2::XMLElement* element = nullptr;

		element = document->NewElement("Name");
		element->SetText(material->name.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("DiffuseFile");
		element->SetText(WriteTexture(folder, material->diffuseFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("SpecularFile");
		element->SetText(WriteTexture(folder, material->specularFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("NormalFile");
		element->SetText(WriteTexture(folder, material->normalFile).c_str());
		node->LinkEndChild(element);


		element = document->NewElement("Ambient");
		element->SetAttribute("R", material->ambient.x);
		element->SetAttribute("G", material->ambient.y);
		element->SetAttribute("B", material->ambient.z);
		element->SetAttribute("A", material->ambient.w);
		node->LinkEndChild(element);

		element = document->NewElement("Diffuse");
		element->SetAttribute("R", material->diffuse.x);
		element->SetAttribute("G", material->diffuse.y);
		element->SetAttribute("B", material->diffuse.z);
		element->SetAttribute("A", material->diffuse.w);
		node->LinkEndChild(element);

		element = document->NewElement("Specular");
		element->SetAttribute("R", material->specular.x);
		element->SetAttribute("G", material->specular.y);
		element->SetAttribute("B", material->specular.z);
		element->SetAttribute("A", material->specular.w);
		node->LinkEndChild(element);

		element = document->NewElement("Emissive");
		element->SetAttribute("R", material->emissive.x);
		element->SetAttribute("G", material->emissive.y);
		element->SetAttribute("B", material->emissive.z);
		element->SetAttribute("A", material->emissive.w);
		node->LinkEndChild(element);
	}

	document->SaveFile(Utils::ToString(finalPath).c_str());
}
string Converter::WriteTexture(string saveFolder, string file)
{
	string fileName = filesystem::path(file).filename().string();
	string folderName = filesystem::path(saveFolder).filename().string();

	const aiTexture* srcTexture = _scene->GetEmbeddedTexture(file.c_str());
	if (srcTexture)
	{
		//string pathStr = saveFolder + fileName;
		string pathStr = (filesystem::path(saveFolder) / fileName).string();

		if (srcTexture->mHeight == 0)
		{
			shared_ptr<FileUtils> file = make_shared<FileUtils>();
			file->Open(Utils::ToWString(pathStr), FileMode::Write);
			file->Write(srcTexture->pcData, srcTexture->mWidth);
		}
		else
		{
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
			desc.Width = srcTexture->mWidth;
			desc.Height = srcTexture->mHeight;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_IMMUTABLE;

			D3D11_SUBRESOURCE_DATA subResource = { 0 };
			subResource.pSysMem = srcTexture->pcData;

			ComPtr<ID3D11Texture2D> texture;
			HRESULT hr = DEVICE->CreateTexture2D(&desc, &subResource, texture.GetAddressOf());
			CHECK(hr);

			DirectX::ScratchImage img;
			::CaptureTexture(DEVICE.Get(), DC.Get(), texture.Get(), img);

			// Save To File
			hr = DirectX::SaveToDDSFile(*img.GetImages(), DirectX::DDS_FLAGS_NONE, Utils::ToWString(fileName).c_str());
			CHECK(hr);
		}
	}
	else
	{
		string originStr = (filesystem::path(FBX_PATH) / folderName / file).string();
		Utils::Replace(originStr, "\\", "/");

		string pathStr = (filesystem::path(saveFolder) / fileName).string();
		Utils::Replace(pathStr, "\\", "/");



		::CopyFileA(originStr.c_str(), pathStr.c_str(), false);
	}

	return fileName;
}


























/*----------------------
	  AnimationData
----------------------*/

void Converter::ExportAnimationData(wstring savePath, uint32 index/*=0*/)
{
	wstring finalPath = MODEL_PATH + savePath;;
	assert(index < _scene->mNumAnimations);
	shared_ptr<asAnimation> animation = ReadAnimationData(_scene->mAnimations[index]);
	WriteAnimationData(animation, finalPath);
}
shared_ptr<asAnimation> Converter::ReadAnimationData(aiAnimation* srcAnimation)
{
	shared_ptr<asAnimation> animation = make_shared<asAnimation>();
	animation->name = srcAnimation->mName.C_Str();
	animation->frameRate = srcAnimation->mTicksPerSecond;
	animation->frameCount = (uint32)srcAnimation->mDuration + 1;
	
	map<string, shared_ptr<vector<asKeyframeData>>> cacheAnimNodes;

	for (uint32 i = 0; i < srcAnimation->mNumChannels; i++)
	{
		aiNodeAnim* srcNode = srcAnimation->mChannels[i];

		shared_ptr<vector<asKeyframeData>> keyframeDatas = ParseAnimationNode(animation, srcNode);

		animation->duration = max(animation->duration, keyframeDatas->back().time);

		cacheAnimNodes[srcNode->mNodeName.C_Str()] = keyframeDatas;
	}

	ReadKeyframeData(animation, _scene->mRootNode, cacheAnimNodes);

	return animation;
}

shared_ptr<vector<asKeyframeData>> Converter::ParseAnimationNode(shared_ptr<asAnimation> animation, 
	aiNodeAnim* srcNode)
{
	shared_ptr<vector<asKeyframeData>> keyframeDatas = make_shared<vector<asKeyframeData>>();
	
	uint32 keyCount = max(max(srcNode->mNumPositionKeys, srcNode->mNumRotationKeys),
		srcNode->mNumScalingKeys);

	for (uint32 k = 0; k < keyCount; k++)
	{
		asKeyframeData frameData;

		bool found = false;
		uint32 t = keyframeDatas->size();

		// Position
		if (::fabsf((float)srcNode->mPositionKeys[k].mTime - (float)t) <= 0.0001f)
		{
			aiVectorKey key = srcNode->mPositionKeys[k];
			frameData.time = (float)key.mTime;
			::memcpy_s(&frameData.translation, sizeof(Vec3), &key.mValue, sizeof(aiVector3D));


			found = true;
		}

		// Rotation
		if (::fabsf((float)srcNode->mRotationKeys[k].mTime - (float)t) <= 0.0001f)
		{
			aiQuatKey key = srcNode->mRotationKeys[k];
			frameData.time = (float)key.mTime;

			frameData.rotation.X = -key.mValue.x;
			frameData.rotation.Y = -key.mValue.y;
			frameData.rotation.Z = -key.mValue.z;
			frameData.rotation.W = key.mValue.w;

			found = true;
		}

		// Scale
		if (::fabsf((float)srcNode->mScalingKeys[k].mTime - (float)t) <= 0.0001f)
		{
			aiVectorKey key = srcNode->mScalingKeys[k];
			frameData.time = (float)key.mTime;
			::memcpy_s(&frameData.scale, sizeof(Vec3), &key.mValue, sizeof(aiVector3D));

			found = true;
		}

		if (found == true)
		{
			keyframeDatas->push_back(frameData);
		}
	}

	if (keyframeDatas->size() < animation->frameCount)
	{
		uint32 count = animation->frameCount - keyframeDatas->size();
		asKeyframeData keyframe = keyframeDatas->back();

		for (uint32 n = 0; n < count; n++)
		{ 
			keyframeDatas->push_back(keyframe);
		}
	}

	return keyframeDatas;
}

void Converter::ReadKeyframeData(shared_ptr<asAnimation> animation, aiNode* srcNode,
	map<string, shared_ptr<vector<asKeyframeData>>>& cache)
{
	shared_ptr<asKeyframe> keyframe = make_shared<asKeyframe>();
	keyframe->boneName = srcNode->mName.C_Str();

	shared_ptr<vector<asKeyframeData>> findKeyframes = cache[srcNode->mName.C_Str()];
	
	for (uint32 i = 0; i < animation->frameCount; i++)
	{
		asKeyframeData frameData;

		if (findKeyframes == nullptr)
		{
			frameData.time = (float)i;
			Matrix transform(srcNode->mTransformation[0]);
			transform = transform.Transpose();
			transform.Decompose(OUT frameData.scale, OUT frameData.rotation, OUT frameData.translation);
		}
		else
		{
			frameData = (*findKeyframes)[i];
		}

		keyframe->transforms.push_back(frameData);
	}

	animation->keyframes.push_back(keyframe);

	for (uint32 i = 0; i < srcNode->mNumChildren; i++)
	{
		ReadKeyframeData(animation, srcNode->mChildren[i], cache);
	}
} 


#define ANIMATION_READ_WRITE_IN_BINARY_DATA
//#define ANIMATION_READ_WRITE_IN_XML
void Converter::WriteAnimationData(shared_ptr<asAnimation> animation, wstring finalPath)
{
#ifdef ANIMATION_READ_WRITE_IN_BINARY_DATA
	wstring wPath = finalPath + L".clip";
	filesystem::path path = filesystem::path(wPath);
	filesystem::create_directory(path.parent_path());

	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(wPath, FileMode::Write);

	file->Write<string>(animation->name);
	file->Write<float>(animation->duration);
	file->Write<float>(animation->frameRate);
	file->Write<uint32>(animation->frameCount);

	file->Write<uint32>(animation->keyframes.size());

	for (shared_ptr<asKeyframe> keyframe : animation->keyframes)
	{
		file->Write<string>(keyframe->boneName);

		file->Write<uint32>(keyframe->transforms.size());
		file->Write(&keyframe->transforms[0], sizeof(asKeyframeData) * keyframe->transforms.size());
	}
#endif
#if defined(ANIMATION_READ_WRITE_IN_XML)
	XmlDocument doc;
	tinyxml2::XMLDeclaration* dec = doc.NewDeclaration();
	doc.LinkEndChild(dec);


	XmlElement* root = doc.NewElement("AnimationData");
	doc.LinkEndChild(root);

	XmlElement* summaryElement = doc.NewElement("Summary");
	root->LinkEndChild(summaryElement);

	XmlElement* nameElement = doc.NewElement("Name");
	summaryElement->LinkEndChild(nameElement);
	nameElement->SetText(animation->name.c_str());

	summaryElement->SetAttribute("duration", animation->duration);
	summaryElement->SetAttribute("frameRate", animation->frameRate);
	summaryElement->SetAttribute("FrameCount", animation->frameCount);

	XmlElement* keyframesElement = doc.NewElement("Keyframes");
	root->LinkEndChild(keyframesElement);


	for (shared_ptr<asKeyframe> keyframe : animation->keyframes)
	{
		XmlElement* keyframeElement = doc.NewElement("Keyframe");
		keyframesElement->LinkEndChild(keyframeElement);

		XmlElement* boneNameElement = doc.NewElement("BoneName");
		keyframeElement->LinkEndChild(boneNameElement);
		boneNameElement->SetText(keyframe->boneName.c_str());

		XmlElement* keyframeDatasElement = doc.NewElement("KeyframeDatas");
		keyframeElement->LinkEndChild(keyframeDatasElement);
		for (const asKeyframeData& keyframeData : keyframe->transforms)
		{
			XmlElement* keyframeDataElement = doc.NewElement("KeyframeData");
			keyframeDatasElement->LinkEndChild(keyframeDataElement);

			keyframeDataElement->SetAttribute("Time", keyframeData.time);

			keyframeDataElement->SetAttribute("ScaleX", keyframeData.scale.X);
			keyframeDataElement->SetAttribute("ScaleY", keyframeData.scale.Y);
			keyframeDataElement->SetAttribute("ScaleZ", keyframeData.scale.Z);

			keyframeDataElement->SetAttribute("QuaternionX", keyframeData.rotation.X);
			keyframeDataElement->SetAttribute("QuaternionY", keyframeData.rotation.Y);
			keyframeDataElement->SetAttribute("QuaternionZ", keyframeData.rotation.Z);
			keyframeDataElement->SetAttribute("QuaternionW", keyframeData.rotation.W);

			keyframeDataElement->SetAttribute("TranslationX", keyframeData.translation.X);
			keyframeDataElement->SetAttribute("TranslationY", keyframeData.translation.Y);
			keyframeDataElement->SetAttribute("TranslationZ", keyframeData.translation.Z);
		}		
	}


	string sPath = Utils::ToString(finalPath);
	sPath = sPath + ".xml";

	filesystem::path xmlPath = filesystem::path(sPath);
	filesystem::create_directory(xmlPath.parent_path());

	doc.SaveFile(sPath.c_str());
#endif

}

uint32 Converter::GetBoneIndex(const string& name)
{
	for (shared_ptr<asBone>& bone : _bones)
	{
		if (bone->name == name)
		{
			return bone->index;
		}
	}

	assert(0);
	return 0;
}
