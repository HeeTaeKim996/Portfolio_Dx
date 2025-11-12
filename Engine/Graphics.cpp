#include "pch.h"
#include "Graphics.h"

#if 1
void Graphics::Init(HWND hwnd)
{
	_hwnd = hwnd;

	CreateDeviceAndSwapChain();
	CreateRenderTargetView();
	CreateDepthStancilView();
	SetViewport();
}

void Graphics::RenderBegin()
{
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), (float*)(&GAME->GetGameDesc().clearColor));
	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1, 0);
	_deviceContext->RSSetViewports(1, &_viewport);

	/*
		- ClearDepthStancilView 의 (3)은, 모든 픽셀을 해당 값으로 초기화. _viewport.MaxDepth = 1.0f; 이었기때문에
		  1로 초기화
	*/
}

void Graphics::RenderEnd()
{
	HRESULT hr = _swapChain->Present(1, 0);
	CHECK(hr);
}
void Graphics::CreateDeviceAndSwapChain()
{
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	{
		desc.BufferDesc.Width = GAME->GetGameDesc().width;
		desc.BufferDesc.Height = GAME->GetGameDesc().height;
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 1;
		desc.OutputWindow = _hwnd;
		desc.Windowed = TRUE;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	/*
		- 앞서 Game.cpp 에서 봤듯이, Render 는 여유가 될때마다 호출이 되는데, 위 Numerator - Denominator는 60fps다
		  만약 Render 가 60fps 보다 높은 빈도로 호출된다면, _swapChain->Present(1, 0) 으로 할시,
		  60fps 에 맞춰 동기(async) 대기한다
	*/
	



	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&desc,
		_swapChain.GetAddressOf(),
		_device.GetAddressOf(),
		nullptr,
		_deviceContext.GetAddressOf()
	);

	CHECK(hr);
}

void Graphics::CreateRenderTargetView()
{
	HRESULT hr;

	ComPtr<ID3D11Texture2D> backBuffer = nullptr;
	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
	CHECK(hr);

	hr = _device->CreateRenderTargetView(backBuffer.Get(), nullptr, _renderTargetView.GetAddressOf());
	CHECK(hr);
}

void Graphics::CreateDepthStancilView()
{
	{
		D3D11_TEXTURE2D_DESC desc = { 0 };
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = static_cast<uint32>(GAME->GetGameDesc().width);
		desc.Height = static_cast<uint32>(GAME->GetGameDesc().height);
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		HRESULT hr = DEVICE->CreateTexture2D(&desc, nullptr, _depthStencilTexture.GetAddressOf());
		CHECK(hr);
		/*
			- MipLevels		: 앞선 00.Global.fx 의 주석의 Mip 과 같은 개념. 1 이라면, 원본 해상도만을 저장
							  만약 n이라면, n개의 MipMap 을 저장. 0이라면, width * height -> 1 * 1 까지 모두 저장
			- DXGI_FORMAT_D24_UNORM_S8_UINT	: 픽셀당, D24(깊이값을 24비트로), S8(스텐실을 8비트로) 총 32비트로 저장
		*/
	}

	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		HRESULT hr = DEVICE->CreateDepthStencilView(_depthStencilTexture.Get(), &desc,
			_depthStencilView.GetAddressOf());
		CHECK(hr);
	}

}

void Graphics::SetViewport()
{
	_viewport.TopLeftX = 0.0f;
	_viewport.TopLeftY = 0.0f;
	_viewport.Width = static_cast<float>(GAME->GetGameDesc().width);
	_viewport.Height = static_cast<float>(GAME->GetGameDesc().height);
	_viewport.MinDepth = 0.0f;
	_viewport.MaxDepth = 1.0f;
}
#endif













































































#if 0 //First
void Graphics::Init(HWND hwnd)
{
	_hwnd = hwnd;

	CreateDeviceAndSwapChain();
	CreateRenderTargetView();
	SetViewport();
}

void Graphics::RenderBegin()
{
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr);
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), (float*)(&GAME->GetGameDesc().clearColor));
	_deviceContext->RSSetViewports(1, &_viewport);
}

void Graphics::RenderEnd()
{
	HRESULT hr = _swapChain->Present(1, 0);
	CHECK(hr);
}

void Graphics::CreateDeviceAndSwapChain()
{
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	{
		desc.BufferDesc.Width = GAME->GetGameDesc().width;
		desc.BufferDesc.Height = GAME->GetGameDesc().height;
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 1;
		desc.OutputWindow = _hwnd;
		desc.Windowed = TRUE;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&desc,
		_swapChain.GetAddressOf(),
		_device.GetAddressOf(),
		nullptr,
		_deviceContext.GetAddressOf()
	);

	CHECK(hr);
}

void Graphics::CreateRenderTargetView()
{
	HRESULT hr;

	ComPtr<ID3D11Texture2D> backBuffer = nullptr;
	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
	CHECK(hr);

	hr = _device->CreateRenderTargetView(backBuffer.Get(), nullptr, _renderTargetView.GetAddressOf());
	CHECK(hr);
}

void Graphics::SetViewport()
{
	_viewport.TopLeftX = 0.0f;
	_viewport.TopLeftY = 0.0f;
	_viewport.Width = static_cast<float>(GAME->GetGameDesc().width);
	_viewport.Height = static_cast<float>(GAME->GetGameDesc().height);
	_viewport.MinDepth = 0.0f;
	_viewport.MaxDepth = 1.0f;
}
#endif // First