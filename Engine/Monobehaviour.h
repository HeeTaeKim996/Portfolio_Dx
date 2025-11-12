#pragma once
#include "Component.h"
class Monobehaviour : public Component
{
	using Super = Component;
public:
	Monobehaviour();
	~Monobehaviour();

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;


public:
	virtual shared_ptr<Component> Clone() const abstract;
};

