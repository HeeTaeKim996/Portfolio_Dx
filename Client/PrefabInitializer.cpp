#include "pch.h"
#include "PrefabInitializer.h"

#include "Engine/GeometryHelper.h"
#include "Engine/Camera.h"
#include "Engine/GameObject.h"
#include "CameraScript.h"
#include "Engine/MeshRenderer.h"
#include "Engine/Mesh.h"
#include "Engine/Material.h"
#include "Engine/Model.h"
#include "Engine/ModelRenderer.h"
#include "Engine/ResourceManager.h"
#include "Engine/RenderManager.h"
#include "Engine/ModelAnimator.h"
#include "Engine/Scene.h"
#include "Kachujin.h"
#include "TempRotator.h"
#include "PlayerMovement.h"
#include "PlayerController.h"


#include "Engine/SphereCollider.h"
#include "Engine/BoxCollider.h"
#include "Engine/CapsuleCollider.h"
#include "Engine/Rigidbody.h"
#include "Goomba.h"

unordered_map<string, vector<shared_ptr<GameObject>>> PrefabLoader::ReturnGameObjects(
	shared_ptr<Shader> shader)
{
	unordered_map<string, vector<shared_ptr<GameObject>>> ret;


	{
		vector<shared_ptr<GameObject>> gameObjects;

		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->GetOrAddTransform()->SetPosition(Vec3(0.f, 0.f, -44.4444f));
		camera->AddComponent(make_shared<Camera>());
		camera->AddComponent(make_shared<CameraScript>());
		camera->SetName("Camera");

		gameObjects.push_back(camera);

		Add(ret, gameObjects);
	}

	{
		vector<shared_ptr<GameObject>> gameObjects;

		shared_ptr<class Model> m1 = RESOURCES->Get<Model>(L"Kachujin");

		shared_ptr<GameObject> kachujin = make_shared<GameObject>();
		kachujin->GetOrAddTransform()->SetPosition(Vec3(0, 0, 20));
		float scale = 0.1f;
		kachujin->GetOrAddTransform()->SetScale(Vec3(scale, scale, scale));


		kachujin->AddComponent(make_shared<ModelRenderer>(shader));
		{
			kachujin->GetModelRenderer()->SetModel(m1);
			kachujin->GetModelRenderer()->SetPass(0);
		}


#if 1
		shared_ptr<SphereCollider> collider = make_shared<SphereCollider>(shader);
		collider->SetRadius(80.f);
		collider->SetOffset(Vec3(0.f, 8.f, 0.f));
		collider->SetRigiMass(200);

		kachujin->AddComponent(collider);
#elif 0
		shared_ptr<BoxCollider> collider = make_shared<BoxCollider>(shader);
		float halfExtent = 80.f;
		collider->SetHalfExtents(Vec3(halfExtent, halfExtent, halfExtent));
		collider->SetOffset(Vec3(0.f, 8.f, 0.f));
		collider->SetRigiMass(200);

		kachujin->AddComponent(collider);
#else
		shared_ptr<CapsuleCollider> collider = make_shared<CapsuleCollider>(shader);
		collider->SetOffset(Vec3(0.f, 10.f, 0.f));
		collider->SetRadius(30.f);
		collider->SetHalfHeight(70.f);
		kachujin->AddComponent(collider);

#endif

		kachujin->AddComponent(make_shared<Rigidbody>());


		kachujin->AddComponent(make_shared<Kachujin>());


		kachujin->SetLayermask(Layermask::Enemy);

		kachujin->SetName("Kachujin_OnlyModel");

		gameObjects.push_back(kachujin);

		Add(ret, gameObjects);
	}


	{
#if 0
		vector<shared_ptr<GameObject>> gameObjects;

		shared_ptr<class Model> m1 = RESOURCES->Get<Model>(L"Kachujin");

		shared_ptr<GameObject> kachujin = make_shared<GameObject>();
		kachujin->GetOrAddTransform()->SetPosition(Vec3(-20, 0, 20));
		float scale = 0.1f;
		kachujin->GetOrAddTransform()->SetScale(Vec3(scale, scale, scale));


		kachujin->AddComponent(make_shared<ModelAnimator>(shader));
		{
			kachujin->GetModelAnimator()->SetModel(m1);
			kachujin->GetModelAnimator()->SetPass(0);
		}


		kachujin->AddComponent(make_shared<Kachujin>());

		kachujin->SetName("Kachujin_Animation");

		gameObjects.push_back(kachujin);

		Add(ret, gameObjects);
#endif
	}

	{
#if 1
		vector<shared_ptr<GameObject>> gameObjects;

		shared_ptr<GameObject> robot = make_shared<GameObject>();
		robot->GetOrAddTransform();
		float scale = 0.1f;
		robot->GetTransform()->SetScale(Vec3(scale, scale, scale));


		shared_ptr<ModelAnimator> modelRenderer = make_shared<ModelAnimator>(shader);
		modelRenderer->SetModel(RESOURCES->Get<Model>(L"Robot"));
		modelRenderer->SetPass(0);
		robot->AddComponent(modelRenderer);

		
#if 0
		shared_ptr<SphereCollider> collider = make_shared<SphereCollider>(shader);
		collider->SetRadius(80.f);
		collider->SetOffset(Vec3(0.f, 8.f, 0.f));
		robot->AddComponent(collider);
#elif 0
		shared_ptr<BoxCollider> collider = make_shared<BoxCollider>(shader);
		float extent = 40.f;
		collider->SetHalfExtents(myVec3(extent, extent, extent));
		collider->SetOffset(Vec3(0.f, 8.f, 0.f));
		robot->AddComponent(collider);
#else
		shared_ptr<CapsuleCollider> collider = make_shared<CapsuleCollider>(shader);
		collider->SetOffset(Vec3(0.f, 8.f, 0.f));
		collider->SetRadius(30.f);
		collider->SetHalfHeight(50.f);
		robot->AddComponent(collider);

#endif


		robot->AddComponent(make_shared<PlayerController>());
		robot->AddComponent(make_shared<PlayerMovement>());



		robot->AddComponent(make_shared<Rigidbody>());

		robot->SetLayermask(Layermask::Player);

		robot->SetName("Robot");
		gameObjects.push_back(robot);
		Add(ret, gameObjects);
#endif
	}



	{
		vector<shared_ptr<GameObject>> gameObjects;

		shared_ptr<class Model> m1 = RESOURCES->Get<Model>(L"Goomba");

		shared_ptr<GameObject> goomba = make_shared<GameObject>();
		goomba->GetOrAddTransform()->SetPosition(Vec3(0, 0, 20));
		float scale = 0.1f;
		goomba->GetOrAddTransform()->SetScale(Vec3(scale, scale, scale));


#if 0
		goomba->AddComponent(make_shared<ModelRenderer>(shader));
		{
			goomba->GetModelRenderer()->SetModel(m1);
			goomba->GetModelRenderer()->SetPass(0);
		}
#else
		shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
		animator->SetModel(m1);
		goomba->AddComponent(animator);
#endif

#if 1
		shared_ptr<SphereCollider> collider = make_shared<SphereCollider>(shader);
		collider->SetRadius(80.f);
		collider->SetOffset(Vec3(0.f, 8.f, 0.f));
		collider->SetRigiMass(200);

		goomba->AddComponent(collider);
#elif 0
		shared_ptr<BoxCollider> collider = make_shared<BoxCollider>(shader);
		float halfExtent = 80.f;
		collider->SetHalfExtents(Vec3(halfExtent, halfExtent, halfExtent));
		collider->SetOffset(Vec3(0.f, 8.f, 0.f));
		collider->SetRigiMass(200);

		goomba->AddComponent(collider);
#else
		shared_ptr<CapsuleCollider> collider = make_shared<CapsuleCollider>(shader);
		collider->SetOffset(Vec3(0.f, 10.f, 0.f));
		collider->SetRadius(30.f);
		collider->SetHalfHeight(70.f);
		goomba->AddComponent(collider);

#endif

		goomba->AddComponent(make_shared<Rigidbody>());

		goomba->AddComponent(make_shared<Goomba>());


		goomba->SetLayermask(Layermask::Enemy);

		goomba->SetName("Goomba");

		gameObjects.push_back(goomba);

		Add(ret, gameObjects);
	}



	{
#if 1
		vector<shared_ptr<GameObject>> gameObjects;

		shared_ptr<GameObject> terrain = make_shared<GameObject>();
		float scale = 10.f;
		terrain->GetOrAddTransform()->SetScale(Vec3(scale, scale, scale));

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>(shader);
		terrain->AddComponent(meshRenderer);

		
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->CreateGrid(20.f, 20.f);


		meshRenderer->SetMesh(mesh);

		shared_ptr<Material> material = RESOURCES->Get<Material>(L"Wood");
		meshRenderer->SetMaterial(material);
		
		

#if 1
		shared_ptr<BoxCollider> collider = make_shared<BoxCollider>(shader);

		collider->SetOffset(Vec3(10.059384, 0.000000, 10.004920));
		collider->SetHalfExtents(Vec3(9.940393, 0.071789, 9.953076));

		terrain->AddComponent(collider);
#else

#endif
		terrain->SetLayermask(Layermask::Terrain);
		
		terrain->SetName("Terrain");
		gameObjects.push_back(terrain);
		Add(ret, gameObjects);
#endif
	}





#if 1
	{
		vector<shared_ptr<GameObject>> gameObjects;

		shared_ptr<GameObject> skySpace = make_shared<GameObject>();
		skySpace->GetOrAddTransform();
		

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>(shader);
		meshRenderer->SetMesh(RESOURCES->Get<Mesh>(L"Sphere"));
		meshRenderer->SetMaterial(RESOURCES->Get<Material>(L"Sky_Space"));
		meshRenderer->SetPass(MeshRenderer::Pass::SKY);

		skySpace->AddComponent(meshRenderer);

		
		skySpace->SetName("Sky_Space");
		gameObjects.push_back(skySpace);
		Add(ret, gameObjects);
	}
#endif






#if 0
	{
		vector<shared_ptr<GameObject>> gameObjects;

		shared_ptr<GameObject> sphere = make_shared<GameObject>();
		sphere->GetOrAddTransform()->SetScale(Vec3(5.f, 5.f, 5.f));
		sphere->GetTransform()->SetPosition(Vec3(15.f, 0.f, 0.f));
		sphere->AddComponent(make_shared<MeshRenderer>(shader));

		shared_ptr<Mesh> mesh = RESOURCES->Get<Mesh>(L"Sphere");
		sphere->GetMeshRenderer()->SetMesh(mesh);

		shared_ptr<Material> material = RESOURCES->Get<Material>(L"Leather");
		sphere->GetMeshRenderer()->SetMaterial(material);
		sphere->AddComponent(make_shared<TempRotator>());

		gameObjects.push_back(sphere);

		{

			shared_ptr<GameObject> sphere2 = sphere->Clone(nullptr, gameObjects);
			shared_ptr<GameObject> sphere3 = sphere->Clone(nullptr, gameObjects);

			sphere2->GetTransform()->SetPosition(Vec3(-10.f, 0.f, 0.f));
			sphere2->GetTransform()->SetParent(*sphere->GetTransform().get());

			sphere3->GetTransform()->SetPosition(Vec3(-35.f, 0.f, 0.f));
			sphere3->GetTransform()->SetParent(*sphere2->GetTransform().get());


			sphere2->SetName("Earth");
			sphere3->SetName("Moon");
		}


		sphere->SetName("SunErathMoon");
		Add(ret, gameObjects);
	}
#endif

#if 1
	{
		vector<shared_ptr<GameObject>> gameObjects;

		shared_ptr<GameObject> box = make_shared<GameObject>();

		box->GetOrAddTransform();
		float boxScale = 10.f;
		box->GetTransform()->SetScale(Vec3(boxScale, boxScale, boxScale));

		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>(shader);
		shared_ptr<Material> material = RESOURCES->Get<Material>(L"Wood");
		meshRenderer->SetMaterial(material);
		MaterialDesc desc = { Color(0.8f, 0.8f, 0.8f, 0.8f), Color(1.f, 1.f, 1.f, 1.f), Color(1.f, 1.f, 1.f, 1.f), Color(0.f, 0.f, 0.f, 1.f) };
		material->SetMaterialDesc(desc);
		shared_ptr<Mesh> mesh = RESOURCES->Get<Mesh>(L"Cube");
		meshRenderer->SetMesh(mesh);
	
		box->AddComponent(meshRenderer);

		shared_ptr<BoxCollider> boxCollider = make_shared<BoxCollider>(shader);
		box->AddComponent(boxCollider);

		box->SetName("StaticBox");
		gameObjects.push_back(box);

		Add(ret, gameObjects);
	}
#endif

	

	return ret;
}

void PrefabLoader::Add(unordered_map<string, vector<shared_ptr<GameObject>>>& ret, 
	vector<shared_ptr<GameObject>> gameObjects)
{
	string key = gameObjects[0]->GetName();

	assert(key != "");

	auto it = ret.find(key);
	if (it != ret.end())
		assert(0);

	ret[key] = gameObjects;
}
