#include "stdafx.h"
#include "HCGraphicDX11.h"
#include "GlobalOption.h"
#include <DirectXColors.h>
#include <D3Dcompiler.h>
#include <WICTextureLoader.h>
#include "HCFont.h"

using Microsoft::WRL::ComPtr;

HCGraphicDX11::~HCGraphicDX11()
{
	m_PipeLines.clear();
	m_PipeLineSlots.clear();
}

void HCGraphicDX11::Init()
{
	m_swapchain = std::make_unique<HCSwapchainDX11>();
	m_swapchain->Init(HC::GO.GRAPHIC.NumSwapchainBuffer,
		m_windowHandle, m_device.GetAddressOf(), m_deviceContext.GetAddressOf());

	m_swapchain->Resize(HC::GO.WIN.WindowsizeX, HC::GO.WIN.WindowsizeY);

	CreateBaseSamplers();

	CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
	depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

	COM_HRESULT_IF_FAILED(
		m_device->CreateDepthStencilState(&depthstencildesc, m_baseDepthStencilState.GetAddressOf()),
		"Failed to create depth stencil state.");

	CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
	COM_HRESULT_IF_FAILED(m_device->CreateRasterizerState(&rasterizerDesc, m_baseRasterizer.GetAddressOf()),
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

	COM_HRESULT_IF_FAILED(m_device->CreateBlendState(&blendDesc, m_baseBlendState.GetAddressOf()),
		"Failed to create blend state.");

	m_mainPassCB = std::make_unique<IHCDX11ConstBuffer<HC::MainPass>>(m_device.Get(), m_deviceContext.Get());
	ID3D11Buffer* baseCBs[] = { static_cast<ID3D11Buffer*>(m_mainPassCB->GetBuffer()) };

	m_deviceContext->VSSetConstantBuffers(0, 1, baseCBs);
	m_deviceContext->HSSetConstantBuffers(0, 1, baseCBs);
	m_deviceContext->DSSetConstantBuffers(0, 1, baseCBs);
	m_deviceContext->GSSetConstantBuffers(0, 1, baseCBs);
	m_deviceContext->PSSetConstantBuffers(0, 1, baseCBs);

	HC::MainPass mainPass;
	DirectX::XMMATRIX orthoP = DirectX::XMMatrixOrthographicOffCenterLH(
		0.0f, static_cast<float>(HC::GO.WIN.WindowsizeX),
		static_cast<float>(HC::GO.WIN.WindowsizeY), 0,
		D3D11_MIN_DEPTH, D3D11_MAX_DEPTH);

	DirectX::XMStoreFloat4x4(&mainPass.gOrthoMatrix, orthoP);

	m_mainPassCB->CopyData(&mainPass);
	m_font = std::make_unique<HCFont>();
	m_font.get()->Init((void*)m_device.Get(), (void*)m_deviceContext.Get());
	IHCFont::TextData tempData = { L"test", DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT2(1.0f, 1.0f) };
	m_font->SetText(tempData);
}

void HCGraphicDX11::Update()
{
	for (auto& it : m_PipeLines)
	{
		HCDX11VertexBuffer* currBuffer = static_cast<HCDX11VertexBuffer*>(it.second->GetVertexBuffer());
		auto reservedOBs = it.second->GetReservedObjects();
		UINT dataSize = it.second->GetCurrInputSample()->GetDataSize();

		size_t numObjects = 0;
		for (auto& it2 : reservedOBs)
		{
			numObjects += it2.second.size();
		}

		if (numObjects * dataSize > currBuffer->BufferSize)
		{
			//TODO: resize vertex buffer
		}

		size_t accumulatedByte = 0;
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};

		COM_HRESULT_IF_FAILED(m_deviceContext->Map(currBuffer->Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)
			, "Failed to map constant buffer.");

		for (auto& it2 : reservedOBs)
		{
			for (auto& it3 : it2.second)
			{
				CopyMemory(static_cast<BYTE*>(mappedResource.pData)+ accumulatedByte, it3, dataSize);
				accumulatedByte += dataSize;
			}
		}
		m_deviceContext->Unmap(currBuffer->Buffer.Get(), 0);
	}
}

