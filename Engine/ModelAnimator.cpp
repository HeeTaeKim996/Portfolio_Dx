#include "pch.h"
#include "ModelAnimator.h"
#include "Material.h"
#include "ModelMesh.h"
#include "Model.h"
#include "ModelAnimation.h"
#include "TimeManager.h"
#include "tinyxml2.h"
#include "Utils.h"
#include "ResourceManager.h"

#include "Scene.h" 
#include "ModelMesh.h"

ModelAnimator::ModelAnimator(shared_ptr<Shader> shader) : Super(ComponentType::Animator, shader)
{

}

ModelAnimator::~ModelAnimator()
{
}

void ModelAnimator::Awake()
{
	isPlayingAnimation = true;
}

void ModelAnimator::Start()
{
}

void ModelAnimator::Update()
{
	_gameObject.lock()->SetRenderIndex(ComponentType::Animator);
}

void ModelAnimator::Render()
{
	if (_model == nullptr)
		return;



	if (isPlayingAnimation)
	{
		if (_ikLookData.isAssigned)
		{
			IKRender();
		}
		else if (_underRotateData.isAssigned)
		{
			Test2_UnderRotateRender();
		}
		else
		{
			AnimationRender();
		}
	}
	else
	{
		ModelRender();
	}

}

void ModelAnimator::SetModel(shared_ptr<Model> model)
{
	_model = model;

	vector<shared_ptr<Material>>& materials = _model->GetMaterials();

	if (_texture == nullptr)
	{
		CreateTexture();
	}
}

void ModelAnimator::CreateTexture()
{
	if (_model->GetAnimationCount() == 0)
	{
		return;
	}

	_animTransforms.resize(_model->GetAnimationCount());

	for (uint32 i = 0; i < _model->GetAnimationCount(); i++)
	{
		CreateAnimationTransform(i);
	}

	// Create Texture
	{

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

		desc.Width = MAX_MODEL_TRANSFORMS * 4;
		desc.Height = MAX_MODEL_KEYFRAMES;
		desc.ArraySize = _model->GetAnimationCount();
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;


		const uint32 dataSize = MAX_MODEL_TRANSFORMS * sizeof(Matrix);
		const uint32 pageSize = dataSize * MAX_MODEL_KEYFRAMES;
		void* mallocPtr = ::malloc(pageSize * _model->GetAnimationCount());

		for (uint32 c = 0; c < _model->GetAnimationCount(); c++)
		{
			uint32 startOffset = c * pageSize;
			BYTE* pageStartPtr = reinterpret_cast<BYTE*>(mallocPtr) + startOffset;

			for (uint32 f = 0; f < MAX_MODEL_KEYFRAMES; f++)
			{
				void* ptr = static_cast<void*>(pageStartPtr + f * dataSize);
				::memcpy(ptr, _animTransforms[c].transforms[f].data(), dataSize);
			}
		}

		vector<D3D11_SUBRESOURCE_DATA> subResource(_model->GetAnimationCount());

		for (uint32 c = 0; c < _model->GetAnimationCount(); c++)
		{
			void* ptr = (BYTE*)mallocPtr + c * pageSize;
			subResource[c].pSysMem = ptr;
			subResource[c].SysMemPitch = dataSize;
			subResource[c].SysMemSlicePitch = pageSize;
		}

		HRESULT hr = DEVICE->CreateTexture2D(&desc, subResource.data(), _texture.GetAddressOf());
		CHECK(hr);

		::free(mallocPtr);


		// Create SRV
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));

			desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.MipLevels = 1;
			desc.Texture2DArray.ArraySize = _model->GetAnimationCount();

			HRESULT hr = DEVICE->CreateShaderResourceView(_texture.Get(), &desc, _srv.GetAddressOf());
			CHECK(hr);
		}
	}
}



