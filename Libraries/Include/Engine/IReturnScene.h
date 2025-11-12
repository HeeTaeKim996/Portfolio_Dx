#pragma once

class Shader;
class Scene;

class IReturnScene
{
public:
	virtual shared_ptr<Scene> ReturnScene(shared_ptr<Shader> shader) =0;
};

