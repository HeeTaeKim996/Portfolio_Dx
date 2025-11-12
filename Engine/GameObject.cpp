#pragma once
#include "pch.h"
#include "GameObject.h"
#include "Monobehaviour.h"
#include "Transform.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "ModelRenderer.h"
#include "ModelAnimator.h"
#include "Renderer.h"
#include "Monobehaviour.h"
#include "Scene.h"
#include "tinyxml2.h"
#include "Component.h"
#include "IReturnMonobehaviour.h"
#include "BaseCollider.h"
#include "Rigidbody.h"

GameObject::GameObject()
{

}

GameObject::~GameObject()
{
}

void GameObject::Awake()
{
	if (!isEnabled)
		return;

	for (shared_ptr<Component>& component : _components)
	{
		if (component)
		{
			component->Awake();
		}
	}
	for (shared_ptr<Monobehaviour> script : _scripts)
	{
		script->Awake();
	}
}

void GameObject::Start()
{
	if (!isEnabled)
		return;

	for (shared_ptr<Component>& component : _components)
	{
		if (component) 
		{
			component->Start();
		}
	}
	for (shared_ptr<Monobehaviour> script : _scripts)
	{
		script->Start();
	}
}

void GameObject::Update()
{
	if (!isEnabled)
		return;

	_renderIndex = ComponentType::End;

	for (shared_ptr<Component>& component : _components)
	{
		if (component && component->IsEnabled())
		{
			component->Update();
		}
	}
	for (shared_ptr<Monobehaviour> script : _scripts)
	{
		if (script->IsEnabled())
		{
			script->Update();
		}
	}

}

void GameObject::LateUpdate()
{
	if (!isEnabled)
		return;

	for (shared_ptr<Component>& component : _components)
	{
		if (component && component->IsEnabled())
		{
			component->LateUpdate();
		}
	}
	for (shared_ptr<Monobehaviour> script : _scripts)
	{
		if (script->IsEnabled())
		{
			script->LateUpdate();
		}
	}
}

void GameObject::FixedUpdate()
{
	if (!isEnabled)
		return;

	for (shared_ptr<Component>& component : _components)
	{
		if (component && component->IsEnabled())
		{
			component->FixedUpdate();
		}
	}
	for (shared_ptr<Monobehaviour> script : _scripts)
	{
		if (script->IsEnabled())
		{
			script->FixedUpdate();
		}
	}
}

void GameObject::EditRender()
{
	if (!isEnabled)
		return;

	ComponentType componentType = ComponentType::End;

	if (_components[(int)ComponentType::Animator])
	{
		componentType = ComponentType::Animator;
	}
	else if (_components[(int)ComponentType::ModelRenderer])
	{
		componentType = ComponentType::ModelRenderer;
	}
	else if (_components[(int)ComponentType::MeshRenderer])
	{
		componentType = ComponentType::MeshRenderer;
	}

	if (componentType == ComponentType::End)
		return;


	shared_ptr<Renderer> renderer = static_pointer_cast<Renderer>(_components[(int)componentType]);
	renderer->Render();
}

void GameObject::OnEnable()
{

	for (shared_ptr<Component>& component : _components)
	{
		if (component && !component->IsEnabled())
		{
			component->Enable();
		}
	}
	for (shared_ptr<Monobehaviour> script : _scripts)
	{
		if (!script->IsEnabled())
		{
			script->Enable();
		}
	}
}

void GameObject::OnDisable()
{
	for (shared_ptr<Component>& component : _components)
	{
		if (component && component->IsEnabled())
		{
			component->Disable();
		}
	}
	for (shared_ptr<Monobehaviour> script : _scripts)
	{
		if (script->IsEnabled())
		{
			script->Disable();
		}
	}
}

void GameObject::Destroy()
{
	if (isEnabled)
	{

		for (shared_ptr<Component>& component : _components)
		{
			if (component && component->IsEnabled())
			{
				component->Disable();
			}
		}
		for (shared_ptr<Monobehaviour> script : _scripts)
		{
			if (script->IsEnabled())
			{
				script->Disable();
			}
		}
	}

	for (shared_ptr<Component>& component : _components)
	{
		if (component)
		{
			component->OnDestroy();
		}
	}
	for (shared_ptr<Monobehaviour> script : _scripts)
	{
		script->OnDestroy();
	}
}

void GameObject::OnCollisionEnter(const Collision& collision)
{
	if (shared_ptr<Rigidbody> rigidbody = GetRigidbody())
	{
		rigidbody->OnCollisionEnter(collision);
	}

	for (shared_ptr<Monobehaviour>& mono : _scripts)
	{
		mono->OnCollisionEnter(collision);
	}
}

void GameObject::OnCollisionStay(const Collision& collision)
{
	if (shared_ptr<Rigidbody> rigidbody = GetRigidbody())
	{
		rigidbody->OnCollisionStay(collision);
	}

	for (shared_ptr<Monobehaviour>& mono : _scripts)
	{
		mono->OnCollisionStay(collision);
	}
}

