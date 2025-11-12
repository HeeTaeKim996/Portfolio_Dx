#pragma once
#include "Component.h"

class Shader;

class Renderer : public Component
{
public:
	Renderer(ComponentType type, shared_ptr<Shader> shader) : Component(type), _shader(shader) {}

	virtual void Render() abstract;

public:
	virtual shared_ptr<Component> Clone() const abstract;

protected:
	shared_ptr<Shader> _shader;
};

