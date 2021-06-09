#include "stdafx.h"
#include "HCSwapchainDX11.h"
#include "GlobalOption.h"

using Microsoft::WRL::ComPtr;

void HCSwapchainDX11::Init(UINT numSwapBuffer, HWND wnd, ID3D11Device** deviceOut, ID3D11DeviceContext** contextOut)
{
	m_numSwapBuffer = numSwapBuffer;
	DXGI_SWAP_CHAIN_DESC scd = { 0 };

	scd.BufferDesc.Width = HC::GO.WIN.WindowsizeX;
	scd.BufferDesc.Height = HC::GO.WIN.WindowsizeY;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = m_presentBufferFormat;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;

	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = numSwapBuffer;
	scd.OutputWindow = wnd;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	COM_HRESULT_IF_FAILED(D3D11CreateDeviceAndSwapChain(nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&scd,
		m_swapchain.GetAddressOf(),
		deviceOut,
		NULL,
		contextOut),
		"Failed to create device and swapchain.");
}

void HCSwapchainDX11::Resize(UINT windowX, UINT windowY)
{
	m_depthStencilBuffer = nullptr;
	m_depthStencilView = nullptr;
	m_renderTargetView = nullptr;

	m_swapchain->ResizeBuffers(m_numSwapBuffer, windowX, windowY, m_presentBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	COM_HRESULT_IF_FAILED(m_swapchain->GetDevice(IID_PPV_ARGS(device.GetAddressOf())), "didn't get d3d11 device");;

	D3D11_RENDER_TARGET_VIEW_DESC viewDesc;
	viewDesc.Format = m_presentBufferFormat;
	viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipSlice = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	COM_HRESULT_IF_FAILED(
		m_swapchain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())),
		"GetBuffer Failed.");
	COM_HRESULT_IF_FAILED(
		device->CreateRenderTargetView(backBuffer.Get(), &viewDesc,
			m_renderTargetView.GetAddressOf()),
		"Failed to create render target view.");

	CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, windowX, windowY);
	depthStencilTextureDesc.MipLevels = 1;
	depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	COM_HRESULT_IF_FAILED(
		device->CreateTexture2D(&depthStencilTextureDesc, nullptr, m_depthStencilBuffer.GetAddressOf()),
		"Failed to create depth stencil buffer.");

	COM_HRESULT_IF_FAILED(
		device->CreateDepthStencilView(m_depthStencilBuffer.Get(), nullptr, m_depthStencilView.GetAddressOf()), 
		"Failed to create depth stencil view.");
}

void HCSwapchainDX11::Present()
{
	COM_HRESULT_IF_FAILED(m_swapchain->Present(0, 0), "Rendertarget Present error");
	m_currBackBuffer = (m_currBackBuffer + 1) % m_numSwapBuffer;
}
