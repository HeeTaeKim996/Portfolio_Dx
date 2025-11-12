#pragma once

class GameObject;
class IReturnMonobehaviour;
class FileUtils;

class PrefabManager
{
	enum
	{

		MonoDiscernCount = 12345678
	};
	


private:
	PrefabManager();
public:
	static PrefabManager* GetInstance()
	{
		static PrefabManager instance;
		return &instance;
	}


	/* Obsolate Start */
public:
	void ObsolateInitialize();
	void ObsolateSavePrefab();
	vector<shared_ptr<GameObject>> ObsolateGetPrefabs() { return ObsolatePrefabs; }
private:
	vector<shared_ptr<GameObject>> ObsolatePrefabs;
	/* Obsolate End */


public:
	void Initialize(unordered_map<string, vector<shared_ptr<GameObject>>> loadedPrefabs);
	shared_ptr<GameObject> GetPrefab(string key);
	vector<shared_ptr<GameObject>> GetAllPrefabs();

private:
	unordered_map<string, vector<shared_ptr<GameObject>>> _prefabs;
};