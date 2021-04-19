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
		: m_numSwapBuffer(0)
		, m_currBackBuffer(0)
	{

	}
	virtual ~HCSwapchainDX11() = default;

	void Init(UINT numSwapBuffer, HWND wnd, ID3D11Device** deviceOut, ID3D11DeviceContext** contextOut);
	void Resize(UINT windowX, UINT windowY);

	void PresentRenderTargetSetting(ID3D11DeviceContext* deviceContext, const float clearColor[4]);
	void Present();

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain>			m_swapchain;
	std::vector<RenderTargetResource>				m_swapResources;
	UINT											m_numSwapBuffer;
	UINT											m_currBackBuffer;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>			m_depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	m_depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	m_renderTargetView;
	
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			m_renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			m_renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	m_renderTargetView;



	const DXGI_FORMAT								m_presentBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	const DXGI_FORMAT								m_colorBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	const DXGI_FORMAT								m_normalBufferFormat = DXGI_FORMAT_R11G11B10_FLOAT;
	const DXGI_FORMAT								m_specPowBufferFormat = DXGI_FORMAT_R32_FLOAT;
	const DXGI_FORMAT								m_objectIDFormat = DXGI_FORMAT_R32_SINT;
};