void HCGraphicDX11::CreateGraphicPipeLine(const std::string& pipeLineName, HCGraphicPipeLine** out)
{
	auto iter = m_PipeLines.find(pipeLineName);
	COM_THROW_IF_FAILED(iter == m_PipeLines.end(), "This name is already created");

	HCDX11VertexBuffer* vertexBuffer = new HCDX11VertexBuffer;
	vertexBuffer->BufferSize = 2048;

	m_PipeLines[pipeLineName] = std::make_unique<HCGraphicPipeLine>(pipeLineName, vertexBuffer,
		[](void* vertexBufferPtr)
		{
			if (vertexBufferPtr)
			{
				HCDX11VertexBuffer* currBuffer = static_cast<HCDX11VertexBuffer*>(vertexBufferPtr);
				delete currBuffer;
			}
		});


	D3D11_BUFFER_DESC vertexBufferDesc = {};

	vertexBufferDesc.ByteWidth = static_cast<UINT>(vertexBuffer->BufferSize);
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;

	COM_HRESULT_IF_FAILED(
		m_device->CreateBuffer(&vertexBufferDesc, nullptr, vertexBuffer->Buffer.GetAddressOf()),
		"Fail to create vertexBuffer");

	*out = m_PipeLines[pipeLineName].get();
}

void HCGraphicDX11::CreateTextureBuffer(const std::string& bufferName, HCTextureBuffer** out)
{
	auto iter = m_textureBuffers.find(bufferName);
	COM_THROW_IF_FAILED(iter == m_textureBuffers.end(), "This textureBuffer name already has created");
	
	m_textureBuffers[bufferName] = std::make_unique<HCDX11TextureBuffer>();
	*out = m_textureBuffers[bufferName].get();
}

void HCGraphicDX11::CreateTexture(const std::wstring& filePath, IHCTexture** out)
{
	auto iter = m_textures.find(filePath);
	COM_THROW_IF_FAILED(iter == m_textures.end(), "This texture name already has created");

	ComPtr<ID3D11ShaderResourceView> textureView = nullptr;
	std::wstring realPath = m_textureFolderPath + filePath;
	COM_HRESULT_IF_FAILED(DirectX::CreateWICTextureFromFile(m_device.Get(), realPath.c_str(), nullptr, &textureView),
		"Texture load Fail");
	m_textures[filePath] = std::make_unique<HCDX11Texture>(textureView.Get());
	*out = m_textures[filePath].get();
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
	auto iter = m_shaders.find(shaderName);
	COM_THROW_IF_FAILED(iter == m_shaders.end(), "This shader name already has created");

	switch (type)
	{
	case HC::SHADERTYPE::VS:
	{
		target = "vs";
	}
	break;
	case HC::SHADERTYPE::GS:
	{
		target = "gs";
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
	D3D_SHADER_MACRO macro[] = { {NULL,NULL },{} };
#ifndef _WIN64
	macro[1].Name = "MACRO"
#endif
	hr = D3DCompileFromFile(filePath.c_str(), macro, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint.c_str(), target.c_str(), compileFlags, 0, byteCode.GetAddressOf(), errors.GetAddressOf());

	if (errors != nullptr)
	{
		OutputDebugStringA((char*)errors->GetBufferPointer());
	}

	COM_HRESULT_IF_FAILED(hr, "shader compile error");

	switch (type)
	{
	case HC::SHADERTYPE::VS:
	{
		ComPtr<ID3D11VertexShader> temp = nullptr;
		m_device->CreateVertexShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &temp);
		m_shaders[shaderName] = std::make_unique<HCDX11Shader>(temp.Get(), byteCode.Get());
	}
	break;
	case HC::SHADERTYPE::GS:
	{
		ComPtr<ID3D11GeometryShader> temp = nullptr;
		m_device->CreateGeometryShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &temp);
		m_shaders[shaderName] = std::make_unique<HCDX11Shader>(temp.Get());
	}
	break;
	case HC::SHADERTYPE::HS:
	{
		ComPtr<ID3D11HullShader> temp = nullptr;
		m_device->CreateHullShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &temp);
		m_shaders[shaderName] = std::make_unique<HCDX11Shader>(temp.Get());
	}
	break;
	case HC::SHADERTYPE::DS:
	{
		ComPtr<ID3D11DomainShader> temp = nullptr;
		m_device->CreateDomainShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &temp);
		m_shaders[shaderName] = std::make_unique<HCDX11Shader>(temp.Get());
	}
	break;
	case HC::SHADERTYPE::PS:
	{
		ComPtr<ID3D11PixelShader> temp = nullptr;
		m_device->CreatePixelShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &temp);
		m_shaders[shaderName] = std::make_unique<HCDX11Shader>(temp.Get());
	}
	break;
	}

	*out = m_shaders[shaderName].get();
}

void HCGraphicDX11::GetGraphicPipeLine(const std::string& pipeLineName, HCGraphicPipeLine** out)
{
}

