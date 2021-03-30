#include "stdafx.h"
#include "HCGraphicDX11.h"
#include "HCSwapchainDX11.h"
#include "GlobalOption.h"
#include <DirectXColors.h>

using Microsoft::WRL::ComPtr;

void HCGraphicDX11::Init()
{
	m_Swapchain = std::make_unique<HCSwapchainDX11>();
	m_Swapchain->Init(HC::GO.GRAPHIC.NumSwapchainBuffer, 
		m_WindowHandle, m_Device.GetAddressOf(), m_DeviceContext.GetAddressOf());

	m_Swapchain->Resize(HC::GO.WIN.WindowsizeX, HC::GO.WIN.WindowsizeY);
}

void HCGraphicDX11::Update()
{
}

LRESULT HCGraphicDX11::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_SIZE:
		{
			int clientWidth = LOWORD(lParam);
			int clientHeight = HIWORD(lParam);

			if (m_Device.Get())
			{
				HC::GO.WIN.WindowsizeX = clientWidth;
				HC::GO.WIN.WindowsizeY = clientHeight;

				if (wParam == SIZE_MINIMIZED)
				{
					m_Minimized = true;
					m_Maximized = false;
				}
				else if (wParam == SIZE_MAXIMIZED)
				{
					m_Minimized = false;
					m_Maximized = true;
				}
				else if (wParam == SIZE_RESTORED)
				{
					if (m_Minimized)
					{
						m_Minimized = false;
					}
					else if (m_Maximized)
					{
						m_Maximized = false;
					}
					else if (m_Resizing)
					{

					}
					else
					{

					}
				}
			}

			return 0;
		}
		case WM_ENTERSIZEMOVE:
		{
			m_Resizing = true;
			return 0;
		}
		case WM_EXITSIZEMOVE:
		{
			if (m_Device.Get())
			{
				m_Resizing = false;
				m_Swapchain->Resize(HC::GO.WIN.WindowsizeX, HC::GO.WIN.WindowsizeY);
			}
	
			return 0;
		}
		case WM_GETMINMAXINFO:
		{
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
			return 0;
		}
	}

	return 0;
}

void HCGraphicDX11::RenderBegin()
{
	m_Swapchain->PresentRenderTargetSetting(m_DeviceContext.Get(), DirectX::Colors::Aqua);
}

void HCGraphicDX11::RenderEnd()
{
	m_Swapchain->Present();
}

void HCGraphicDX11::SetPipeLineObject(HCGraphicPipeLine* pipeLine)
{
}

void HCGraphicDX11::RenderObjects(const std::string& textureBufferName, const std::vector<const GameObject*> objects)
{
}
