#pragma once

class Shader;

class ITransferShader
{
public:
	virtual void TransferShader(shared_ptr<Shader> shader) =0;
};

