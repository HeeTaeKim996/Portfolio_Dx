#include "pch.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Game.h"
#include "Mesh.h"
#include "Shader.h"
#include "RenderManager.h"
#include "Material.h"
#include "tinyxml2.h"
#include "Utils.h"
#include "ResourceManager.h"

MeshRenderer::MeshRenderer(shared_ptr<Shader> shader) : Super(ComponentType::MeshRenderer, shader)
{
}

MeshRenderer::~MeshRenderer()
{
}



void MeshRenderer::Update()
{
	_gameObject.lock()->SetRenderIndex(ComponentType::MeshRenderer);
}

void MeshRenderer::Render ()
{


	if (_mesh == nullptr || _material == nullptr)
	{
		return;
	}

	if (_shader == nullptr)
	{
		return;
	}

	_material->Update();


	Matrix world = GetTransform()->GetSRT();
	RENDER->PushTransformData(TransformDesc{ world });

	uint32 stride = _mesh->GetVertexBuffer()->GetStride();
	uint32 offset = _mesh->GetVertexBuffer()->GetOffset();

	DC->IASetVertexBuffers(0, 1, _mesh->GetVertexBuffer()->GetComPtr().GetAddressOf(), &stride, &offset);
	DC->IASetIndexBuffer(_mesh->GetIndexBuffer()->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);

	_shader->DrawIndexed(2, _pass, _mesh->GetIndexBuffer()->GetCount(), 0, 0);
}









void MeshRenderer::LoadXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent)
{
	XmlElement* meshElement = parent->FirstChildElement("Mesh");
	if (meshElement)
	{
		XmlElement* meshNameElement = meshElement->FirstChildElement("Name");
		wstring meshName = Utils::ToWString(meshNameElement->GetText());
		_mesh = RESOURCES->Get<Mesh>(meshName);
	}

	XmlElement* mateiralElement = parent->FirstChildElement("Material");
	if (mateiralElement)
	{
		XmlElement* materialNameElement = mateiralElement->FirstChildElement("Name");
		wstring materialName = Utils::ToWString(materialNameElement->GetText());
		_material = RESOURCES->Get<Material>(materialName);
	}
}

void MeshRenderer::WriteXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent)
{
	XmlElement* meshRendererElement = doc.NewElement("MeshRenderer");
	parent->LinkEndChild(meshRendererElement);

	if (_mesh)
	{
		XmlElement* meshElement = doc.NewElement("Mesh");
		meshRendererElement->LinkEndChild(meshElement);
		
		XmlElement* meshNameElement = doc.NewElement("Name");
		meshElement->LinkEndChild(meshNameElement);
		meshNameElement->SetText(Utils::ToString(_mesh->GetName()).c_str());
	}
	if (_material)
	{
		XmlElement* materialElement = doc.NewElement("Material");
		meshRendererElement->LinkEndChild(materialElement);

		XmlElement* materialNameElement = doc.NewElement("Name");
		materialElement->LinkEndChild(materialNameElement);
		materialNameElement->SetText(Utils::ToString(_material->GetName()).c_str());
	}
}






shared_ptr<Component> MeshRenderer::Clone() const
{
	shared_ptr<MeshRenderer> copy = make_shared<MeshRenderer>(*this);

	// TODO (깊은 복사가 필요한 부분 수동 입력)

	return copy;
}
