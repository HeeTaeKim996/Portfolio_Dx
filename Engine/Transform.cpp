#include "pch.h"
#include "Transform.h"
#include "Scene.h"
#include "GameObject.h"
#include "tinyxml2.h"
#include "BaseCollider.h"


/*-------------------------------------------
		MyMath Library 사용 대체 코드
-------------------------------------------*/



/*--------------
	  생성자
---------------*/
Transform::Transform() : Super(ComponentType::Transform)
{
}

Transform::Transform(const TransformBase& InLocalTransformBase)
	: Super(ComponentType::Transform), _localTransformBase(InLocalTransformBase)
{
	UpdateWorld();
}

void Transform::Awake()
{
}






/*----------------------------
	 로컬 트래스폼 관련 함수
-----------------------------*/
void Transform::SetLocalTransform(const TransformBase& InTransformBase)
{
	_localTransformBase = InTransformBase;
	UpdateWorld();
}




void Transform::SetLocalPosition(const myVec3& InPosition)
{
	_localTransformBase.SetPosition(InPosition);
	UpdateWorld();
}

void Transform::AddLocalPosition(const myVec3& InDeltaPosition)
{
	_localTransformBase.AddPosition(InDeltaPosition);
	UpdateWorld();
}





myMatrix3x3 Transform::GetLocalRotationMatrix()
{
	return _localTransformBase.GetRotation().ToRotateMatrix();
}

void Transform::SetLocalRotation(const myVec3& InEulerAngles)
{
	_localTransformBase.SetRotation(myEulerAngles(InEulerAngles.X, InEulerAngles.Y, InEulerAngles.Z));
	UpdateWorld();
}

void Transform::SetLocalRotation(const myQuaternion& InQuaternion)
{
	_localTransformBase.SetRotation(InQuaternion);
	UpdateWorld();
}

void Transform::SetLocalRotation(const myMatrix3x3& InMatrix)
{
	_localTransformBase.SetRotation(InMatrix);
	UpdateWorld();
}

void Transform::AddLocalYawRotation(float InDegree)
{
	_localTransformBase.AddYawRotation(InDegree);
	UpdateWorld();
}

void Transform::AddLocalPitchRotation(float InDegree)
{
	_localTransformBase.AddPitchRotation(InDegree);
	UpdateWorld();
}

void Transform::AddLocalRollRotation(float InDegree)
{
	_localTransformBase.AddRollRotation(InDegree);
	UpdateWorld();
}


void Transform::AddLocalUnitXRotation(float InDegree)
{
	_localTransformBase.AddUnitXRotation(InDegree);
	UpdateWorld();
}

void Transform::AddLocalUnitYRotation(float InDegree)
{
	_localTransformBase.AddUnitYRotation(InDegree);
	UpdateWorld();
}

void Transform::AddLocalUnitZRotation(float InDegree)
{
	_localTransformBase.AddUnitZRotation(InDegree);
	UpdateWorld();
}









void Transform::SetLocalScale(const myVec3& InScale)
{
	_localTransformBase.SetScale(InScale);
	UpdateWorld();
}







void Transform::SetTransform(const TransformBase& InTransform)
{
	_worldTransformBase = InTransform;
	UpdateLocal();
	UpdateChildrenWorld();
}

void Transform::SetPosition(const myVec3& InPosition)
{
	_worldTransformBase.SetPosition(InPosition);
	UpdateLocal();
	UpdateChildrenWorld();
}

void Transform::AddPosition(const myVec3& InDeltaPosition)
{
	_worldTransformBase.AddPosition(InDeltaPosition);
	UpdateLocal();
	UpdateChildrenWorld();
}

myMatrix3x3 Transform::GetRotationMatrix()
{
	return _worldTransformBase.GetRotation().ToRotateMatrix();
}

void Transform::SetRotation(const myVec3& InEulerAngles)
{
	_worldTransformBase.SetRotation(myEulerAngles(InEulerAngles.X, InEulerAngles.Y, InEulerAngles.Z));
	UpdateLocal();
	UpdateChildrenWorld();
}

void Transform::SetRotation(const myQuaternion& InQuaternion)
{
	_worldTransformBase.SetRotation(InQuaternion);
	UpdateLocal();
	UpdateChildrenWorld();
}