void GameObject::OnCollisionExit(const Collision& collision)
{
	if (shared_ptr<Rigidbody> rigidbody = GetRigidbody())
	{
		rigidbody->OnCollisionExit(collision);
	}

	for (shared_ptr<Monobehaviour>& mono : _scripts)
	{
		mono->OnCollisionExit(collision);
	}
}

shared_ptr<Component> GameObject::GetFixedComponent(ComponentType type)
{
	uint8 index = static_cast<uint8>(type);
	assert(index < FIXED_COMPONENT_COUNT);
	
	return _components[index];
}

shared_ptr<Transform> GameObject::GetTransform()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::Transform);
	return static_pointer_cast<Transform>(component);
}

shared_ptr<Camera> GameObject::GetCamera()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::Camera);
	return static_pointer_cast<Camera>(component);
}

shared_ptr<MeshRenderer> GameObject::GetMeshRenderer()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::MeshRenderer);
	return static_pointer_cast<MeshRenderer>(component);
}

shared_ptr<ModelRenderer> GameObject::GetModelRenderer()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::ModelRenderer);
	return static_pointer_cast<ModelRenderer>(component);
}

shared_ptr<ModelAnimator> GameObject::Animator()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::Animator);
	return static_pointer_cast<ModelAnimator>(component);
}

shared_ptr<BaseCollider> GameObject::GetCollider()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::Collider);
	return static_pointer_cast<BaseCollider>(component);
}

shared_ptr<Rigidbody> GameObject::GetRigidbody()
{
	shared_ptr<Component> component = GetFixedComponent(ComponentType::Rigidbody);
	return static_pointer_cast<Rigidbody>(component);
}


shared_ptr<Transform> GameObject::GetOrAddTransform()
{
	if (GetTransform() == nullptr)
	{
		shared_ptr<Transform> transform = make_shared<Transform>();
		AddComponent(transform);
	}

	return GetTransform();
}

void GameObject::AddComponent(shared_ptr<Component> component)
{
	component->SetGameObject(shared_from_this());

	uint8 index = static_cast<uint8>(component->GetType());
	if (index < FIXED_COMPONENT_COUNT)
	{
		_components[index] = component;
	}
	else
	{
		_scripts.push_back(static_pointer_cast<Monobehaviour>(component));
	}
}

void GameObject::RemoveFixedComponent(ComponentType removeType)
{
	assert((int)removeType <= (int)ComponentType::End);

	_components[(int)removeType] = nullptr;
}

void GameObject::Render()
{
	if (!isEnabled)
		return;

	if (_renderIndex == ComponentType::End)
		return;

	shared_ptr<Component> component = GetFixedComponent(_renderIndex);

#if _DEBUG
	if (!component)
		assert(0);
#endif
	shared_ptr<Renderer> renderer = static_pointer_cast<Renderer>(component);
	renderer->Render();
}

void GameObject::SetCircDbNode(CircDbNode* node)
{
#ifdef _DEBUG
	assert(_node == nullptr);
#endif

	_node = node;
}

void GameObject::RemoveCircDbNode()
{
#ifdef _DEBUG
	assert(_node == nullptr);
#endif

	_node = nullptr;
}


void GameObject::SetActive(bool isSettingActive)
{
	if (isSettingActive == isEnabled)
		return;

	isEnabled = isSettingActive;


	if (isSettingActive)
	{
		OnEnable();
	}
	else
	{
		OnDisable();
	}
}


void GameObject::WriteXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent)
{

	tinyxml2::XMLElement* infoElement = doc.NewElement("ObjectInfo");
	parent->LinkEndChild(infoElement);
	{
		infoElement->SetAttribute("Name", _name.c_str());
	}


	tinyxml2::XMLElement* fixedComponent = doc.NewElement("FixedComponents");
	parent->LinkEndChild(fixedComponent);
	for (shared_ptr<Component> component : _components)
	{
		if (component)
		{
			component->WriteXML(doc, fixedComponent);
		}
	}


	tinyxml2::XMLElement* monobehaviours = doc.NewElement("Monobehaviours");
	parent->LinkEndChild(monobehaviours);
	for (shared_ptr<Monobehaviour> monobehaviour : _scripts)
	{

		XmlElement* monobehaviourElement = doc.NewElement("Monobehaviour");
		monobehaviours->LinkEndChild(monobehaviourElement);
		XmlElement* monoClassElement = doc.NewElement("ClassName");
		monobehaviourElement->LinkEndChild(monoClassElement);

		monobehaviour->WriteXML(doc, monoClassElement);
	}


	tinyxml2::XMLElement* childrenObjectsElement = doc.NewElement("ChildrenObjects");
	parent->LinkEndChild(childrenObjectsElement);

	shared_ptr<Transform> transform = GetTransform();
	if (transform)
	{
		vector<Transform*> childrenTransforms = transform->GetChildren();
		for (Transform* childTransform : childrenTransforms)
		{
			shared_ptr<GameObject> gameObject = childTransform->GetGameObject();
			XmlElement* childObjectElement = doc.NewElement("GameObject");
			childrenObjectsElement->LinkEndChild(childObjectElement);

			gameObject->WriteXML(doc, childObjectElement);
		}
	}
}

