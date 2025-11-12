#include "pch.h"
#include "Texture.h"

#if 0
Texture::Texture() : Super(ResourceType::Texture)
{

}

Texture::~Texture()
{

}

void Texture::Load(const wstring& path)
{
	DirectX::TexMetadata md;
	HRESULT hr = ::LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, &md, _img);
	CHECK(hr);

	hr = ::CreateShaderResourceView(DEVICE.Get(), _img.GetImages(), _img.GetImageCount(), md, _shaderResourceView.GetAddressOf());
	CHECK(hr);
	
	_size.x = md.width;
	_size.y = md.height;
}
#endif







































































#if 1 // First
Texture::Texture() : Super(ResourceType::Texture)
{

}

Texture::~Texture()
{

}

void Texture::Load(const wstring& path)
{
	DirectX::TexMetadata md;
	HRESULT hr = ::LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, &md, _img);
	CHECK(hr);

	hr = ::CreateShaderResourceView(DEVICE.Get(), _img.GetImages(), _img.GetImageCount(), md, 
		_shaderResourceView.GetAddressOf());
	CHECK(hr);

	_size.x = md.width;
	_size.y = md.height;
}
#endif // First