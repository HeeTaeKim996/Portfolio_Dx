#pragma once

class Shader;
class GameObject;

class IReturnGameObjects
{
public:
	virtual unordered_map<string, vector<shared_ptr<GameObject>>> ReturnGameObjects(shared_ptr<Shader> shader) =0;
};

