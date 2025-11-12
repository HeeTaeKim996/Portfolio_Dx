#pragma once
#include "Engine/Monobehaviour.h"

class PlayerMovement;
class Transform;

class CameraScript : public Monobehaviour
{
public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;


public:
	void RegisterLockOn(shared_ptr<Transform> transform);
	void RemoveLockOn();

private:
	myVec3 GetPivotPos(shared_ptr<Transform> transform);

public:
	virtual void LoadXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent) override;
	virtual void WriteXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent) override;
	virtual shared_ptr<Component> Clone() const override;

private:
	POINT _mousePos;
	weak_ptr<Transform> _playerTransform;
	Vec3 interPolVec = myVec3(0.f, 10.f, 0.f);
	Vec3 _posVec;
	float _distance = 70.f;
	weak_ptr<Transform> _lockTransform;
};

