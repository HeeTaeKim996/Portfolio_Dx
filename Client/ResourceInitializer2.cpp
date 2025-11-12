#include "pch.h"
#include "ResourceInitializer2.h"
#include "Engine/ResourceManager.h"
#include "Engine/ResourceBase.h"
#include "Engine/Model.h"
#include "Engine/Material.h"

void ResourceInitializer2::TransferShader(shared_ptr<Shader> shader)
{
	
	{
		shared_ptr<Model> m1 = make_shared<Model>(L"Kachujin");
		
		//m1->ReadModel();
		//m1->ReadMaterial();
		

		m1->ReadAnimation(L"Idle");
		m1->ReadAnimation(L"Run");
		m1->ReadAnimation(L"Slash");

		

		RESOURCES->Add<Model>(m1);
	}

	{
#if 0
		shared_ptr<Model> robotModel = make_shared<Model>();
		wstring modelPath = L"Robot/Robot";

		robotModel->ReadModel(modelPath);
		robotModel->ReadMaterial(modelPath);

		robotModel->SetName(L"Robot");
		RESOURCES->Add<Model>(robotModel);
#endif
	}

	{
#if 1
		shared_ptr<Model> robot = make_shared<Model>(L"Robot");
		
		//robot->ReadModel();
		//robot->ReadMaterial();
		
		robot->ReadAnimation(L"Robot_Walking");
		robot->ReadAnimation(L"Robot_Running");
		robot->ReadAnimation(L"Robot_Alert");
		
		robot->ReadAnimation(L"AimingForward");
		robot->ReadAnimation(L"AimingBackward");
		robot->ReadAnimation(L"AimingRight");
		robot->ReadAnimation(L"AimingLeft");

		RESOURCES->Add<Model>(robot);
#endif
	}


#if 1
	shared_ptr<Material> material = make_shared<Material>(shader);

	//shared_ptr<Texture> texture
	//	= RESOURCES->Load<Texture>(L"Wood", L"..\\Assets\\Resources\\Textures\\OtherTextures\\Wood.jpg");

	shared_ptr<Texture> texture
		= RESOURCES->Load<Texture>(L"Wood", L"..\\Assets\\Resources\\Textures\\OtherTextures\\Wood.jpg");

	material->SetDiffuseMap(texture);
	material->SetNormalMap(texture);
	material->SetSpecularMap(texture);

	MaterialDesc& desc = material->GetMaterialDesc();
	desc.ambient = Color(1.f, 1.f, 1.f, 1.f);
	desc.diffuse = Color(1.f, 1.f, 1.f, 1.f);
	desc.specular = Color(1.f, 1.f, 1.f, 1.f);
	desc.emissive = Color(0.f, 0.f, 0.f, 0.f);

	material->SetName(L"Wood");
	RESOURCES->Add(material);
#endif


	{ // Material
		shared_ptr<Material> material = make_shared<Material>(shader);

		shared_ptr<Texture> texture
			= RESOURCES->Load<Texture>(L"Leather_Texture", L"..\\Assets\\Resources\\Textures\\Leather.jpg");
		material->SetDiffuseMap(texture);

		shared_ptr<Texture> normal
			= RESOURCES->Load<Texture>(L"Leather_Normal", L"..\\Assets\\Resources\\Textures\\Leather_Normal.jpg");
		material->SetNormalMap(normal);

		MaterialDesc& desc = material->GetMaterialDesc();
		desc.ambient = Color(1.f, 1.f, 1.f, 1.f);
		desc.diffuse = Color(1.f, 1.f, 1.f, 1.f);
		desc.specular = Color(1.f, 1.f, 1.f, 1.f);
		desc.emissive = Color(1.f, 0.f, 0.f, 1.f);

		material->SetName(L"Leather");
		RESOURCES->Add(material);
	}


#if 1
	{ // Sky_Space
		shared_ptr<Material> material = make_shared<Material>(shader);

		shared_ptr<Texture> texture
			= RESOURCES->Load<Texture>
			(L"Sky_Space", L"..\\Assets\\Resources\\Textures\\OtherTextures\\Sky_Space.jpg");
		material->SetDiffuseMap(texture);

		material->SetName(L"Sky_Space");
		RESOURCES->Add<Material>(material);
	}
#endif

#if 1
	{
		shared_ptr<Model> goomba = make_shared<Model>(L"Goomba");
		goomba->ReadAnimation(L"Idle");
		goomba->ReadAnimation(L"Walk");
		RESOURCES->Add<Model>(goomba);
	}
#endif


}