void ModelAnimator::CreateAnimationTransform(uint32 index)
{
	vector<TransformBase> tempAnimBoneTransforms(MAX_MODEL_TRANSFORMS);

	shared_ptr<ModelAnimation> animation = _model->GetAnimationByIndex(index);

	for (uint32 f = 0; f < animation->frameCount; f++)
	{
		for (uint32 b = 0; b < _model->GetBoneCount(); b++)
		{
			shared_ptr<ModelBone> bone = _model->GetBoneByIndex(b);


			TransformBase animLocal;


			shared_ptr<ModelKeyframe> frame = animation->GetKeyframe(bone->name);
			if (frame != nullptr)
			{
				ModelKeyframeData& data = frame->transforms[f];
				animLocal = TransformBase(data.translation, data.rotation, data.scale);
			}
			else
			{
						
			}


			int32 parentIndex = bone->parentIndex;

			TransformBase parentAnimWorld;
			if (parentIndex >= 0)
			{
				parentAnimWorld = tempAnimBoneTransforms[parentIndex];
			}

			tempAnimBoneTransforms[b] = animLocal.LocalToWorld(parentAnimWorld);


			TransformBase worldTransform(bone->transform);
			TransformBase invWorldTransform = worldTransform.Inverse();

			_animTransforms[index].transforms[f][b]
				= (invWorldTransform.GetSRT() * tempAnimBoneTransforms[b].GetSRT());



			// DEBUG
#if 0 // Obsolate
#include "ETCUtils.h"
			if (f == 1 && b == 1)
			{
				FILE* file;
				::fopen_s(&file, "../AnimData7.csv", "w");

				::fprintf(file, "quaternion\n");
				shared_ptr<ModelKeyframe> frame = animation->GetKeyframe(bone->name);
				ModelKeyframeData& data = frame->transforms[f];
				::fprintf(file, "%f, %f, %f, %f\n",
					data.rotation.X, data.rotation.Y, data.rotation.Z, data.rotation.W);

				::fprintf(file, "1\n");
				ETCUtils_AddMyMatrixToCls(file, animLocal.GetSRT());

				::fprintf(file, "2\n");
				ETCUtils_AddMyMatrixToCls(file, parentAnimWorld.GetSRT());


				::fprintf(file, "invMat\n");
				myMatrix4x4 invMat = invWorldTransform.GetSRT();
				for (int i = 0; i < 4; i++)
				{
					::fprintf(file, "%f, %f, %f, %f \n",
						invMat[i][0], invMat[i][1], invMat[i][2], invMat[i][3]);
				}


				::fprintf(file, "AnimMat\n");
				myMatrix4x4 animMat = tempAnimBoneTransforms[b].GetSRT();
				for (int i = 0; i < 4; i++)
				{
					::fprintf(file, "%f, %f, %f, %f \n",
						animMat[i][0], animMat[i][1], animMat[i][2], animMat[i][3]);
				}


				::fprintf(file, "FinalMat\n");
				myMatrix4x4 finalMat = _animTransforms[index].transforms[f][b];
				for (int i = 0; i < 4; i++)
				{
					::fprintf(file, "%f, %f, %f, %f \n",
						finalMat[i][0], finalMat[i][1], finalMat[i][2], finalMat[i][3]
					);
				}

				fprintf(file, "\n");


				::fclose(file);
			}
#endif
		}
	}
}

void ModelAnimator::AnimationRender()
{
	UpdateTweenDesc();

	RENDER->PushTweenData(_tweenDesc);

	_shader->GetSRV("TransformMap")->SetResource(_srv.Get());


#if 0 // Obsolate
	// Bones
	{
		BoneDesc boneDesc;

		vector<shared_ptr<ModelBone>>& bones = _model->GetBone();
		const uint32 boneCount = _model->GetBoneCount();
		for (uint32 i = 0; i < boneCount; i++)
		{
			shared_ptr<ModelBone> bone = bones[i];
			//boneDesc.transforms[i] = bone->transform;
			boneDesc.transforms[i] = bone->transform;
		}
		RENDER->PushBoneData(boneDesc);
	}
#endif



	// Transform
	{
		Matrix world = GetTransform()->GetSRT();
		RENDER->PushTransformData(TransformDesc{ world });

		vector<shared_ptr<ModelMesh>>& meshes = _model->GetMeshes();
		for (shared_ptr<ModelMesh>& mesh : meshes)
		{
			if (mesh->material)
			{
				mesh->material->Update();
			}

			// BoneIndex
			_shader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);


			uint32 stride = mesh->vertexBuffer->GetStride();
			uint32 offset = mesh->vertexBuffer->GetOffset();

			DC->IASetVertexBuffers(0, 1, mesh->vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
			DC->IASetIndexBuffer(mesh->indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);

			_shader->DrawIndexed(0, _pass, mesh->indexBuffer->GetCount(), 0, 0);
		}
	}
}

