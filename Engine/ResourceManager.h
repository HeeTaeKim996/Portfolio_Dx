#pragma once

#include "ResourceBase.h"

#if 1
class Shader;
class Texture;
class Mesh;
class Material;
class Model;

class ResourceManager
{
	//DECLARE_SINGLE(ResourceManager);
private:
	ResourceManager() {}

public:
	static ResourceManager* GetInstance()
	{
		static ResourceManager instance;
		return &instance;
	}

public:
	void Init();

	template<typename T>
	shared_ptr<T> Load(const wstring& key, const wstring& path);

	template<typename T>
	bool Add(shared_ptr<T> object);

	template<typename T>
	shared_ptr<T> Get(const wstring& key);

	shared_ptr<Texture> GetOrAddTexture(const wstring& key, const wstring& path);

	template<typename T>
	ResourceType GetResourceType();

private:
	void CreateDefaultMesh();

private:
	wstring _resourcePath;

private:
	using KeyObjMap = map<wstring/*key*/, shared_ptr<ResourceBase>>;
	array<KeyObjMap, RESOURCE_TYPE_COUNT> _resources;
};

template<typename T>
shared_ptr<T>
ResourceManager::Load(const wstring& key, const wstring& path)
{
	auto objectType = GetResourceType<T>();
	KeyObjMap& keyObjMap = _resources[static_cast<uint8>(objectType)];

	auto findIt = keyObjMap.find(key);
	if (findIt != keyObjMap.end())
		return static_pointer_cast<T>(findIt->second);

	shared_ptr<T> object = make_shared<T>();
	object->Load(path);
	keyObjMap[key] = object;

	return object;
}

template<typename T>
bool ResourceManager::Add(shared_ptr<T> object)
{
	ResourceType resourceType = GetResourceType<T>();
	KeyObjMap& keyObjMap = _resources[static_cast<uint8>(resourceType)];

	

	wstring key = object->GetName();
	assert(key != L"");

	auto findIt = keyObjMap.find(key);
	if (findIt != keyObjMap.end())
		return false;



	keyObjMap[key] = object;
	return true;
}

template<typename T>
shared_ptr<T> ResourceManager::Get(const wstring& key)
{
	ResourceType resourceType = GetResourceType<T>();
	KeyObjMap& keyObjMap = _resources[static_cast<uint8>(resourceType)];

	auto findIt = keyObjMap.find(key);
	if (findIt != keyObjMap.end())
		return static_pointer_cast<T>(findIt->second);

	return nullptr;
}

template<typename T>
ResourceType ResourceManager::GetResourceType()
{
	if (std::is_same_v<T, Texture>)		return ResourceType::Texture;
	if (std::is_same_v<T, Mesh>)		return ResourceType::Mesh;
	if (std::is_same_v<T, Material>)	return ResourceType::Material;
	if (std::is_same_v<T, Model>)		return ResourceType::Model;


	assert(false);
	return ResourceType::None;
}
#endif 