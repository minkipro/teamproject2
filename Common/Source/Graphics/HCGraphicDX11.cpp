#include "stdafx.h"
#include "HCGraphicDX11.h"
#include "GlobalOption.h"
#include <DirectXColors.h>
#include <WICTextureLoader.h>

using Microsoft::WRL::ComPtr;

void HCGraphicDX11::Init()
{
	m_Swapchain = std::make_unique<HCSwapchainDX11>();
	m_Swapchain->Init(HC::GO.GRAPHIC.NumSwapchainBuffer,
		m_WindowHandle, m_Device.GetAddressOf(), m_DeviceContext.GetAddressOf());

	m_Swapchain->Resize(HC::GO.WIN.WindowsizeX, HC::GO.WIN.WindowsizeY);

	CreateBaseSamplers();
	//test
	HCTexture testTexture;
	IHCTexture* pTestTexture = &testTexture;
	CreateTexture("../Common/Texture/knight.png", L"../Common/Texture/knight.png", &pTestTexture);//이거 텍스쳐 이름이 필요할까.. 그냥 파일패스로 하는게?
	POINT tSize = pTestTexture->GetTextureSize();
	//~test
}

void HCGraphicDX11::Update()
{
}

HRESULT HCGraphicDX11::CreateGraphicPipeLine(const std::string& pipeLineName, HCGraphicPipeLine** out)
{
	return E_NOTIMPL;
}

HRESULT HCGraphicDX11::CreateTextureBuffer(const std::string& bufferName, IHCTextureBuffer** out)
{
	return E_NOTIMPL;
}

HRESULT HCGraphicDX11::CreateTexture(const std::string& textureName, const std::wstring& filePath, IHCTexture** out)
{
	
	ID3D11ShaderResourceView** textureView = static_cast<ID3D11ShaderResourceView**>((*out)->GetTextureData());
	HRESULT hr = DirectX::CreateWICTextureFromFile(m_Device.Get(), filePath.c_str(), nullptr, textureView);
	if (hr == S_OK)
	{
		(*out)->SetName(textureName);//텍스쳐 로드에 실패했으면 어떻게해야할지 생각..
	}
	return hr;
}

HRESULT HCGraphicDX11::CreateShaderResource(const std::string& resourceName, size_t stride, const POINT& size, IHCTexture** out)
{
	return E_NOTIMPL;
}

HRESULT HCGraphicDX11::CreateCB(const std::string& bufferName, size_t stride, size_t num, IHCCBuffer** out)
{
	return E_NOTIMPL;
}

HRESULT HCGraphicDX11::CreateShader(const std::string& shaderName, HC::SHADERTYPE type, const std::wstring& filePath, const std::string& entryPoint, IHCShader** out)
{
	return E_NOTIMPL;
}

void HCGraphicDX11::GetGraphicPipeLine(const std::string& pipeLineName, HCGraphicPipeLine** out)
{
}

void HCGraphicDX11::GetTextureBuffer(const std::string& bufferName, IHCTextureBuffer** out)
{
}

void HCGraphicDX11::GetTexture(const std::string& textureName, IHCTexture** out)
{
}

void HCGraphicDX11::GetShaderResource(const std::string& resourceName, IHCTexture** out)
{
}

void HCGraphicDX11::GetCB(const std::string& bufferName, IHCCBuffer** out)
{
}

void HCGraphicDX11::GetShader(const std::string& shaderName, IHCShader** out)
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

void HCGraphicDX11::ApplyBaseCB()
{
}

