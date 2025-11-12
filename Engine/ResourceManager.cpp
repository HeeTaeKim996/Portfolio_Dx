#include "pch.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"
#include <filesystem>


void ResourceManager::Init()
{
	CreateDefaultMesh();
}

shared_ptr<Texture> ResourceManager::GetOrAddTexture(const wstring& key, const wstring& path)
{
	if (filesystem::exists(filesystem::path(path)) == false) 
		return nullptr;

	return Load<Texture>(key, path);
}

void ResourceManager::CreateDefaultMesh()
{
	{
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->CreateQuad();
		Add(mesh);
	}
	{
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->CreateCube();
		Add(mesh);
	}
	{
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->CreateSphere();
		Add(mesh);
	}
	{
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->CreateCylinder();
		Add(mesh);
	}
}