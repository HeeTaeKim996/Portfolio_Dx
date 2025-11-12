#include "pch.h"
#include "AssimpTool.h"
#include "Converter.h"
#include "Engine/Utils.h"
#include "filesystem"

void AssimpTool::Init()
{
#if 0
	CreateModelData(L"Tower", L"Tower");
#endif

#if 0
	CreateModelData(L"Kachujin", L"Mesh");

	CreateAnimationData(L"Kachujin", L"Idle");
	CreateAnimationData(L"Kachujin", L"Run");
	CreateAnimationData(L"Kachujin", L"Slash");
#endif
	
#if 1
	CreateModelData(L"Robot", L"Robot_Walking");
	CreateAnimationData(L"Robot", L"Robot_Walking");
	CreateAnimationData(L"Robot", L"Robot_Alert");
	CreateAnimationData(L"Robot", L"Robot_Running");

	CreateAnimationData(L"Robot", L"AimingForward");
	CreateAnimationData(L"Robot", L"AimingBackward");
	CreateAnimationData(L"Robot", L"AimingRight");
	CreateAnimationData(L"Robot", L"AimingLeft");
#endif

#if 0
	CreateModelData(L"Goomba", L"Idle");
	CreateAnimationData(L"Goomba", L"Idle");
	CreateAnimationData(L"Goomba", L"Walk");
#endif
}




void AssimpTool::CreateModelData(wstring folderPath, wstring meshName)
{
	// ※ 규약이 필요한데, Assets 파일에 해당 FBX 모델을 담은, FBX 모델명과 동일한 파일명 필요
	shared_ptr<Converter> converter = make_shared<Converter>();
	wstring fbxPath = folderPath + L"/" + meshName + L".fbx";
	converter->ReadAssetFile(fbxPath);



	std::filesystem::path p(folderPath);
	wstring modelName = p.filename();
	wstring exportPath = modelName + L"/" + modelName;

	converter->ExportMaterialData(exportPath);
	converter->ExportModelData(exportPath);
}

void AssimpTool::CreateAnimationData(wstring folderPath, wstring animationName)
{
	shared_ptr<Converter> converter = make_shared<Converter>();
	wstring fbxPath = folderPath + L"/" + animationName + L".fbx";
	converter->ReadAssetFile(fbxPath);

	std::filesystem::path p(folderPath);
	wstring modelName = p.filename();
	wstring exportPath = modelName + L"/" + animationName;


	converter->ExportAnimationData(exportPath);
}