void ModelAnimator::ModelRender()
{
	// Bones
	{
		BoneDesc boneDesc;

		vector<shared_ptr<ModelBone>>& bones = _model->GetBone();
		const uint32 boneCount = _model->GetBoneCount();
		for (uint32 i = 0; i < boneCount; i++)
		{
			shared_ptr<ModelBone> bone = bones[i];
			//boneDesc.transforms[i] = bone->transform;
			boneDesc.transforms[i] = bone->transform;
		}
		RENDER->PushBoneData(boneDesc);
	}



	// Transform
	{
		Matrix world = GetTransform()->GetSRT();
		RENDER->PushTransformData(TransformDesc{ world });

		vector<shared_ptr<ModelMesh>>& meshes = _model->GetMeshes();
		for (shared_ptr<ModelMesh>& mesh : meshes)
		{
			if (mesh->material)
			{
				mesh->material->Update();
			}

			// BoneIndex
			_shader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);


			uint32 stride = mesh->vertexBuffer->GetStride();
			uint32 offset = mesh->vertexBuffer->GetOffset();

			DC->IASetVertexBuffers(0, 1, mesh->vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
			DC->IASetIndexBuffer(mesh->indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);

			_shader->DrawIndexed(1, _pass, mesh->indexBuffer->GetCount(), 0, 0);
		}
	}
}

