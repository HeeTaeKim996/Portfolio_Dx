#pragma once

class Scene;

class MainExecute : public IExecute
{
public:
	void Init() override;
	void Update() override;


	shared_ptr<Shader> _shader;




	void Temp_GetScene(shared_ptr<Scene> scene) { _scene = scene; }


private:
	shared_ptr<Scene> _scene;
};