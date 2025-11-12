#pragma once
#include "Renderer.h"

class Mesh;
class Material;
class Shader;

class MeshRenderer : public Renderer
{
	using Super = Renderer;
public:
	MeshRenderer(shared_ptr<Shader> shader);
	virtual ~MeshRenderer();

	virtual void Update() override;
	virtual void Render() override;

	void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }
	void SetMaterial(shared_ptr<Material> material) { _material = material; }



	// @@ Obsolate @@
	void SetTexture(shared_ptr<Texture> texture) { }
	void SetShader(shared_ptr<Shader> shader) {  }

public:
	enum class Pass
	{
		RED = 1,
		BLUE = 2,
		GREEN = 3,
		WHITE = 4,
		SKY = 5,
		WIREFRAME,
	};
	void SetPass(Pass pass) { _pass = static_cast<uint8>(pass); }


public:
	virtual void LoadXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent) override;
	virtual void WriteXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parent) override;
	virtual shared_ptr<Component> Clone() const override;

private:
	uint8 _pass = 0;
	shared_ptr<Mesh> _mesh;
	shared_ptr<Material> _material;


};

