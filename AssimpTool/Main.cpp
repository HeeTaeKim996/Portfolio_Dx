#include "pch.h"
#include "Main.h"
#include "Engine/Game.h"
#include "AssimpTool.h"









int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

	shared_ptr<AssimpTool> assimpTool = make_shared<AssimpTool>();
	assimpTool->Init();


	return 0;
}