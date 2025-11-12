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

	{ // 디버그 용도 스킨데이터 CSV 파일 만들기 ( 아래 WriteModelFile 에 바이너리 데이터로 실데이터는 저장됨 )
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
	 //※ SimpleMath Matrix 는 float 주소 하나 주면, 채워주는 코드 있는데, myMatrix에는 안만들었어서, 이렇게 처리
	Matrix localTransform = MyMathUtils::SimpleMatrixToMyMatrix(dxMat);
	bone->transform = localTransform.Transpose(); // ※ FBX 는 col_major 로 작성되기에, Transpose 필요
	// ※ 여기서 얻는 SRT 는 로컬 트랜스폼

	Matrix parentWorld = Matrix::Identity;
	if (parent >= 0)
	{
		parentWorld = _bones[parent]->transform;
	}
	bone->transform = bone->transform * parentWorld; 
	// ※ bone->transform 에 로컬 트랜스폼이 아닌, 월드 트랜스폼을 저장. FBX에서 오는 transform 은 로컬트랜스폼 정보
	_bones.push_back(bone);




	// Mesh
	ReadMeshData(node, index);



	// 재귀함수
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
				/*※ UV 맵을 여러장 사용하는 것에 대비해, [][] 2차원 배열이지만, 대부분 UV를 한장 사용하기 때문에
					mTextureCoords[0][n] 의 형태로 거의 고정되어 사용 */
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
	/* ※ FBX 에는 정점별로 본들의 Weight 가 저장돼있는 게 아니라, 본별로 Weight > 0 인 정점들이 기록돼있기 때문에,
		 코드가 아래와 같다 */

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


		// Bone 을 순회하면서 연관된 VertexId, Weight 를 구해서 기록한다
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

		// 최종 결과 계산
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

	// 파일이 없으면 만든다
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

	// 파일이 없으면 만든다
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
			srcMaterial->Get(AI_MATKEY_SHININESS, material->specular.w); // ※ 여기서 w는 alpha 가 아닌, 반사광의 강도 제곱승값으로 사용

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
	// ※ 최종이 확장자이니, parent_path() 는 확장자가 담긴 파일명. 위 코드는 해당 파일을 생성하는 코드 (이미 있다면 X)

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
		/*	○ string 의 + 와 filesystem 의 /
			 - 위의 주석이 달린 부분으로 pathStr 을 만들면, saveFolder의 끝이 \ 가 아닐시, 폴더가 fileNAme 과 단순히
			   합쳐져서, 파일명이 이상해진다.
			 - 반면 filesystem 의 / 는, \ 가 없을시, \ 를 추가하여 폴더 를 구분하고, \ 가 이미 있을시, + 와 동일하게
			   작동한다
			 - 따라서 filesystem 의 / 를 애용하자
		*/

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

	/*
		- 총 3분기로 FBX 에 담긴 텍스쳐 정보를 새로 지정한 폴더에 PNG JPG DDS 파일 중 하나로 복사하고, XML 에 담기 위한
		  복사된 파일이 있는 경로 주소를 string 으로 리턴하는 함수
		
		- 3분기별로 나누어 설명하면,
		 1) FBX 파일 내에 JPG 또는 PNG 파일이 위치한 경우
		  - 조건은 if(srcTexture) && if(srcTexture->mHeight == 0) 
		  - 텍스쳐로 PNG, JPG 로 압축되어 저장된 경우, mHeight 가 계산되지 않은 디펄트 상태(0) 이므로, mHeight == 0 이라면,
		    압축이 풀리기 전인 PNG JPG 상태이므로, FBX 내 파일(PNG JPG)을 저장 경로에 복사후, 경로만 리턴
		 2) (PNG, JPG) 파일의 압축이 풀린 상태로, FBX 파일 내에 위치한 경우
		  - 조건은 if(srcTexture) && else
		  - PNG, JPG 의 압축이 풀려, [r8.0] [g8.0] [b8.0] [r1.0] ... 형태의 배열로 데이터가 저장된 경우에 해당
		  - 해당 경우, mHeight 가 계산되므로, > 0 인 경우에 해당
		  - 배열 데이터를 CreateTexture2D로 텍스쳐로 만든 후, CaptureTexture 로 GPU 의 데이터를 CPU 로 가져와 Img 로 만든 후,
		    SaveToDDSFile 로 Img 를 DDS (DirectX 의 이미지 저장 형식) 로 해당 경로에 저장 및 저장 경로를 리턴
		 3) FBX 파일 밖에 PNG JPG 가 위치한 경우
		  - 조건은 else 
		  - 가장 흔한 형태로, FBX 파일 밖에 PNG JPG 가 위치한 경우. (Mesh AI 도 이런 형태로 줌)
		  - CopyFileA 로 기존 경로의 JPG PNG 를 새로운 저장 경로에 복사하고, 경로를 리턴

	
		※ 위 3경로 모두, 텍스쳐 정보가 정점이 아닌 별도의 파일로 저장됨
		   이전에 MeshAI 에서 받은 오브젝트에, 땜빵으로 면에 색을 칠해서, 기존 텍스쳐에 오버라이드돼서 사용했던 경우는 위 3분기
		   에 해당되지 않은 형태로, 정점에 컬러 정보가 담겨 있는 경우 
		   ( asMesh::mColors 같은 형태로 데이터가 담김. 현재 작성 기준 구조체에 없으므로, 그런 형태는 지원 안하는 듯 )
	

		※	FileUtils 는 바이너리 데이터를 읽고/쓰기 위해 직접 만든 클래스이다. PNG, JPG 클래스 또한 바이너리 데이터로,
			읽기, 쓰기가 가능하다
	*/
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
	/* ※ mDuration 의 단위는 초 가 아닌 프레임 단위.따라서 애니매이션의 전체 시간(초) 는 mDuration이 아닌,
		  mDuration / mTickPerSecond 이다.
		  frameCount 에 + 1 을 한 이유는 mDuration의 사작 프레임 인덱스가 0이기 때문
		  
		  ※ 정확히는 mDuration 의 단위가 프레임이 아닌 Tick 이라 하는데, 프레임 이라는 개념에 더 가까운 것 같다 */

	map<string, shared_ptr<vector<asKeyframeData>>> cacheAnimNodes;

	for (uint32 i = 0; i < srcAnimation->mNumChannels; i++)
	{
		aiNodeAnim* srcNode = srcAnimation->mChannels[i];

		// 애니매이션 노드 데이터 파싱
		shared_ptr<vector<asKeyframeData>> keyframeDatas = ParseAnimationNode(animation, srcNode);

		// 현재 찾은 노드 중에 제일 긴 시간으로 애니매이션 시간 갱신
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

	// Keyframe 늘려주기
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


	/*
		- 강의 대로 따라 적었지만, 납득이 되지 않는다. mPositionKeys || mRotationKeys || mScalingKeys 중 하나의 
		  요소들간에 mTime의 차이가 1미만이라면, 이후에는 모든 요소들이 추출이 되지 않을텐데. if 가 아닌 while을 사용하지
		  않아도 되는 전제가 있다는 건지
	*/
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

	// 애니매이션 키프레임 채우기
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
	filesystem::path path = filesystem::path(wPath); // 폴더가 없으면 만든다
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

	filesystem::path xmlPath = filesystem::path(sPath); // 폴더가 없으면 만든다
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
