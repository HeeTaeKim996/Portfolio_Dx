#pragma once

#if 1
class Graphics
{
	//DECLARE_SINGLE(Graphics);
private:
	Graphics() {}

public:
	static Graphics* GetInstance()
	{
		static Graphics instance;
		return &instance;
	}

public:
	void Init(HWND hwnd);

	void RenderBegin();
	void RenderEnd();

	ComPtr<ID3D11Device> GetDevice() { return _device; }
	ComPtr<ID3D11DeviceContext> GetDeviceContext() { return _deviceContext; }

private:
	void CreateDeviceAndSwapChain();
	void CreateRenderTargetView();
	void CreateDepthStancilView();
	void SetViewport();

private:
	HWND _hwnd = {};

	// Device & SwapChain
	ComPtr<ID3D11Device> _device = nullptr;
	ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;
	ComPtr<IDXGISwapChain> _swapChain = nullptr;

	// RTV
	ComPtr<ID3D11RenderTargetView> _renderTargetView;

	// DSV
	ComPtr<ID3D11Texture2D> _depthStencilTexture;
	ComPtr<ID3D11DepthStencilView> _depthStencilView;
	/*
		- 깊이값 산출은 RasterizerState 에서 자동 처리. (S, T, 1-S-T), 투영보정보간 사용해서 처리되는듯

		○ Stencil
		 - Depth 는 깊이값으로 해당 픽셀을 그릴지 말지를 결정한다면, Stencil 은 해당 픽셀에 특정 조건을 부여해서
		   특수 처리 용도로 사용
		 - ex) UI 렌더링을 위해 그외 렌더링 사용 제한. 거울 평면을 그리기 위해 렌더링 제한후, 해당 영역을 계산하여
		       거울 렌더링 등..
	*/

	// Misc
	D3D11_VIEWPORT _viewport = { 0 };
};
#endif

















































#if 0 // First
class Graphics
{
	DECLARE_SINGLE(Graphics);

public:
	void Init(HWND hWnd);

	void RenderBegin();
	void RenderEnd();

	ComPtr<ID3D11Device> GetDevice() { return _device; }
	ComPtr<ID3D11DeviceContext> GetDeviceContext() { return _deviceContext; }

private:
	void CreateDeviceAndSwapChain();
	void CreateRenderTargetView();
	void SetViewport();


private:
	HWND _hwnd = {};

	// Device & SwapChain
	ComPtr<ID3D11Device> _device = nullptr;
	ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;
	ComPtr<IDXGISwapChain> _swapChain = nullptr;

	// RTV
	ComPtr<ID3D11RenderTargetView> _renderTargetView;

	// Misc
	D3D11_VIEWPORT _viewport = { 0 };
};
#endif // First