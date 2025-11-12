#pragma once
#include "tinyxml2.h"

#include "GameObject.h"

class Camera;
class BaseCollider;

class Scene : public enable_shared_from_this<Scene>
{
public:
	Scene();
	~Scene();


	void Awake();
	void Start();
	void Update();
	void LateUpdate();
	void FixedUpdate();
	void Render();

	void OnEnable();
	void OnDisable();

	void EditUpdate();
	void EditRender();

public:
	void AddGameObject(shared_ptr<GameObject> gameObject);
	void DestroyGameObject(shared_ptr<GameObject> gameObject);

	void AddRootObject(shared_ptr<GameObject> gameObject);
	void RemoveRootObject(shared_ptr<GameObject> gameObject);



	vector<shared_ptr<GameObject>>& GetGameObjects() { return _gameObjects; }
	
	vector<shared_ptr<GameObject>> GetRootObjects();

	shared_ptr<Scene> Clone();


	shared_ptr<GameObject> Instantiate(string prefabKey);
	shared_ptr<GameObject> Instantiate(string prefabKey, Vec3 position);

	template<typename T>
	shared_ptr<T> FindObjectOfMono();


	void SaveScene(XmlDocument& doc, XmlElement* parent);
	void LoadScene(XmlDocument& doc, XmlElement* parent);

	void SetSceneName(string name) { _sceneName = name; }
	string GetSceneName() { return _sceneName; }


	shared_ptr<Camera> GetCamera(); 
	shared_ptr<GameObject> RayPick(int32 screenX, int32 screenY);
	vector<shared_ptr<BaseCollider>> GetColliders();

private:
	string _sceneName;

	vector<shared_ptr<GameObject>> _gameObjects;
	vector<uint32> _freeIndexList;
	uint32 _gameObjectAddingIndex = 0;
	shared_ptr<CircDbLinkedList> _circDbLinkedList;

public: // юс╫ц Public
	shared_ptr<GameObject> _skyObject;
};

template<typename T>
inline shared_ptr<T> Scene::FindObjectOfMono()
{
	for (shared_ptr<GameObject> gameObject : _gameObjects)
	{
		if (shared_ptr<T> t = gameObject->GetMono<T>())
		{
			return t;
		}
	}

	return nullptr;
}
