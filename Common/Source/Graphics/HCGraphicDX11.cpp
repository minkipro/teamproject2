#include "stdafx.h"
#include "HCGraphicDX11.h"
#include "GlobalOption.h"
#include <DirectXColors.h>
#include <D3Dcompiler.h>
#include <WICTextureLoader.h>

using Microsoft::WRL::ComPtr;

void HCGraphicDX11::Init()
{
	m_Swapchain = std::make_unique<HCSwapchainDX11>();
	m_Swapchain->Init(HC::GO.GRAPHIC.NumSwapchainBuffer,
		m_WindowHandle, m_Device.GetAddressOf(), m_DeviceContext.GetAddressOf());

	m_Swapchain->Resize(HC::GO.WIN.WindowsizeX, HC::GO.WIN.WindowsizeY);

	CreateBaseSamplers();

	CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
	depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

	COM_HRESULT_IF_FAILED(
		m_Device->CreateDepthStencilState(&depthstencildesc, m_BaseDepthStencilState.GetAddressOf()), 
		"Failed to create depth stencil state.");

	CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
	COM_HRESULT_IF_FAILED(m_Device->CreateRasterizerState(&rasterizerDesc, m_BaseRasterizer.GetAddressOf()),
		"Failed to create rasterizer state.");

	D3D11_RENDER_TARGET_BLEND_DESC rtbd = { 0 };
	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

	D3D11_BLEND_DESC blendDesc = { 0 };
	blendDesc.RenderTarget[0] = rtbd;

	COM_HRESULT_IF_FAILED(m_Device->CreateBlendState(&blendDesc, m_BaseBlendState.GetAddressOf()), 
		"Failed to create blend state.");

	m_MainPassCB = std::make_unique<IHCDX11ConstBuffer<HC::MainPass>>(m_Device.Get(), m_DeviceContext.Get());
	ID3D11Buffer* baseCBs[] = { static_cast<ID3D11Buffer*>(m_MainPassCB->GetBuffer()) };

	m_DeviceContext->VSSetConstantBuffers(0, 1, baseCBs);
	m_DeviceContext->HSSetConstantBuffers(0, 1, baseCBs);
	m_DeviceContext->DSSetConstantBuffers(0, 1, baseCBs);
	m_DeviceContext->GSSetConstantBuffers(0, 1, baseCBs);
	m_DeviceContext->PSSetConstantBuffers(0, 1, baseCBs);
}

void HCGraphicDX11::Update()
{

}

void HCGraphicDX11::CreateGraphicPipeLine(const std::string& pipeLineName, HCGraphicPipeLine** out)
{
	
}

void HCGraphicDX11::CreateTextureBuffer(const std::string& bufferName, IHCTextureBuffer** out)
{
}

void HCGraphicDX11::CreateTexture(const std::string& textureName, const std::wstring& filePath, IHCTexture** out)
{
	
	ID3D11ShaderResourceView* textureView = nullptr;
	COM_HRESULT_IF_FAILED(DirectX::CreateWICTextureFromFile(m_Device.Get(), filePath.c_str(), nullptr, &textureView),
		"Texture load Fail");
	m_Textures[textureName] = std::make_unique<HCDX11Texture>(textureView);
	*out = m_Textures[textureName].get();
}

void HCGraphicDX11::CreateShaderResource(const std::string& resourceName, size_t stride, const POINT& size, IHCTexture** out)
{
}

void HCGraphicDX11::CreateCB(const std::string& bufferName, size_t stride, size_t num, IHCCBuffer** out)
{
}