void ModelAnimator::IKRender()
{
	// UpdateTweenDesc
	UpdateTweenDesc();



	// Transform
	vector<TempBoneData> bones = GetTempBoneData();
	uint32 boneSize = _model->GetBoneCount();



	if (_ikHeadIndex == -1)
	{
		shared_ptr<ModelBone> bone = _model->GetBoneByName(L"Head");
		assert(bone != nullptr);
		_ikHeadIndex = bone->index;


		vector<shared_ptr<ModelMesh>> vertexeses = _model->GetMeshes();

		_headMeshPos = myVec3::Zero;
		int size = vertexeses.size();

		float sumWeights = 0.f;

		for (int i = 0; i < size; i++)
		{
			const vector<ModelVertexType>& vertexes = vertexeses[i]->geometry->GetVertices();

			for (int j = 0; j < vertexes.size(); j++)
			{
				float weight = 0.f;

				for (int k = 0; k < 4; k++)
				{
					if (vertexes[j].blendIndices[k] == _ikHeadIndex)
					{
						weight = vertexes[j].blendWeights[k];
					}

					if (weight != 0)
					{
						_headMeshPos += vertexes[j].position * weight;
						sumWeights += weight;
					}
				}
			}
		}


		_headMeshPos /= sumWeights;
	}






	TransformBase headTransform = bones[_ikHeadIndex].world;

	//Vec3 headPos = headTransform.GetPosition();
	//Quaternion headRot = headTransform.GetRotation();


	Matrix worldSRT = GetTransform()->GetSRT();

	Vec3 headPos = (_headMeshPos.ToVec4(1.f) * headTransform.GetSRT() * worldSRT).ToVec3();

	//headPos.Y = _ikLookData.lookingPos.Y;

	Quaternion headRot = headTransform.GetRotation() * GetTransform()->GetRotation();

	Vec3 dir = (headPos - _ikLookData.lookingPos).GetNormalize();

	myVec3 headUp = GetTransform()->GetRotation().RotateVector(headTransform.GetYAxis());
	//Quaternion aimQuat(dir, headTransform.GetYAxis());
	//Quaternion aimQuat(dir, headUp);
	//Quaternion aimQuat(dir, headRot.ToRotateMatrix()[1]);
	Quaternion aimQuat(dir, myVec3::UnitY);





	vector<int> headToHipIndex;

	int index = _ikHeadIndex;
	while (true)
	{
		headToHipIndex.push_back(index);

		int parentIndex = bones[index].parent;
		if (parentIndex == -1)
			break;

		index = parentIndex;
	}

	int ri = 0;
	myQuaternion totalMoveQuat;
	if (headRot.Dot(aimQuat) > 0.0f)
	{
		totalMoveQuat = headRot.Inverse() * aimQuat;
	}
	else
	{
		totalMoveQuat = headRot.Inverse() * (-aimQuat);
	}


	for (int index : headToHipIndex)
	{

		TransformBase world = bones[index].world;
		//myQuaternion quat = world.GetRotation() * myQuaternion::Slerp(headRot, aimQuat, _ikLookData.ratios[ri++]);

		myQuaternion moveQuat = myQuaternion::Slerp(myQuaternion(), totalMoveQuat, _ikLookData.ratios[ri++]);
		myQuaternion finalQuat = bones[index].world.GetRotation() * moveQuat;

		bones[index].world.SetRotation(finalQuat);
	}


	std::function<void(int, int)> UpdateWorld = [&](int parent, int child)
		{
#if 1
			TempBoneData& childBone = bones[child];
			TempBoneData& parentBone = bones[parent];

			childBone.world = childBone.local.LocalToWorld(parentBone.world);

			for (int childsChild : childBone.childrens)
			{
				UpdateWorld(child, childsChild);
			}
#else
			TempBoneData& childBone = bones[child];
			TempBoneData& parentBone = bones[parent];

			myQuaternion worldRot = childBone.local.GetRotation() * parentBone.world.GetRotation();
			childBone.world.SetRotation(worldRot);

			for (int childsChild : childBone.childrens)
			{
				UpdateWorld(child, childsChild);
			}
#endif
		};



	for (int i = headToHipIndex.size() - 1; i >= 0; i--)
	{
		TempBoneData& bone = bones[headToHipIndex[i]];

		for (int child : bone.childrens)
		{
			int special = i > 0 ? headToHipIndex[i - 1] : -1;
			if (child == special)
			{
#if 1
				TempBoneData& childBone = bones[special];

				Quaternion parentWorldQuat = bone.world.GetRotation();
				Quaternion childWorldQuat = childBone.world.GetRotation();

				Quaternion childLocalQuat = childWorldQuat * (parentWorldQuat.Inverse());
				childBone.local.SetRotation(childLocalQuat);

				childBone.world = childBone.local.LocalToWorld(bone.world);

#elif 0
				TempBoneData& childBone = bones[special];

				myVec3 tl = (bone.world.GetRotation().Inverse() * (childBone.world.GetPosition() - bone.world.GetPosition()))
					/ bone.world.GetScale();
				childBone.local.SetPosition(tl);

				Quaternion parentWorldQuat = bone.world.GetRotation();
				Quaternion childWorldQuat = childBone.world.GetRotation();
				Quaternion childLocalQuat = childWorldQuat * (parentWorldQuat.Inverse());
				childBone.local.SetRotation(childLocalQuat);
#else


#endif
			}
			else
			{
				UpdateWorld(headToHipIndex[i], child);
			}
		}

	}



	// Bone Push
	BoneDesc boneDesc;

	for (uint32 i = 0; i < boneSize; i++)
	{
		boneDesc.transforms[i] = bones[i].world.GetSRT();
	}

	RENDER->PushBoneData(boneDesc);





	// Transform Push
	{
		Matrix world = GetTransform()->GetSRT();
		RENDER->PushTransformData(TransformDesc{ world });

		vector<shared_ptr<ModelMesh>>& meshes = _model->GetMeshes();
		for (shared_ptr<ModelMesh>& mesh : meshes)
		{
			if (mesh->material)
			{
				mesh->material->Update();
			}


			// BoneIndex
			_shader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);


			uint32 stride = mesh->vertexBuffer->GetStride();
			uint32 offset = mesh->vertexBuffer->GetOffset();

			DC->IASetVertexBuffers(0, 1, mesh->vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
			DC->IASetIndexBuffer(mesh->indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);

			_shader->DrawIndexed(0, (int)AnimPass::IK, mesh->indexBuffer->GetCount(), 0, 0);
		}
	}

}




























