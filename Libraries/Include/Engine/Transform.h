#pragma once
#include "Component.h"

class BaseCollider;

/*-------------------------------------------
		MyMath Library 사용 대체 코드
-------------------------------------------*/

class Transform : public Component
{
	using Super = Component;
public:
	// 생성자
	Transform();
	Transform(const TransformBase& InLocalTransformBase);

	virtual void Awake() override;

public:
	// 로컬 트랜스폼 관련 함수
	TransformBase& GetLocalTransform() { return _localTransformBase; }
	const TransformBase& GetLocalTransform() const { return _localTransformBase; }
	void SetLocalTransform(const TransformBase& InTransformBase);


	myVec3 GetLocalPosition() const { return _localTransformBase.GetPosition(); }
	void SetLocalPosition(const myVec3& InPosition);
	void AddLocalPosition(const myVec3& InDeltaPosition);


	myMatrix3x3 GetLocalRotationMatrix();
	myQuaternion GetLocalRotation() { _localTransformBase.GetRotation(); }
	myVec3 GetLocalEulerAngles() { return _localTransformBase.GetEulerAngles().ToVec3(); }
	myVec3 GetLocalXAxis() const { return _localTransformBase.GetXAxis(); }
	myVec3 GetLocalYAxis() const { return _localTransformBase.GetYAxis(); }
	myVec3 GetLocalZAxis() const { return _localTransformBase.GetZAxis(); }

	void SetLocalRotation(const myVec3& InEulerAngles);
	void SetLocalRotation(const myQuaternion& InQuaternion);
	void SetLocalRotation(const myMatrix3x3& InMatrix);

	void AddLocalYawRotation(float InDegree);
	void AddLocalPitchRotation(float InDegree);
	void AddLocalRollRotation(float InDegree);

	void AddLocalUnitXRotation(float InDegree);
	void AddLocalUnitYRotation(float InDegree);
	void AddLocalUnitZRotation(float InDegree);

	myVec3 GetLocalScale() const { return _localTransformBase.GetScale(); }
	void SetLocalScale(const myVec3& InScale);



	myMatrix4x4 GetLocalSRT() const { return _localTransformBase.GetSRT(); }


public:
	// 월드 트랜스폼 관련 함수
	TransformBase& GetTransform() { return _worldTransformBase; }
	const TransformBase& GetTransform() const { return _worldTransformBase; }
	void SetTransform(const TransformBase& InTransform);

	myVec3 GetPosition() const { return _worldTransformBase.GetPosition(); }
	void SetPosition(const myVec3& InPosition);
	void AddPosition(const myVec3& InDeltaPosition);


	myMatrix3x3 GetRotationMatrix();
	myQuaternion GetRotation() { return _worldTransformBase.GetRotation(); }
	myVec3 GetEulerAngles() { return _worldTransformBase.GetEulerAngles().ToVec3(); }
	myVec3 GetXAxis() const { return _worldTransformBase.GetXAxis(); }
	myVec3 GetYAxis() const { return _worldTransformBase.GetYAxis(); }
	myVec3 GetZAxis() const { return _worldTransformBase.GetZAxis(); }


	myVec3 Forward() const { return -_worldTransformBase.GetZAxis(); } // ※ -ZForward
	myVec3 Right() const { return -_worldTransformBase.GetXAxis(); } // ※ -ZForward
	myVec3 Up() const { return _worldTransformBase.GetYAxis(); }

	void SetRotation(const myVec3& InEulerAngles);
	void SetRotation(const myQuaternion& InQuaternion);
	void SetRotation(const myMatrix3x3& InMatrix);


	void AddYawRotation(float InDegree);
	void AddPitchRotation(float InDegree);
	void AddRollRotation(float InDegree);

	void AddUnitXRotation(float InDegree);
	void AddUnitYRotation(float InDegree);
	void AddUnitZRotation(float InDegree);

	myVec3 GetScale() const { return _worldTransformBase.GetScale(); }
	void SetScale(const myVec3& InScale);


	myMatrix4x4 GetSRT() const { return _worldTransformBase.GetSRT(); }






public:
	// 계층 구조 관련 함수
	void SetRoot() { SetRoot(false); }
private:
	void SetRoot(bool isSwitchingParent);
public:
	Transform& GetRoot();
	void SetParent(Transform& InTransform);

	bool HasParent() const { return _parentPtr != nullptr; }
	vector<Transform*>& GetChildren() { return _childPtr; }
	vector<Transform*>::const_iterator ChildBegin() const { return _childPtr.begin(); }
	vector<Transform*>::const_iterator ChildEnd() const { return _childPtr.end(); }
	Transform* GetParentPtr() const { return _parentPtr; }

private:
	// 내부에서만 호출하는 함수
	void UpdateLocal();
	void UpdateWorld();
	void UpdateChildrenWorld();

public:
	// Collider
	void InitializeCollider(shared_ptr<BaseCollider> baseCollider);
	void RemoveCollider();

public:
	virtual void LoadXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent) override;
	virtual void WriteXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent) override;
	virtual shared_ptr<Component> Clone() const override;

private:
	// 멤버 함수
	TransformBase _localTransformBase;
	TransformBase _worldTransformBase;

	Transform* _parentPtr = nullptr;
	vector<Transform*> _childPtr;
	weak_ptr<BaseCollider> collider;
};