void HCGraphicDX11::CreateShader(const std::string& shaderName, HC::SHADERTYPE type, const std::wstring& filePath, const std::string& entryPoint, IHCShader** out)
{
	std::string target;
	auto iter = m_Shaders.find(shaderName);
	COM_THROW_IF_FAILED(iter == m_Shaders.end(), "This shader name already has created");

	switch (type)
	{
	case HC::SHADERTYPE::VS:
	{
		target = "vs";
	}
		break;
	case HC::SHADERTYPE::GS:
	{
		target = "ps";
	}
		break;
	case HC::SHADERTYPE::HS:
	{
		target = "hs";
	}
		break;
	case HC::SHADERTYPE::DS:
	{
		target = "ds";
	}
		break;
	case HC::SHADERTYPE::PS:
	{
		target = "ps";
	}
		break;
	case HC::SHADERTYPE::COUNT:
	default:
		COM_THROW_IF_FAILED(false, "This shader type is invalid");
		break;
	}

	target += "_5_0";

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> byteCode = nullptr;
	ComPtr<ID3DBlob> errors;
	hr = D3DCompileFromFile(filePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
	{
		OutputDebugStringA((char*)errors->GetBufferPointer());
	}

	COM_HRESULT_IF_FAILED(hr, "shader compile error");

	switch (type)
	{
	case HC::SHADERTYPE::VS:
	{
		ID3D11VertexShader* temp = nullptr;
		m_Device->CreateVertexShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &temp);
		m_Shaders[shaderName] = std::make_unique<HCDX11Shader>(temp);
	}
	break;
	case HC::SHADERTYPE::GS:
	{
		ID3D11GeometryShader* temp = nullptr;
		m_Device->CreateGeometryShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &temp);
		m_Shaders[shaderName] = std::make_unique<HCDX11Shader>(temp);
	}
	break;
	case HC::SHADERTYPE::HS:
	{
		ID3D11HullShader* temp = nullptr;
		m_Device->CreateHullShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &temp);
		m_Shaders[shaderName] = std::make_unique<HCDX11Shader>(temp);
	}
	break;
	case HC::SHADERTYPE::DS:
	{
		ID3D11DomainShader* temp = nullptr;
		m_Device->CreateDomainShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &temp);
		m_Shaders[shaderName] = std::make_unique<HCDX11Shader>(temp);
	}
	break;
	case HC::SHADERTYPE::PS:
	{
		ID3D11PixelShader* temp = nullptr;
		m_Device->CreatePixelShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &temp);
		m_Shaders[shaderName] = std::make_unique<HCDX11Shader>(temp);
	}
	break;
	}

	*out = m_Shaders[shaderName].get();
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

	switch (pipeLine->m_Primitive)
	{
	case HC::PRIMITIVE_TOPOLOGY::POINT:
	{
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	}
		break;
	case HC::PRIMITIVE_TOPOLOGY::LINELIST:
	{
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	}
		break;
	case HC::PRIMITIVE_TOPOLOGY::LINESTRIP:
	{
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
	}
		break;
	case HC::PRIMITIVE_TOPOLOGY::TRIANGLELIST:
	{
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
		break;
	case HC::PRIMITIVE_TOPOLOGY::TRIANGLESTRIP:
	{
		m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	}
		break;
	default:
		break;
	}

	COM_THROW_IF_FAILED(pipeLine->m_InputSample != nullptr, "inputSample is not attached to pipeline : " + pipeLine->GetPipeLineName());
	
	auto iter = m_InputLayout.find(pipeLine->m_InputSample->GetInputName());
	if (iter == m_InputLayout.end())
	{
		CreateInputLayout(pipeLine->m_InputSample.get());

		iter= m_InputLayout.find(pipeLine->m_InputSample->GetInputName());
	}

	m_DeviceContext->IASetInputLayout(iter->second.Get());

	ID3D11Buffer* vertexBuffers[] = { static_cast<ID3D11Buffer*>(pipeLine->GetVertexBuffer()->GetBuffer()) };
	m_DeviceContext->IASetVertexBuffers(0, 1, vertexBuffers, pipeLine->GetVertexBuffer()->GetStrides(), pipeLine->GetVertexBuffer()->GetOffsets());

	if (pipeLine->m_Rasterizer)
	{

	}
	else
	{
		m_DeviceContext->RSSetState(m_BaseRasterizer.Get());
	}
	
	if (pipeLine->m_DepthStencilState)
	{

	}
	else
	{
		m_DeviceContext->OMSetDepthStencilState(m_BaseDepthStencilState.Get(), 0);
	}

	if (pipeLine->m_BlendState)
	{

	}
	else
	{
		m_DeviceContext->OMSetBlendState(m_BaseBlendState.Get(), NULL, 0xFFFFFFFF);
	}

	if(pipeLine->m_CBuffers.size())
	{

	}
}