void ModelAnimator::Test2_UnderRotateRender()
{
	// UpdateTweenDesc
	UpdateTweenDesc();


	// ���� Transform ����
	int bonesSize = _model->GetBoneCount();
	vector<TempBoneData> bones(bonesSize);

	auto GetBoneMatrix = [=](bool isFirst, int32 boneIndex) ->myMatrix4x4
		{
			int32 animIndex = isFirst ? _tweenDesc.currentDesc.animIndex : _tweenDesc.reservedDesc.animIndex;
			uint32 currentFrame = isFirst ? _tweenDesc.currentDesc.currentFrame : _tweenDesc.reservedDesc.currentFrame;
			uint32 nextFrame = isFirst ? _tweenDesc.currentDesc.nextFrame : _tweenDesc.reservedDesc.nextFrame;
			float ratio = isFirst ? _tweenDesc.currentDesc.ratio : _tweenDesc.reservedDesc.ratio;

			Matrix currMat = _animTransforms[animIndex].transforms[currentFrame][boneIndex];
			Matrix nextMat = _animTransforms[animIndex].transforms[nextFrame][boneIndex];

			return myMatrix4x4::Lerp(currMat, nextMat, ratio);
		};

	vector<shared_ptr<ModelBone>>& modelBones = _model->GetBone();


	for (shared_ptr<ModelBone>& modelBone : modelBones)
	{
		int index = modelBone->index;
		if (index == -1)
			continue;

		bones[index].world = TransformBase(GetBoneMatrix(true, index));

		if (_tweenDesc.reservedDesc.animIndex >= 0)
		{
			bones[index].local = TransformBase(GetBoneMatrix(false, index));
		}

		bones[index].parent = modelBone->parentIndex;
		if (modelBone->parentIndex != -1)
		{
			bones[modelBone->parentIndex].childrens.push_back(index);
		}
	}




	{ 
		if (_underRotate_Hip_Index == -1)
		{
			shared_ptr<ModelBone> hipBone = _model->GetBoneByName(L"Hips");
			assert(hipBone != nullptr);
			shared_ptr<ModelBone> spine02Bone = _model->GetBoneByName(L"Spine02");
			assert(spine02Bone != nullptr);

			_underRotate_Hip_Index = hipBone->index;
			_underRotate_LowSpine_Index = spine02Bone->index;
		}
	}

	assert(bones[_underRotate_LowSpine_Index].childrens.size() == 1);
	int middleSpineIndex = bones[_underRotate_LowSpine_Index].childrens[0];

	vector<int> underBoneIndexes;

	std::function<void(int)> RegisterUnderBone = [&](int me)
		{
			underBoneIndexes.push_back(me);

			for (int child : bones[me].childrens)
			{
				RegisterUnderBone(child);
			}
		};

	underBoneIndexes.push_back(_underRotate_Hip_Index);
	for (int child : bones[_underRotate_Hip_Index].childrens)
	{
		if (!(child == _underRotate_LowSpine_Index))
		{
			RegisterUnderBone(child);
		}
	}

	::function<void(bool)> RotateBones = [&](bool isFirst)
		{
			float degree = 0.f;

			int animIndex = isFirst ? _underRotateData.animIndexMapping[_tweenDesc.currentDesc.animIndex]
				: _underRotateData.animIndexMapping[_tweenDesc.reservedDesc.animIndex];

			switch (animIndex)
			{
			case UnderRotateData::FORWARD:
			{
				if (_underRotateData.crossY < 0)
				{
					degree = -_underRotateData.diffDegree;
				}
				else
				{
					degree = _underRotateData.diffDegree;
				}
				break;
			}
			case UnderRotateData::RIGHT:
			{
				degree = -_underRotateData.diffDegree + 90.f;
				break;
			}
			case UnderRotateData::LEFT:
			{
				degree = _underRotateData.diffDegree - 90.f;
				break;
			}
			case UnderRotateData::BACKWARD:
			{
				degree = _underRotateData.diffDegree + 180.f;
				if (_underRotateData.crossY < 0)
					degree = -degree;
				break;
			}


			default:
			{
				assert(0);
			}
			}

			Quaternion underQuat(Vec3::UnitY, degree);
			Quaternion lowSpineQuat = Quaternion::Slerp(myQuaternion(), underQuat, _underRotateData.ratios[0]);
			Quaternion middleSpineQuat = Quaternion::Slerp(myQuaternion(), underQuat, _underRotateData.ratios[1]);

			::function<void(const Quaternion&, TransformBase&)> RotateSRT = [](const Quaternion& rotQuat, TransformBase& srt)
				{
					Vec3 rotedTranslation = rotQuat.RotateVector(srt.GetPosition());
					Quaternion rotedQuaternion = srt.GetRotation() * rotQuat;

					srt.SetPosition(rotedTranslation);
					srt.SetRotation(rotedQuaternion);
				};

			if (isFirst)
			{
				RotateSRT(lowSpineQuat, bones[_underRotate_LowSpine_Index].world);
				RotateSRT(middleSpineQuat, bones[middleSpineIndex].world);

				for (int index : underBoneIndexes)
				{
					RotateSRT(underQuat, bones[index].world);
				}
			}
			else
			{
				RotateSRT(lowSpineQuat, bones[_underRotate_LowSpine_Index].local);
				RotateSRT(middleSpineQuat, bones[middleSpineIndex].local);

				for (int index : underBoneIndexes)
				{
					RotateSRT(underQuat, bones[index].local);
				}
			}
		};


	RotateBones(true);
	if (_tweenDesc.reservedDesc.animIndex >= 0)
	{
		RotateBones(false);
	}





	// Bone Push
	BoneDesc boneDesc;
	if (_tweenDesc.reservedDesc.animIndex < 0)
	{
		for (uint32 i = 0; i < bonesSize; i++)
		{
			boneDesc.transforms[i] = bones[i].world.GetSRT();
		}
	}
	else
	{
		for (uint32 i = 0; i < bonesSize; i++)
		{
			boneDesc.transforms[i] = Matrix::Lerp(bones[i].world.GetSRT(), bones[i].local.GetSRT(), _tweenDesc.tweenRatio);
		}
	}

	RENDER->PushBoneData(boneDesc);



	// Transform Push
	{
		Matrix world = GetTransform()->GetSRT();
		RENDER->PushTransformData(TransformDesc{ world });

		vector<shared_ptr<ModelMesh>>& meshes = _model->GetMeshes();
		for (shared_ptr<ModelMesh>& mesh : meshes)
		{
			if (mesh->material)
			{
				mesh->material->Update();
			}

			// BoneIndex
			_shader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);

			uint32 stride = mesh->vertexBuffer->GetStride();
			uint32 offset = mesh->vertexBuffer->GetOffset();

			DC->IASetVertexBuffers(0, 1, mesh->vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
			DC->IASetIndexBuffer(mesh->indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);

			_shader->DrawIndexed(0, (int)AnimPass::IK, mesh->indexBuffer->GetCount(), 0, 0);
		}
	}
}





















































