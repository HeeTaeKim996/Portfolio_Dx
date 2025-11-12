#include "pch.h"
#include "Main.h"
#include "Engine/Game.h"
#include "Engine/Shader.h"


#include "ResourceInitializer2.h"
#include "MonobehaviourLoader.h"
#include "PrefabInitializer.h"
#include "SceneCodeLoader.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

	GameDesc desc;
	desc.hInstance = hInstance;


	shared_ptr<ResourceInitializer2> resourceInitializer2 = make_shared<ResourceInitializer2>();
	desc.resourceLoader = resourceInitializer2;
	
	shared_ptr<MonobehaviourLoader> monoLoader = make_shared<MonobehaviourLoader>();
	desc.MonobehaviourLoader = monoLoader;

	shared_ptr<PrefabLoader> prefabLoader = make_shared<PrefabLoader>();
	desc.prefabLoader = prefabLoader;

	shared_ptr<IReturnScene> sceneLoader = make_shared<SceneCodeLoader>();
	desc.sceneCodeLoader = sceneLoader;
	
	GAME->Run(desc);

	return 0;
}