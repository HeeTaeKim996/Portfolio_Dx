#pragma once
#include "Component.h"


enum class ProjectionType
{
	Perspective, // 원근
	Orthographic, // 직교
};

class Camera : public Component
{
	using Super = Component;
public:
	Camera();
	virtual ~Camera();

	virtual void Update() override;

	void UpdateMatrix();

	void SetNear(float value) { _near = value; }
	void SetFar(float value) { _far = value; }
	void SetFOV(float value) { _fov = value; }


	void SetWidth(float value) { _width = value; }
	void SetHeight(float value) { _height = value; }

	
	void SetViewMatrix();
	void SetProjectionMatrix();
	void SetVInvMatrix();

	float GetNear() { return _near; }
	float GetFar() { return _far; }
	float GetFOV() { return _fov; }
	float GetWidth() { return _width; }
	float GetHeight() { return _height; }

public:
	virtual void LoadXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent);
	virtual void WriteXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent);
	virtual shared_ptr<Component> Clone() const override;

private:
	float _near = 1.f;
	float _far = 1000.f;
	float _fov = XM_PI / 4.f;
	float _width = 0.f;
	float _height = 0.f;

public:
	static bool readyRender;
	static Matrix S_MatView;
	static Matrix S_MatProjection;
	static Matrix S_MatVInv;
};