void ModelAnimator::UpdateTweenDesc()
{

	TweenDesc& desc = _tweenDesc;

	float animSpeed = fmaxf(0.f, speed);
	{
		desc.currentDesc.sumTime += DT * animSpeed;

		shared_ptr<ModelAnimation> firstAnim =
			_model->GetAnimationByIndex(desc.currentDesc.animIndex);
		if (firstAnim)
		{
			float timePerFrame = 1.f / (firstAnim->frameRate * desc.currentDesc.speed);
			if (desc.currentDesc.sumTime >= timePerFrame)
			{
				desc.currentDesc.sumTime = 0;
				if (firstAnim->isLoop)
				{
					desc.currentDesc.currentFrame
						= (desc.currentDesc.currentFrame + 1) % firstAnim->frameCount;
					desc.currentDesc.nextFrame
						= (desc.currentDesc.currentFrame + 1) % firstAnim->frameCount;
				}
				else
				{
					int maxFrame = firstAnim->frameCount - 1;
					if (++desc.currentDesc.currentFrame > maxFrame)
					{
						desc.currentDesc.currentFrame = maxFrame;
					}
					if (++desc.currentDesc.nextFrame > maxFrame)
					{
						desc.currentDesc.nextFrame = maxFrame;
					}
				}
			}
			desc.currentDesc.ratio = (desc.currentDesc.sumTime / timePerFrame);
		}
	}

	if (desc.reservedDesc.animIndex >= 0)
	{
		desc.tweenSumTime += DT * animSpeed;
		desc.tweenRatio = desc.tweenSumTime / desc.tweenDuration;

		if (desc.tweenRatio >= 1.f)
		{

			desc.currentDesc = desc.reservedDesc;
			desc.ClearReservedAnim();
		}
		else
		{
			shared_ptr<ModelAnimation> reservedAnim
				= _model->GetAnimationByIndex(desc.reservedDesc.animIndex);
			desc.reservedDesc.sumTime += DT * animSpeed;

			float timePerFrame = 1.f / (reservedAnim->frameRate * desc.reservedDesc.speed);
			if (desc.reservedDesc.sumTime >= timePerFrame)
			{
				int maxFrame = reservedAnim->frameCount - 1;
				if (++desc.reservedDesc.currentFrame > maxFrame)
				{
					desc.reservedDesc.currentFrame = maxFrame;
				}
				if (++desc.reservedDesc.nextFrame > maxFrame)
				{
					desc.reservedDesc.nextFrame = maxFrame;
				}
			}

			desc.reservedDesc.ratio = desc.reservedDesc.sumTime / timePerFrame;
		}

	}
}