shared_ptr<GameObject> GameObject::LoadXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent, 
	vector<shared_ptr<GameObject>>& loadedObjects)
{
	XmlElement* infoElement = parent->FirstChildElement("ObjectInfo");
	_name = infoElement->Attribute("Name");

	tinyxml2::XMLElement* fixedComponentsElement = parent->FirstChildElement("FixedComponents");
	XmlElement* fixedElement = fixedComponentsElement->FirstChildElement("Transform");
	if (fixedElement)
	{
		shared_ptr<Transform> transform = make_shared<Transform>();
		transform->LoadXML(doc, fixedElement);

		AddComponent(transform);
	}

	fixedElement = fixedComponentsElement->FirstChildElement("MeshRenderer");
	if (fixedElement)
	{
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>(GAME->GetShader());
		meshRenderer->LoadXML(doc, fixedElement);

		AddComponent(meshRenderer);
	}
	

	fixedElement = fixedComponentsElement->FirstChildElement("Camera");
	if (fixedElement)
	{
		shared_ptr<Camera> camera = make_shared<Camera>();
		camera->LoadXML(doc, fixedElement);

		AddComponent(camera);
	}


	fixedElement = fixedComponentsElement->FirstChildElement("ModelAnimator");
	if (fixedElement)
	{
		shared_ptr<ModelAnimator> modelAnimator = make_shared<ModelAnimator>(GAME->GetShader());
		modelAnimator->LoadXML(doc, fixedElement);

		AddComponent(modelAnimator);
	}


	fixedElement = fixedComponentsElement->FirstChildElement("ModelRenderer");
	if (fixedElement)
	{
		shared_ptr<ModelRenderer> modelRenderer = make_shared<ModelRenderer>(GAME->GetShader());
		modelRenderer->LoadXML(doc, fixedElement);

		AddComponent(modelRenderer);
	}


	
	XmlElement* monobehavioursElement = parent->FirstChildElement("Monobehaviours");
	XmlElement* monobehaviourElement = monobehavioursElement->FirstChildElement("Monobehaviour");
	while (monobehaviourElement)
	{
		XmlElement* classNameElement = monobehaviourElement->FirstChildElement("ClassName");
		shared_ptr<Monobehaviour> monobehaviour 
			= GAME->GetGameDesc().MonobehaviourLoader->ReturnMonobehaviour(doc, classNameElement);

		AddComponent(monobehaviour);

		monobehaviourElement = monobehaviourElement->NextSiblingElement("Monobehaviour");
	}


	loadedObjects.push_back(shared_from_this());


	XmlElement* childrenElement = parent->FirstChildElement("ChildrenObjects");
	XmlElement* childElement = childrenElement->FirstChildElement("GameObject");
	while (childElement)
	{
		shared_ptr<GameObject> childObject = make_shared<GameObject>();
		childObject->LoadXML(doc, childElement, loadedObjects);

		childObject->GetTransform()->SetParent(*GetTransform().get());


		childElement = childElement->NextSiblingElement("GameObject");
	}


	return shared_from_this();
}


shared_ptr<GameObject> GameObject::Clone(shared_ptr<Scene> scene, vector<shared_ptr<GameObject>>& reserveGameObjects)
{
	shared_ptr<GameObject> copy = make_shared<GameObject>();

	copy->_scene = scene;
	copy->_name = _name;
	copy->_layermask = _layermask;

	copy->_scripts.resize(_scripts.size());

	int index = 0;
	for (shared_ptr<Component> component : _components)
	{
		if (component)
		{
			shared_ptr<Component> copiedComponent = component->Clone();
			copiedComponent->SetGameObject(copy);
			copy->_components[index] = copiedComponent;
			copiedComponent->SET_IS_ENABLED_FALSE_ONLY_USED_IN_GAME_OBJECT_CLONING();
		}
		index++;
	}

	index = 0;
	for (shared_ptr<Monobehaviour> script : _scripts)
	{
		if (script)
		{
			shared_ptr<Monobehaviour> copiedScript = static_pointer_cast<Monobehaviour>(script->Clone());
			copiedScript->SetGameObject(copy);
			copy->_scripts[index] = copiedScript;
		}
		index++;
	}


	copy->_renderIndex = _renderIndex;


	reserveGameObjects.push_back(copy);


	shared_ptr<Transform> transform = GetTransform();
	if (transform)
	{
		shared_ptr<Transform> copyTransform = copy->GetTransform();



		Transform* parentTransform = transform->GetParentPtr();
		if (parentTransform)
		{
			copyTransform->SetParent(*transform->GetParentPtr());
		}


		vector<Transform*> childrenTransform = transform->GetChildren();
		for (Transform* childTransform : childrenTransform)
		{
			shared_ptr<GameObject> copyChildObject
				= childTransform->GetGameObject()->Clone(scene, reserveGameObjects);
			shared_ptr<Transform> copyChildTransform
				= copyChildObject->GetTransform();
			copyChildTransform->SetParent(*copyTransform.get());
		}
	}

	return copy;
}