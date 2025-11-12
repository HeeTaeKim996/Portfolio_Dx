#include "pch.h"
#include "ModelRenderer.h"
#include "Model.h"
#include "Shader.h"
#include "Material.h"
#include "ModelMesh.h"
#include "tinyxml2.h"
#include "Utils.h"
#include "ResourceManager.h"

ModelRenderer::ModelRenderer(shared_ptr<Shader> shader) : Super(ComponentType::ModelRenderer, shader)
{
}

ModelRenderer::~ModelRenderer()
{
}

void ModelRenderer::Update()
{
	_gameObject.lock()->SetRenderIndex(ComponentType::ModelRenderer);
}

void ModelRenderer::Render()
{
	if (_model == nullptr)
		return;

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

void ModelRenderer::SetModel(shared_ptr<Model> model)
{
	_model = model;

	vector<shared_ptr<Material>>& materials = _model->GetMaterials();
}

void ModelRenderer::LoadXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent)
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

void ModelRenderer::WriteXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent)
{
	XmlElement* modelAnimatorElement = doc.NewElement("ModelRenderer");
	parent->LinkEndChild(modelAnimatorElement);

	XmlElement* modelNameElement = doc.NewElement("Name");
	modelAnimatorElement->LinkEndChild(modelNameElement);
	modelNameElement->SetText(Utils::ToString(_model->GetName()).c_str());

	modelAnimatorElement->SetAttribute("Pass", static_cast<int>(_pass));
}

shared_ptr<Component> ModelRenderer::Clone() const
{
	shared_ptr<ModelRenderer> copy = make_shared<ModelRenderer>(*this);

	return copy;
	
}
