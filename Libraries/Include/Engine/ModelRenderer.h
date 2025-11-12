#pragma once
#include "Renderer.h"

class Model;
class Shader;
class Material;

class ModelRenderer : public Renderer
{
	using Super = Renderer;
public:
	ModelRenderer(shared_ptr<Shader> shader);
	virtual ~ModelRenderer();

	virtual void Update() override;
	virtual void Render() override;

	void SetModel(shared_ptr<Model> model);
	void SetPass(uint8 pass) { _pass = pass; }
	

public:
	virtual void LoadXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent) override;
	virtual void WriteXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent) override;
	virtual shared_ptr<Component> Clone() const override;

private:
	uint8 _pass = 0;
	shared_ptr<Model> _model;

};