void HCGraphicDX11::RenderObjects(const std::string& textureBufferName, const std::vector<const HC::InputDataSample*> objects)
{
	auto iter = m_TextureBuffers.find(textureBufferName);
	auto textureViews = iter->second->GetTextureViews();

	m_DeviceContext->PSSetShaderResources(10, textureViews.size(), &textureViews.front());
	m_DeviceContext->DrawInstanced(objects.size(), 1, 0, 0);
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
	COM_HRESULT_IF_FAILED(m_Device->CreateSamplerState(&samplerDesc, pointSampler.GetAddressOf()),
		"fail to create samplerstate");
	m_Samplers.push_back(pointSampler);

	//SamplerState gsamPointClamp : register(s1);
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	pointSampler = nullptr;
	COM_HRESULT_IF_FAILED(m_Device->CreateSamplerState(&samplerDesc, pointSampler.GetAddressOf()),
		"fail to create samplerstate");
	m_Samplers.push_back(pointSampler);

	//SamplerState gsamLinearWrap : register(s2);
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	pointSampler = nullptr;
	COM_HRESULT_IF_FAILED(m_Device->CreateSamplerState(&samplerDesc, pointSampler.GetAddressOf()),
		"fail to create samplerstate");
	m_Samplers.push_back(pointSampler);

	//SamplerState gsamLinearClamp : register(s3);
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	pointSampler = nullptr;
	COM_HRESULT_IF_FAILED(m_Device->CreateSamplerState(&samplerDesc, pointSampler.GetAddressOf()),
		"fail to create samplerstate");
	m_Samplers.push_back(pointSampler);

	//SamplerState gsamAnisotropicWrap : register(s4);
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 8;

	pointSampler = nullptr;
	COM_HRESULT_IF_FAILED(m_Device->CreateSamplerState(&samplerDesc, pointSampler.GetAddressOf()),
		"fail to create samplerstate");
	m_Samplers.push_back(pointSampler);

	//SamplerState gsamAnisotropicClamp : register(s5);
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 8;

	pointSampler = nullptr;
	COM_HRESULT_IF_FAILED(m_Device->CreateSamplerState(&samplerDesc, pointSampler.GetAddressOf()),
		"fail to create samplerstate");
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
	COM_HRESULT_IF_FAILED(m_Device->CreateSamplerState(&samplerDesc, pointSampler.GetAddressOf()),
		"fail to create samplerstate");
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

void HCGraphicDX11::CreateInputLayout(const HC::InputDataSample* sample)
{
	std::string name = sample->GetInputName();
	ComPtr<ID3D11InputLayout> layout = nullptr;
	std::vector<HCInputLayoutElement> InputData = sample->GetInputData();;
	std::vector<D3D11_INPUT_ELEMENT_DESC> dx11Elements(InputData.size());

	for (size_t i = 0; i < InputData.size(); i++)
	{
		D3D11_INPUT_ELEMENT_DESC temp = {};

		temp.SemanticName = InputData[i].SemanticName;
		temp.SemanticIndex = InputData[i].SemanticIndex;
		temp.Format = InputData[i].Format;
		temp.AlignedByteOffset = InputData[i].AlignedByteOffset;
		temp.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		dx11Elements.push_back(temp);
	}

	COM_HRESULT_IF_FAILED(m_Device->CreateInputLayout(
		&dx11Elements.front(), dx11Elements.size(), nullptr, 0, layout.GetAddressOf()),
		"fail to create inputlayout");

	m_InputLayout[name] = layout;
}
