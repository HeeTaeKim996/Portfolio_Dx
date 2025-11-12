#pragma once

class Scene;
class Shader;

class IReturnScene
{
public:
	virtual vector<shared_ptr<Scene>> Init(shared_ptr<Shader> shader) abstract;
};