void Transform::SetRotation(const myMatrix3x3& InMatrix)
{
	_worldTransformBase.SetRotation(InMatrix);
	UpdateLocal();
	UpdateChildrenWorld();
}

void Transform::AddYawRotation(float InDegree)
{
	_worldTransformBase.AddYawRotation(InDegree);
	UpdateLocal();
	UpdateChildrenWorld();
}

void Transform::AddPitchRotation(float InDegree)
{
	_worldTransformBase.AddPitchRotation(InDegree);
	UpdateLocal();
	UpdateChildrenWorld();
}

void Transform::AddRollRotation(float InDegree)
{
	_worldTransformBase.AddRollRotation(InDegree);
	UpdateLocal();
	UpdateChildrenWorld();
}

void Transform::AddUnitXRotation(float InDegree)
{
	_worldTransformBase.AddUnitXRotation(InDegree);
	UpdateLocal();
	UpdateChildrenWorld();
}

void Transform::AddUnitYRotation(float InDegree)
{
	_worldTransformBase.AddUnitYRotation(InDegree);
	UpdateLocal();
	UpdateChildrenWorld();
}

void Transform::AddUnitZRotation(float InDegree)
{
	_worldTransformBase.AddUnitZRotation(InDegree);
	UpdateLocal();
	UpdateChildrenWorld();
}

void Transform::SetScale(const myVec3& InScale)
{
	_worldTransformBase.SetScale(InScale);
	UpdateLocal();
	UpdateChildrenWorld();
}


void Transform::SetRoot(bool isSwitchingParent)
{
	if (!HasParent())
		return;


	Transform& parent = *GetParentPtr();
	auto it = std::find(parent.ChildBegin(), parent.ChildEnd(), this);

#ifdef _DEBUG
	assert(it != parent.ChildEnd());
#endif

	// 부모 트랜스폼에서 자식 정보를 제거
	parent.GetChildren().erase(it);

	// 자신에게서 부모 정보를 제거
	_parentPtr = nullptr;



	if (!isSwitchingParent)
	{
		shared_ptr<GameObject> gameObject = _gameObject.lock();
		shared_ptr<Scene> scene = gameObject->GetScene();
#ifdef _DEBUG
		assert(scene != nullptr);
#endif
		scene->AddRootObject(gameObject);
	}


	// 로컬 정보를 월드 정보로 변경한다
	UpdateLocal();

}


Transform& Transform::GetRoot()
{
	Transform* parent = nullptr;
	Transform* current = this;

	while ((parent = current->GetParentPtr()) != nullptr)
	{
		current = parent;
	}

	return *current;
}

void Transform::SetParent(Transform& parentTransform)
{
	if (HasParent())
	{
		// 현재 노드를 부모로부터 분리
		SetRoot(true);
	}
	else
	{
		// 기존 루트 오브젝트였다면, 씬에서 루트오브젝트 삭제
		shared_ptr<GameObject> gameObject = _gameObject.lock();
		shared_ptr<Scene> scene = gameObject->GetScene();
		if (scene)
		{
			scene->RemoveRootObject(gameObject);
		}
	}


#ifdef _DEBUG
	// 새로운 부모의 자식으로 등록. 이미 있는 경우에는 문제가 있는 상황.	
	auto it = std::find(parentTransform.ChildBegin(), parentTransform.ChildEnd(), this);
	assert(it == parentTransform.ChildEnd());
#endif


	// 새로운 트랜스폼 노드로 부모 재설정
	parentTransform.GetChildren().push_back(this);
	_parentPtr = &parentTransform;

	// 새로운 부모에 맞춰 자신의 로컬 정보를 업데이트
	UpdateLocal();
}

void Transform::UpdateLocal()
{
	if (HasParent())
	{
		const Transform& parent = *GetParentPtr();
		_localTransformBase = _worldTransformBase.WorldToLocal(parent.GetTransform());
	}
	else
	{
		_localTransformBase = _worldTransformBase;
	}
}

