#include "stdafx.h"
#include "HCSwapchainDX11.h"
#include "GlobalOption.h"

void HCSwapchainDX11::Init(UINT numSwapBuffer, HWND wnd, ID3D11Device** deviceOut, ID3D11DeviceContext** contextOut)
{
	m_NumSwapBuffer = numSwapBuffer;
	DXGI_SWAP_CHAIN_DESC scd = { 0 };

	scd.BufferDesc.Width = HC::GO.WIN.WindowsizeX;
	scd.BufferDesc.Height = HC::GO.WIN.WindowsizeY;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = m_PresentBufferFormat;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = numSwapBuffer;
	scd.OutputWindow = wnd;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr;
	COM_HRESULT_IF_FAILED(D3D11CreateDeviceAndSwapChain(nullptr,
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL,
		NULL,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&scd,
		m_Swapchain.GetAddressOf(),
		deviceOut,
		NULL,
		contextOut),
		"Failed to create device and swapchain.");
}

void HCSwapchainDX11::Resize(UINT windowX, UINT windowY)
{
	m_SwapResources.clear();
	m_SwapResources.resize(m_NumSwapBuffer);
	m_Swapchain->ResizeBuffers(m_NumSwapBuffer, windowX, windowY, m_PresentBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	COM_HRESULT_IF_FAILED(m_Swapchain->GetDevice(IID_PPV_ARGS(device.GetAddressOf())), "didn't get d3d11 device");;

	for (UINT i = 0; i < m_NumSwapBuffer; i++)
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		COM_HRESULT_IF_FAILED(
			m_Swapchain->GetBuffer(i, IID_PPV_ARGS(backBuffer.GetAddressOf())),
			"GetBuffer Failed.");
		COM_HRESULT_IF_FAILED(
			device->CreateRenderTargetView(backBuffer.Get(), NULL,
				m_SwapResources[i].RenderTargetView.GetAddressOf()),
			"Failed to create render target view.");
	}

	//Describe our Depth/Stencil Buffer
	CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, windowX, windowY);
	depthStencilTextureDesc.MipLevels = 1;
	depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	COM_HRESULT_IF_FAILED(
		device->CreateTexture2D(&depthStencilTextureDesc, NULL, m_DepthStencilBuffer.GetAddressOf()),
		"Failed to create depth stencil buffer.");

	COM_HRESULT_IF_FAILED(
		device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), NULL, m_DepthStencilView.GetAddressOf()), 
		"Failed to create depth stencil view.");
}

void HCSwapchainDX11::PresentRenderTargetSetting(ID3D11DeviceContext* deviceContext, const float clearColor[4])
{
	auto present = m_SwapResources[m_CurrBackBuffer].RenderTargetView.Get();

	deviceContext->OMSetRenderTargets(1, &present, m_DepthStencilView.Get());
	deviceContext->ClearRenderTargetView(present, clearColor);
	deviceContext->ClearDepthStencilView(m_DepthStencilView.Get(), 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void HCSwapchainDX11::Present()
{
	COM_HRESULT_IF_FAILED(m_Swapchain->Present(0, 0), "Rendertarget Present error");
	m_CurrBackBuffer = (m_CurrBackBuffer + 1) % m_NumSwapBuffer;
}