void HCGraphicDX11::SetPipeLineObject(const HCGraphicPipeLine* pipeLine)
{
	for (unsigned int i = 0; i < static_cast<unsigned int>(HC::SHADERTYPE::COUNT); i++)
	{
		void* currShader = nullptr;

		if (pipeLine->m_Shaders[i])
		{
			currShader = pipeLine->m_Shaders[i]->GetShaderData();
		}

		switch (static_cast<HC::SHADERTYPE>(i))
		{
		case HC::SHADERTYPE::VS:
		{
			m_DeviceContext->VSSetShader(static_cast<ID3D11VertexShader*>(currShader), nullptr, 0);
		}
		break;
		case HC::SHADERTYPE::GS:
		{
			m_DeviceContext->GSSetShader(static_cast<ID3D11GeometryShader*>(currShader), nullptr, 0);
		}
		break;
		case HC::SHADERTYPE::HS:
		{
			m_DeviceContext->HSSetShader(static_cast<ID3D11HullShader*>(currShader), nullptr, 0);
		}
		break;
		case HC::SHADERTYPE::DS:
		{
			m_DeviceContext->DSSetShader(static_cast<ID3D11DomainShader*>(currShader), nullptr, 0);
		}
		break;
		case HC::SHADERTYPE::PS:
		{
			m_DeviceContext->PSSetShader(static_cast<ID3D11PixelShader*>(currShader), nullptr, 0);
		}
		break;
		case HC::SHADERTYPE::COUNT:
		default:
			COM_THROW_IF_FAILED(false, "this shader type is not corrected");
			break;
		}
	}

	if (pipeLine->m_Rasterizer)
	{

	}
	else
	{
		m_DeviceContext->RSSetState(m_BaseRasterizer.Get());

	}

	if (pipeLine->m_BlendState)
	{

	}
	else
	{
		m_DeviceContext->OMSetBlendState(m_BaseBlend.Get(), NULL, 0xFFFFFFFF);
	}

	std::vector<ID3D11SamplerState*> samplers;

	for (auto& it : m_Samplers)
	{
		samplers.push_back(it.Get());
	}
	m_DeviceContext->PSSetSamplers(0, static_cast<UINT>(samplers.size()), &samplers.front());

	//m_DeviceContext->IASetPrimitiveTopology();
	//m_DeviceContext->IASetInputLayout();

	//m_Device->CreateInputLayout()
}

void HCGraphicDX11::RenderObjects(const std::string& textureBufferName, const std::vector<const GameObject*> objects)
{


}

void HCGraphicDX11::CreateBaseSamplers()
{
	D3D11_SAMPLER_DESC samplerDesc = {};
	ComPtr<ID3D11SamplerState> pointSampler;

	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//SamplerState gsamPointWrap : register(s0);
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	pointSampler = nullptr;
	m_Device->CreateSamplerState(&samplerDesc, pointSampler.GetAddressOf());
	m_Samplers.push_back(pointSampler);

	//SamplerState gsamPointClamp : register(s1);
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	pointSampler = nullptr;
	m_Device->CreateSamplerState(&samplerDesc, pointSampler.GetAddressOf());
	m_Samplers.push_back(pointSampler);

	//SamplerState gsamLinearWrap : register(s2);
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	pointSampler = nullptr;
	m_Device->CreateSamplerState(&samplerDesc, pointSampler.GetAddressOf());
	m_Samplers.push_back(pointSampler);

	//SamplerState gsamLinearClamp : register(s3);
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	pointSampler = nullptr;
	m_Device->CreateSamplerState(&samplerDesc, pointSampler.GetAddressOf());
	m_Samplers.push_back(pointSampler);

	//SamplerState gsamAnisotropicWrap : register(s4);
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 8;

	pointSampler = nullptr;
	m_Device->CreateSamplerState(&samplerDesc, pointSampler.GetAddressOf());
	m_Samplers.push_back(pointSampler);

	//SamplerState gsamAnisotropicClamp : register(s5);
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 8;

	pointSampler = nullptr;
	m_Device->CreateSamplerState(&samplerDesc, pointSampler.GetAddressOf());
	m_Samplers.push_back(pointSampler);

	//SamplerComparisonState gsamShadow : register(s6);
	samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 1.0f;

	pointSampler = nullptr;
	m_Device->CreateSamplerState(&samplerDesc, pointSampler.GetAddressOf());
	m_Samplers.push_back(pointSampler);


	UINT numSamplers = static_cast<UINT>(m_Samplers.size());
	std::vector<ID3D11SamplerState*> samplers;

	for (auto& it : m_Samplers)
	{
		samplers.push_back(it.Get());
	}

	m_DeviceContext->VSSetSamplers(0, numSamplers, &samplers.front());
	m_DeviceContext->HSSetSamplers(0, numSamplers, &samplers.front());
	m_DeviceContext->DSSetSamplers(0, numSamplers, &samplers.front());
	m_DeviceContext->GSSetSamplers(0, numSamplers, &samplers.front());
	m_DeviceContext->PSSetSamplers(0, numSamplers, &samplers.front());
}