vector<TempBoneData> ModelAnimator::GetTempBoneData(bool calcLocal /*= true*/)
{
	vector<TempBoneData> bones(_model->GetBoneCount());

	auto GetBoneMatrix = [=](bool isFirst, int32 boneIndex) ->myMatrix4x4
		{
			int32 animIndex = isFirst ? _tweenDesc.currentDesc.animIndex : _tweenDesc.reservedDesc.animIndex;
			uint32 currentFrame = isFirst ? _tweenDesc.currentDesc.currentFrame : _tweenDesc.reservedDesc.currentFrame;
			uint32 nextFrame = isFirst ? _tweenDesc.currentDesc.nextFrame : _tweenDesc.reservedDesc.nextFrame;
			float ratio = isFirst ? _tweenDesc.currentDesc.ratio : _tweenDesc.reservedDesc.ratio;

			Matrix currMat = _animTransforms[animIndex].transforms[currentFrame][boneIndex];
			Matrix nextMat = _animTransforms[animIndex].transforms[nextFrame][boneIndex];

			return myMatrix4x4::Lerp(currMat, nextMat, ratio);
		};

	vector<shared_ptr<ModelBone>>& modelBones = _model->GetBone();


	for (shared_ptr<ModelBone>& modelBone : modelBones)
	{
		int index = modelBone->index;
		if (index == -1)
			continue;

		Matrix retMat = GetBoneMatrix(true, index);

		if (_tweenDesc.reservedDesc.animIndex >= 0)
		{
			Matrix secondMat = GetBoneMatrix(false, index);

			retMat = myMatrix4x4::Lerp(retMat, secondMat, _tweenDesc.tweenRatio);
		}

		bones[index].world = TransformBase(retMat);
		bones[index].parent = modelBone->parentIndex;
		if (modelBone->parentIndex != -1)
		{
			bones[modelBone->parentIndex].childrens.push_back(index);
		}
	}

	if (calcLocal)
	{
		for (TempBoneData& bone : bones)
		{
			if (bone.parent == -1)
			{
				bone.local = bone.world;
			}
			else
			{
				bone.local = bone.world.WorldToLocal(bones[bone.parent].world);
			}
		}
	}

	return bones;
}




