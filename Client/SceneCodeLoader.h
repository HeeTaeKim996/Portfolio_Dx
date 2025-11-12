#pragma once
#include "Engine/InitResource.h"

class SceneCodeLoader : public IReturnScene
{
public:
	virtual vector<shared_ptr<Scene>> Init(shared_ptr<Shader> shader);
};

