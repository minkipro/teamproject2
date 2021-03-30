#pragma once
#include <vector>
#include <d3d11.h>
#include <wrl/client.h>

class HCSwapchainDX11
{
	struct RenderTargetResource
	{
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	RenderTargetView;
		
	};

public:
	HCSwapchainDX11()
		: m_NumSwapBuffer(0)
		, m_CurrBackBuffer(0)
	{

	}
	virtual ~HCSwapchainDX11() = default;

	void Init(UINT numSwapBuffer, HWND wnd, ID3D11Device** deviceOut, ID3D11DeviceContext** contextOut);
	void Resize(UINT windowX, UINT windowY);

	void PresentRenderTargetSetting(ID3D11DeviceContext* deviceContext, const float clearColor[4]);
	void Present();

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain>			m_Swapchain;
	std::vector<RenderTargetResource>				m_SwapResources;
	UINT											m_NumSwapBuffer;
	UINT											m_CurrBackBuffer;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>			m_DepthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	m_DepthStencilView;

	const DXGI_FORMAT								m_PresentBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	const DXGI_FORMAT								m_ColorBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	const DXGI_FORMAT								m_NormalBufferFormat = DXGI_FORMAT_R11G11B10_FLOAT;
	const DXGI_FORMAT								m_SpecPowBufferFormat = DXGI_FORMAT_R32_FLOAT;
	const DXGI_FORMAT								m_ObjectIDFormat = DXGI_FORMAT_R32_SINT;
};

