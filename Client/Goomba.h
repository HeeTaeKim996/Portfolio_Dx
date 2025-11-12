#pragma once
#include "Engine/Monobehaviour.h"
#include "AnimationHelper.h"

class PlayerMovement;

class Goomba : public Monobehaviour, public AnimationHelper
{
public:
	enum Animation
	{
		Idle = 0,
		Walk = 1,
	};



public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;



public:
	void Die();
	void Destry();

private:
	void StartDetectPlayer();
	void DetectPlayer();
	

public:
	shared_ptr<Component> Clone() const override;
	
private:
	bool isLayderOn = false;
	weak_ptr<PlayerMovement> _player;
	bool _dead = false;

};

