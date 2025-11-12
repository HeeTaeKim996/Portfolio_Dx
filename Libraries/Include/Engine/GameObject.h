#pragma once
#include "Component.h"

class Monobehaviour;
class Transform;
class Camera;
class MeshRenderer;
class ModelAnimator;
class ModelRenderer;
class Scene;
class BaseCollider;
struct Collision;
class Rigidbody;

class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject();
	~GameObject();


	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FixedUpdate();

	void EditRender();

	void OnEnable();
	void OnDisable();
	void Destroy();

	void OnCollisionEnter(const Collision& collision);
	void OnCollisionStay(const Collision& collision);
	void OnCollisionExit(const Collision& collision);

	shared_ptr<Component> GetFixedComponent(ComponentType type);
	shared_ptr<Transform> GetTransform();
	shared_ptr<Camera> GetCamera();
	shared_ptr<MeshRenderer> GetMeshRenderer();
	shared_ptr<ModelRenderer> GetModelRenderer();
	shared_ptr<ModelAnimator> Animator();
	shared_ptr<BaseCollider> GetCollider();
	shared_ptr<Rigidbody> GetRigidbody();

	shared_ptr<Transform> GetOrAddTransform();
	void AddComponent(shared_ptr<Component> component);
	void RemoveFixedComponent(ComponentType removeType);

	template<typename T>
	shared_ptr<T> GetComponent();

	template<typename T>
	shared_ptr<T> GetMono();

	void SetRenderIndex(ComponentType index) { _renderIndex = index; }
	void Render();

public:
	void SetScene(shared_ptr<Scene> scene) { _scene = scene; }
	shared_ptr<Scene> GetScene() { return _scene.lock(); }

	uint32 GetId() { return _id; }
	void SetId(uint32 id) { _id = id; }

	bool HasCircDbNode() { return _node != nullptr; }
	CircDbNode* GetCircDbNode() { return _node; }
	void SetCircDbNode(CircDbNode* node);
	void RemoveCircDbNode();

public:
	void SetName(string name) { _name = name; }
	string GetName() { return _name; }
	void SetPrefabName(string name) { _prefabName = name; }
	string GetPrefabName() { return _prefabName; }
	void SetActive(bool isSettingActive);

	void SetLayermask(Layermask layermask) { _layermask = layermask; }
	Layermask GetLayermask() { return _layermask; }

public:
	void WriteXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent);
	shared_ptr<GameObject> LoadXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent,
		vector<shared_ptr<GameObject>>& loadedObjects);


	shared_ptr<GameObject> Clone(shared_ptr<Scene> scene, vector<shared_ptr<GameObject>>& reserveGameObjects);

protected:
	std::array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;
	vector<shared_ptr<Monobehaviour>> _scripts;
	ComponentType _renderIndex;

public:
	bool isEnabled = true;

protected:
	weak_ptr<Scene> _scene;
	uint32 _id;
	string _name = "";
	CircDbNode* _node = nullptr;
	string _prefabName = "";
	Layermask _layermask = Layermask::None;
};




template<typename T>
inline shared_ptr<T> GameObject::GetComponent()
{
	for (shared_ptr<Monobehaviour>& mono : _scripts)
	{
		if (shared_ptr<T> c = dynamic_pointer_cast<T>(mono))
		{
			return c;
		}
	}

	for (shared_ptr<Component>& comp : _components)
	{
		if (shared_ptr<T> c = dynamic_pointer_cast<T>(comp))
		{
			return c;
		}
	}

	return nullptr;
}

template<typename T>
inline shared_ptr<T> GameObject::GetMono()
{
	for (shared_ptr<Monobehaviour>& mono : _scripts)
	{
		if (shared_ptr<T> c = dynamic_pointer_cast<T>(mono))
		{
			return c;
		}
	}
	
	return nullptr;
}
