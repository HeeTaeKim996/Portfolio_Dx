#pragma once

#include "TimerActor.h"

class GameObject;
class Transform;
class FileUtils;
struct Collision;

enum class ComponentType : uint8
{
	Transform,
	MeshRenderer,
	ModelRenderer,
	Camera,
	Animator,
	Collider,
	Rigidbody,
	// ..
	Script,

	End,
};

enum
{
	FIXED_COMPONENT_COUNT = static_cast<uint8>(ComponentType::End) - 1,
};

class Component : public TimerActor
{
public:
	Component(ComponentType type);
	virtual ~Component();

	virtual void Awake() {}
	virtual void Start() {}
	virtual void Update() {}
	virtual void LateUpdate() {}
	virtual void FixedUpdate() {}
	
	virtual void OnEnable() {}
	virtual void OnDisable() {}
	virtual void OnDestroy() {}

	virtual void OnCollisionEnter(const Collision& collision) {}
	virtual void OnCollisionStay(const Collision& collision) {}
	virtual void OnCollisionExit(const Collision& collision) {}
	

public:
	ComponentType GetType() { return _type; }

	shared_ptr<GameObject> GetGameObject();
	shared_ptr<Transform> GetTransform();


	bool IsEnabled() { return _isEnabled; }
	void Disable() { _isEnabled = false; OnDisable(); }
	void Enable() { _isEnabled = true; OnEnable(); }

	void SET_IS_ENABLED_FALSE_ONLY_USED_IN_GAME_OBJECT_CLONING() { _isEnabled = false; }


public:
	virtual void LoadXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent) {}
	virtual void WriteXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent) {}
	virtual shared_ptr<Component> Clone() const = 0;


private:
	bool _isEnabled = false;
	friend class GameObject;
	void SetGameObject(shared_ptr<GameObject> gameObject) { _gameObject = gameObject; }


protected:
	ComponentType _type;
	weak_ptr<GameObject> _gameObject;

};

