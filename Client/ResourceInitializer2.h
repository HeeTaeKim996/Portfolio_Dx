#pragma once
#include "Engine/ITransferShader.h"

class ResourceInitializer2 : public ITransferShader
{
public:
	virtual void TransferShader(shared_ptr<Shader> shader) override;
};