void ModelAnimator::Play(int animIndex)
{
	assert(animIndex >= 0 && animIndex < _model->GetAnimationCount());

	_tweenDesc.ClearReservedAnim();
	_tweenDesc.currentDesc.animIndex = animIndex;
}

void ModelAnimator::CrossFade(int animIndex, float lerpTime/*=1.f*/)
{
	assert(animIndex >= 0 && animIndex < _model->GetAnimationCount());

	if (_tweenDesc.HasReserve())
	{
		_tweenDesc.currentDesc = _tweenDesc.reservedDesc;
	}

	_tweenDesc.tweenDuration = lerpTime;
	_tweenDesc.reservedDesc.animIndex = animIndex;
}

void ModelAnimator::Crossfade_SyncElapsedTime(int animIndex, float lerpTime/*=1.f*/)
{
	assert(animIndex >= 0 && animIndex < _model->GetAnimationCount());

	if (_tweenDesc.HasReserve())
	{
		_tweenDesc.currentDesc = _tweenDesc.reservedDesc;
	}

	_tweenDesc.tweenDuration = lerpTime;
	_tweenDesc.reservedDesc.animIndex = animIndex;


	_tweenDesc.reservedDesc.currentFrame = _tweenDesc.currentDesc.currentFrame; 
	_tweenDesc.reservedDesc.nextFrame = _tweenDesc.currentDesc.nextFrame;
	
	//_model->GetAnimationByIndex(_tweenDesc.currentDesc.animIndex);
	//float currElapseRatio = (_tweenDesc.currentDesc.currentFrame
	//	/ static_cast<float>(_model->GetAnimationByIndex(_tweenDesc.currentDesc.animIndex)->frameCount - 1));

	//int reservesMaxFrame = _model->GetAnimationByIndex(animIndex)->frameCount - 1;
	//int reservesCurrFrame = static_cast<int>(currElapseRatio * reservesMaxFrame);
	//_tweenDesc.reservedDesc.currentFrame = reservesCurrFrame;
	//_tweenDesc.reservedDesc.nextFrame = ++reservesCurrFrame > reservesMaxFrame ? 0 : reservesCurrFrame;
	//_tweenDesc.reservedDesc.ratio = currElapseRatio;


}








void ModelAnimator::LoadXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent)
{
	XmlElement* modelNameElement = parent->FirstChildElement("Name");
	string name = modelNameElement->GetText();

	int intValue;
	parent->QueryIntAttribute("Pass", &intValue);
	_pass = static_cast<uint8>(intValue);


	shared_ptr<Model> model
		= RESOURCES->Get<Model>(Utils::ToWString(name));

	SetModel(model);
}

void ModelAnimator::WriteXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent)
{
	XmlElement* modelAnimatorElement = doc.NewElement("ModelAnimator");
	parent->LinkEndChild(modelAnimatorElement);

	XmlElement* modelNameElement = doc.NewElement("Name");
	modelAnimatorElement->LinkEndChild(modelNameElement);
	modelNameElement->SetText(Utils::ToString(_model->GetName()).c_str());

	modelAnimatorElement->SetAttribute("Pass", static_cast<int>(_pass));
}

shared_ptr<Component> ModelAnimator::Clone() const
{
	shared_ptr<ModelAnimator> copy = make_shared<ModelAnimator>(*this);

	
	return copy;
}
