#pragma once

class AssimpTool
{

public:
	void Init() ;



private:
	void CreateModelData(wstring folerPath, wstring meshName);

	void CreateAnimationData(wstring folerPath, wstring animationName);
};