void Transform::UpdateWorld()
{
	// 자신의 월드 트랜스폼을 우선 업데이트
	if (HasParent())
	{
		const Transform& parent = *GetParentPtr();
		_worldTransformBase = _localTransformBase.LocalToWorld(parent.GetTransform());
	}
	else
	{
		_worldTransformBase = _localTransformBase;
	}

	// 월드 정보 변경시 자식의 월드 정보도 업데이트
	UpdateChildrenWorld();
}

void Transform::UpdateChildrenWorld()
{

	if (shared_ptr<BaseCollider> col = collider.lock())
	{
		col->UpdateColliderTransform(static_pointer_cast<Transform>(shared_from_this()));
	}



	for (auto it = ChildBegin(); it != ChildEnd(); it++)
	{
		(*it)->UpdateWorld();
	}
}

void Transform::InitializeCollider(shared_ptr<BaseCollider> baseCollider)
{
	collider = baseCollider;
	baseCollider->UpdateColliderTransform(static_pointer_cast<Transform>(shared_from_this()));
}

void Transform::RemoveCollider()
{
	collider.reset();
}




void Transform::LoadXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent)
{
	// ※ 부모 자식 관계는 여기가 아니라, GameObject에서 LoadXML 때 처리

	XmlElement* srtElement = parent->FirstChildElement("Scale");
	Vec3 scale;
	scale.X = srtElement->FloatAttribute("X");
	scale.Y = srtElement->FloatAttribute("Y");
	scale.Z = srtElement->FloatAttribute("Z");
	_worldTransformBase.SetScale(scale);


	srtElement = parent->FirstChildElement("Rotation");
	Quaternion quaternion;
	quaternion.X = srtElement->FloatAttribute("X");
	quaternion.Y = srtElement->FloatAttribute("Y");
	quaternion.Z = srtElement->FloatAttribute("Z");
	quaternion.W = srtElement->FloatAttribute("W");
	_worldTransformBase.SetRotation(quaternion);


	srtElement = parent->FirstChildElement("Translation");
	Vec3 pos;
	pos.X = srtElement->FloatAttribute("X");
	pos.Y = srtElement->FloatAttribute("Y");
	pos.Z = srtElement->FloatAttribute("Z");
	_worldTransformBase.SetPosition(pos);

	_localTransformBase = _worldTransformBase;
	// ※ 우선 local 은 월드를 오버라이드하고, 이후 GameObject.cpp 에서 SetParent로, 부모가 있다면 로컬 갱신	
}

void Transform::WriteXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent)
{
	tinyxml2::XMLElement* transform = doc.NewElement("Transform");
	parent->LinkEndChild(transform);



	tinyxml2::XMLElement* element = nullptr;
	{
		element = doc.NewElement("Scale");
		transform->LinkEndChild(element);
		Vec3 scale = _worldTransformBase.GetScale();

		element->SetAttribute("X", scale.X);
		element->SetAttribute("Y", scale.Y);
		element->SetAttribute("Z", scale.Z);
	}
	{
		element = doc.NewElement("Rotation");
		transform->LinkEndChild(element);
		Quaternion quaternion = _worldTransformBase.GetRotation();

		element->SetAttribute("X", quaternion.X);
		element->SetAttribute("Y", quaternion.Y);
		element->SetAttribute("Z", quaternion.Z);
		element->SetAttribute("W", quaternion.W);
	}
	{
		element = doc.NewElement("Translation");
		transform->LinkEndChild(element);
		Vec3 pos = _worldTransformBase.GetPosition();

		element->SetAttribute("X", pos.X);
		element->SetAttribute("Y", pos.Y);
		element->SetAttribute("Z", pos.Z);
	}

}

shared_ptr<Component> Transform::Clone() const
{
	shared_ptr<Transform> copy = make_shared<Transform>();
	// ※ 여기서는 vector의 vector<Transform*> _childPtr 의 size를 0으로 유지하기 위해 *this 를 인자로 넣지 않고, 수동 입력함

	copy->_localTransformBase = _localTransformBase;
	copy->_worldTransformBase = _worldTransformBase;


	// ※ _parentPtr, _childPtr은 여기서 Clone 되지 않고, GameObject 의 Clone의 마무리 작업에서 처리됨


	return copy;
}

