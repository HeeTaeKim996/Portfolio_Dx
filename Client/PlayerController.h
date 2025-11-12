#pragma once

#include "Engine/Monobehaviour.h"

class PlayerMovement;

class PlayerController : public Monobehaviour
{
public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;


	virtual void OnCollisionEnter(const Collision& collision) override;
	virtual void OnCollisionExit(const Collision& collision) override;

private:
	Vec2 DrawVirtualPad(const ImVec2& center, float radius);


public:
	virtual shared_ptr<Component> Clone() const override;

private:
	weak_ptr<PlayerMovement> _playerMovement;

};