void HCGraphicDX11::GetTextureBuffer(const std::string& bufferName, HCTextureBuffer** out)
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

		if (m_device.Get())
		{
			HC::GO.WIN.WindowsizeX = clientWidth;
			HC::GO.WIN.WindowsizeY = clientHeight;

			if (wParam == SIZE_MINIMIZED)
			{
				m_minimized = true;
				m_maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				m_minimized = false;
				m_maximized = true;
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (m_minimized)
				{
					m_minimized = false;
				}
				else if (m_maximized)
				{
					m_maximized = false;
				}
				else if (m_resizing)
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
		m_resizing = true;
		return 0;
	}
	case WM_EXITSIZEMOVE:
	{
		if (m_device.Get())
		{
			m_resizing = false;
			m_swapchain->Resize(HC::GO.WIN.WindowsizeX, HC::GO.WIN.WindowsizeY);
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
	m_swapchain->PresentRenderTargetSetting(m_deviceContext.Get(), DirectX::Colors::Aqua);
}

void HCGraphicDX11::RenderEnd()
{
	m_deviceContext->GSSetShader(nullptr, nullptr, 0);
	m_font.get()->Render();
	m_swapchain->Present();
}

void HCGraphicDX11::SetPipeLineObject(const HCGraphicPipeLine* pipeLine)
{
	for (unsigned int i = 0; i < static_cast<unsigned int>(HC::SHADERTYPE::COUNT); i++)
	{
		void* currShader = nullptr;

		if (pipeLine->m_shaders[i])
		{
			currShader = pipeLine->m_shaders[i]->GetShaderData();
		}

		switch (static_cast<HC::SHADERTYPE>(i))
		{
		case HC::SHADERTYPE::VS:
		{
			m_deviceContext->VSSetShader(static_cast<ID3D11VertexShader*>(currShader), nullptr, 0);
		}
		break;
		case HC::SHADERTYPE::GS:
		{
			m_deviceContext->GSSetShader(static_cast<ID3D11GeometryShader*>(currShader), nullptr, 0);
		}
		break;
		case HC::SHADERTYPE::HS:
		{
			m_deviceContext->HSSetShader(static_cast<ID3D11HullShader*>(currShader), nullptr, 0);
		}
		break;
		case HC::SHADERTYPE::DS:
		{
			m_deviceContext->DSSetShader(static_cast<ID3D11DomainShader*>(currShader), nullptr, 0);
		}
		break;
		case HC::SHADERTYPE::PS:
		{
			m_deviceContext->PSSetShader(static_cast<ID3D11PixelShader*>(currShader), nullptr, 0);
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
		m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	}
	break;
	case HC::PRIMITIVE_TOPOLOGY::LINELIST:
	{
		m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	}
	break;
	case HC::PRIMITIVE_TOPOLOGY::LINESTRIP:
	{
		m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
	}
	break;
	case HC::PRIMITIVE_TOPOLOGY::TRIANGLELIST:
	{
		m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	break;
	case HC::PRIMITIVE_TOPOLOGY::TRIANGLESTRIP:
	{
		m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	}
	break;
	default:
		break;
	}

	COM_THROW_IF_FAILED(pipeLine->GetCurrInputSample() != nullptr, "inputSample is not attached to pipeline : " + pipeLine->GetPipeLineName());

	auto iter = m_inputLayout.find(pipeLine->GetCurrInputSample()->GetInputName());
	if (iter == m_inputLayout.end())
	{
		CreateInputLayout(pipeLine->GetCurrInputSample(), 
			static_cast<HCDX11Shader*>(pipeLine->m_shaders[static_cast<unsigned int>(HC::SHADERTYPE::VS)]));

		iter = m_inputLayout.find(pipeLine->GetCurrInputSample()->GetInputName());
	}

	m_deviceContext->IASetInputLayout(iter->second.Get());

	HCDX11VertexBuffer* currBuffer = static_cast<HCDX11VertexBuffer*>(pipeLine->GetVertexBuffer());
	ID3D11Buffer* vertexBuffers[] = { currBuffer->Buffer.Get() };
	const UINT strides[] = { pipeLine->GetCurrInputSample()->GetDataSize() };
	const UINT offsets[] = { 0 };
	m_deviceContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);

	if (pipeLine->m_Rasterizer)
	{

	}
	else
	{
		m_deviceContext->RSSetState(m_baseRasterizer.Get());
	}

	if (pipeLine->m_DepthStencilState)
	{

	}
	else
	{
		m_deviceContext->OMSetDepthStencilState(m_baseDepthStencilState.Get(), 0);
	}

	if (pipeLine->m_BlendState)
	{

	}
	else
	{
		m_deviceContext->OMSetBlendState(m_baseBlendState.Get(), NULL, 0xFFFFFFFF);
	}

	if (pipeLine->m_CBuffers.size())
	{

	}
}

void HCGraphicDX11::RenderObjects(const std::string& textureBufferName, const std::vector<const HC::InputDataSample*> objects, size_t offset)
{
	auto iter = m_textureBuffers.find(textureBufferName);
	auto textureViews = iter->second->GetTextureViews();

	m_deviceContext->PSSetShaderResources(10, static_cast<UINT>(textureViews.size()), &textureViews.front());
	m_deviceContext->DrawInstanced(static_cast<UINT>(objects.size()), 1, static_cast<UINT>(offset), 0);
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
	COM_HRESULT_IF_FAILED(m_device->CreateSamplerState(&samplerDesc, pointSampler.GetAddressOf()),
		"fail to create samplerstate");
	m_samplers.push_back(pointSampler);

	//SamplerState gsamPointClamp : register(s1);
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	pointSampler = nullptr;
	COM_HRESULT_IF_FAILED(m_device->CreateSamplerState(&samplerDesc, pointSampler.GetAddressOf()),
		"fail to create samplerstate");
	m_samplers.push_back(pointSampler);

	//SamplerState gsamLinearWrap : register(s2);
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	pointSampler = nullptr;
	COM_HRESULT_IF_FAILED(m_device->CreateSamplerState(&samplerDesc, pointSampler.GetAddressOf()),
		"fail to create samplerstate");
	m_samplers.push_back(pointSampler);

	//SamplerState gsamLinearClamp : register(s3);
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	pointSampler = nullptr;
	COM_HRESULT_IF_FAILED(m_device->CreateSamplerState(&samplerDesc, pointSampler.GetAddressOf()),
		"fail to create samplerstate");
	m_samplers.push_back(pointSampler);

	//SamplerState gsamAnisotropicWrap : register(s4);
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 8;

	pointSampler = nullptr;
	COM_HRESULT_IF_FAILED(m_device->CreateSamplerState(&samplerDesc, pointSampler.GetAddressOf()),
		"fail to create samplerstate");
	m_samplers.push_back(pointSampler);

	//SamplerState gsamAnisotropicClamp : register(s5);
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 8;

	pointSampler = nullptr;
	COM_HRESULT_IF_FAILED(m_device->CreateSamplerState(&samplerDesc, pointSampler.GetAddressOf()),
		"fail to create samplerstate");
	m_samplers.push_back(pointSampler);

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
	COM_HRESULT_IF_FAILED(m_device->CreateSamplerState(&samplerDesc, pointSampler.GetAddressOf()),
		"fail to create samplerstate");
	m_samplers.push_back(pointSampler);


	UINT numSamplers = static_cast<UINT>(m_samplers.size());
	std::vector<ID3D11SamplerState*> samplers;

	for (auto& it : m_samplers)
	{
		samplers.push_back(it.Get());
	}

	m_deviceContext->VSSetSamplers(0, numSamplers, &samplers.front());
	m_deviceContext->HSSetSamplers(0, numSamplers, &samplers.front());
	m_deviceContext->DSSetSamplers(0, numSamplers, &samplers.front());
	m_deviceContext->GSSetSamplers(0, numSamplers, &samplers.front());
	m_deviceContext->PSSetSamplers(0, numSamplers, &samplers.front());
}

void HCGraphicDX11::CreateInputLayout(const HC::InputDataSample* sample, HCDX11Shader* vs)
{
	std::string name = sample->GetInputName();
	ComPtr<ID3D11InputLayout> layout = nullptr;
	std::vector<HCInputLayoutElement> InputData = sample->GetInputData();;
	std::vector<D3D11_INPUT_ELEMENT_DESC> dx11Elements;

	for (size_t i = 0; i < InputData.size(); i++)
	{
		D3D11_INPUT_ELEMENT_DESC temp = {};

		temp.SemanticName = InputData[i].SemanticName.c_str();
		temp.SemanticIndex = InputData[i].SemanticIndex;
		temp.Format = InputData[i].Format;
		temp.AlignedByteOffset = InputData[i].AlignedByteOffset;
		temp.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		dx11Elements.push_back(temp);
	}

	COM_HRESULT_IF_FAILED(
		m_device->CreateInputLayout(&dx11Elements.front(), static_cast<UINT>(dx11Elements.size()), 
		vs->GetCPUData()->GetBufferPointer(), vs->GetCPUData()->GetBufferSize(),
		layout.GetAddressOf()),
		"fail to create inputlayout");

	m_inputLayout[name] = layout;
}
