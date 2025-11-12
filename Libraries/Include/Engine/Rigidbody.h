#pragma once

#include "Component.h"

#define TERMINAL_FALL_SPEED -200.f
#define GRAVITY 180.f


class Rigidbody : public Component
{
	using Super = Component;
public:
	Rigidbody();
	~Rigidbody();
	
	virtual void OnEnable() override;
	virtual void OnDisable() override;

	virtual void Update() override;

	virtual void OnCollisionEnter(const Collision& collision) override;
	virtual void OnCollisionStay(const Collision& collision) override;
	virtual void OnCollisionExit(const Collision& collision) override;

public:
	void RegisterCollider(shared_ptr<BaseCollider> collider);
	void RemoveCollider();
	const bool& IsGrouneded() { return _isGrounded; }

public:
	void AddForce(myVec3 force) { velocity += force; }

private:
	bool CheckIsGrounded();


public:
	virtual shared_ptr<Component> Clone() const override;

public:
	Vec3 position;
	bool useGravity = true;
	Vec3 velocity;
	

private:
	bool _isGrounded = false;
	Vec3 _position;
	weak_ptr<BaseCollider> _collider;
};