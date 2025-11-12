#pragma once
#include "Engine/Monobehaviour.h"
#include "AnimationHelper.h"

class ModelAnimator;
class CameraScript;

class PlayerMovement : public Monobehaviour, public AnimationHelper
{
	enum Animation // ResourceIntializer2 ÂüÁ¶
	{
		Walk = 0,
		Run = 1,
		Idle = 2,
		AimingForward = 3,
		AimingBackward = 4,
		AimingRight = 5,
		AimingLeft = 6,
	};

public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

	virtual void OnCollisionEnter(const Collision& collision) override;

public:
	void InvokeMove(int vertical, int horizon, float interpolSpeed = 1.f);
	void InvokeMove(Vec2 screenVec, float interpoalSpeed = 1.f);
	void InformNoMove();
	void InvokeJump();
	void OnLockButtonClicked();


private:
	Vec3 DrawVirtualPad(const ImVec2& center, float radius);


public:
	virtual shared_ptr<Component> Clone() const override;


private:
	bool isMoveInvoked = false;
	Vec3 _movingVec;
	Vec3 _movingSlerpVec = Vec3::UnitZ;

	float _movementSpeed = 25.f;

	weak_ptr<Transform> _cameraTransform;
	weak_ptr<Rigidbody> _rigidbody;
	weak_ptr<CameraScript> _cameraScript;

	bool _isLockOn = false;
	weak_ptr<Transform> _lockTransform;
};

