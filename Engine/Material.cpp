#pragma once;
#include "pch.h"
#include "Material.h"

Material::Material(shared_ptr<Shader> shader) : Super(ResourceType::Material)
{
	_diffuseEffectBuffer = shader->GetSRV("DiffuseMap");
	_normalEffectBuffer = shader->GetSRV("NormalMap");
	_specularEffectBuffer = shader->GetSRV("SpecularMap");
}

Material::~Material()
{
}



void Material::Update()
{
	RENDER->PushMaterialData(_desc);

	if (_diffuseMap)
	{
		_diffuseEffectBuffer->SetResource(_diffuseMap->GetComPtr().Get());
	}
	if (_normalMap)
	{
		_normalEffectBuffer->SetResource(_normalMap->GetComPtr().Get());
	}
	if (_specularMap)
	{
		_specularEffectBuffer->SetResource(_specularMap->GetComPtr().Get());
	}
}

shared_ptr<Material> Material::Clone()
{
	//return make_shared<Material>(*this);

	shared_ptr<Material> material = make_shared<Material>(GAME->GetShader());

	material->_desc					= _desc;
	material->_diffuseMap			= _diffuseMap;
	material->_normalMap			= _normalMap;
	material->_specularMap			= _specularMap;
	material->_diffuseEffectBuffer	= _diffuseEffectBuffer;
	material->_normalEffectBuffer	= _normalEffectBuffer;
	material->_specularEffectBuffer = _specularEffectBuffer;

	return material;
}
