#pragma once
#include "Engine/IReturnGameObjects.h"


class PrefabLoader : public IReturnGameObjects
{
public:
	virtual unordered_map<string, vector<shared_ptr<GameObject>>> ReturnGameObjects(shared_ptr<Shader> shader);

private:
	void Add(unordered_map<string, vector<shared_ptr<GameObject>>>& ret,vector<shared_ptr<GameObject>> gameObjects);
};

