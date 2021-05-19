#include "stdafx.h"
#include "HCGraphicDX11.h"
#include "GlobalOption.h"
#include <DirectXColors.h>
#include <D3Dcompiler.h>
#include <WICTextureLoader.h>
#include "Util\StringHelper.h"
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
	CreateTextures();
	CreateGraphicPipeLineBaseSettings();
	 
	m_font = std::make_unique<HCFont>();
	m_font.get()->Init((void*)m_device.Get(), (void*)m_deviceContext.Get());
	IHCFont::TextData tempData = { L"test", DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT2(1.0f, 1.0f) };
	m_font->SetText(tempData);
}

void HCGraphicDX11::Update()
{
	HC::MainPass mainPass;
	DirectX::XMMATRIX orthoP = DirectX::XMMatrixOrthographicOffCenterLH(
		0.0f, static_cast<float>(HC::GO.WIN.WindowsizeX),
		static_cast<float>(HC::GO.WIN.WindowsizeY), 0,
		D3D11_MIN_DEPTH, D3D11_MAX_DEPTH);

	DirectX::XMStoreFloat4x4(&mainPass.OrthoMatrix, orthoP);

	m_mainPassCB->CopyData(&mainPass);
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

void HCGraphicDX11::CreateResource(const std::string& resourceName, const HC::GRAPHIC_RESOURCE_DESC& desc, IHCResource** out)
{
	/*switch (type)
	{
	case HC::GRAPHIC_RESOURCE_TYPE::DEFAULT_SHADER_RESOURCE:
	{
		D3D11_TEXTURE2D_DESC pixelFuncRenderTargetDesc = {};
		pixelFuncRenderTargetDesc.Usage = D3D11_USAGE_DEFAULT;
		pixelFuncRenderTargetDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
		pixelFuncRenderTargetDesc.Format = format;
		pixelFuncRenderTargetDesc.MipLevels = 1;
		pixelFuncRenderTargetDesc.ArraySize = 1;
		pixelFuncRenderTargetDesc.SampleDesc.Count = 1;
		pixelFuncRenderTargetDesc.Width = size.x;
		pixelFuncRenderTargetDesc.Height = size.y;

		ComPtr<ID3D11Texture2D> pixelFunc;
		COM_HRESULT_IF_FAILED(
			m_device->CreateTexture2D(&pixelFuncDesc, nullptr, pixelFunc.GetAddressOf()),
			"Failed to create pixelFunc buffer.");
	}
	break;
	case HC::GRAPHIC_RESOURCE_TYPE::READBACK_BUFFER:
	{
		D3D11_TEXTURE2D_DESC pixelFuncDesc = {};
		pixelFuncDesc.Usage = D3D11_USAGE_STAGING;
		pixelFuncDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		pixelFuncDesc.Format = format;
		pixelFuncDesc.MipLevels = 1;
		pixelFuncDesc.ArraySize = 1;
		pixelFuncDesc.SampleDesc.Count = 1;
		pixelFuncDesc.Width = size.x;
		pixelFuncDesc.Height = size.y;

		ComPtr<ID3D11Texture2D> pixelFunc;
		COM_HRESULT_IF_FAILED(
			m_device->CreateTexture2D(&pixelFuncDesc, nullptr, pixelFunc.GetAddressOf()),
			"Failed to create pixelFunc buffer.");
	}
	break;
	default:
		COM_THROW_IF_FAILED(false, "this type is not supported type");
		break;
	}*/
}

void HCGraphicDX11::CreateCB(const std::string& bufferName, size_t stride, size_t num, std::unique_ptr<IHCCBuffer>& out)
{
	out = std::make_unique<HCDX11ConstBuffer>(m_device.Get(), m_deviceContext.Get(), stride);
}

void HCGraphicDX11::CreateShader(const std::string& shaderName, HC::SHADER_TYPE type, const std::wstring& filePath, const std::string& entryPoint, IHCShader** out)
{
	std::string target;
	auto iter = m_shaders.find(shaderName);
	COM_THROW_IF_FAILED(iter == m_shaders.end(), "This shader name already has created");

	switch (type)
	{
	case HC::SHADER_TYPE::VS:
	{
		target = "vs";
	}
	break;
	case HC::SHADER_TYPE::GS:
	{
		target = "gs";
	}
	break;
	case HC::SHADER_TYPE::HS:
	{
		target = "hs";
	}
	break;
	case HC::SHADER_TYPE::DS:
	{
		target = "ds";
	}
	break;
	case HC::SHADER_TYPE::PS:
	{
		target = "ps";
	}
	break;
	case HC::SHADER_TYPE::COUNT:
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
		entryPoint.c_str(), target.c_str(), compileFlags, 0, byteCode.GetAddressOf(), errors.GetAddressOf());

	if (errors != nullptr)
	{
		OutputDebugStringA((char*)errors->GetBufferPointer());
	}

	COM_HRESULT_IF_FAILED(hr, "shader compile error");

	switch (type)
	{
	case HC::SHADER_TYPE::VS:
	{
		ComPtr<ID3D11VertexShader> temp = nullptr;
		m_device->CreateVertexShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &temp);
		m_shaders[shaderName] = std::make_unique<HCDX11Shader>(temp.Get(), byteCode.Get());
	}
	break;
	case HC::SHADER_TYPE::GS:
	{
		ComPtr<ID3D11GeometryShader> temp = nullptr;
		m_device->CreateGeometryShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &temp);
		m_shaders[shaderName] = std::make_unique<HCDX11Shader>(temp.Get());
	}
	break;
	case HC::SHADER_TYPE::HS:
	{
		ComPtr<ID3D11HullShader> temp = nullptr;
		m_device->CreateHullShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &temp);
		m_shaders[shaderName] = std::make_unique<HCDX11Shader>(temp.Get());
	}
	break;
	case HC::SHADER_TYPE::DS:
	{
		ComPtr<ID3D11DomainShader> temp = nullptr;
		m_device->CreateDomainShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &temp);
		m_shaders[shaderName] = std::make_unique<HCDX11Shader>(temp.Get());
	}
	break;
	case HC::SHADER_TYPE::PS:
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

void HCGraphicDX11::GetShaderResource(const std::string& resourceName, IHCResource** out)
{
}

void HCGraphicDX11::GetCB(const std::string& bufferName, IHCCBuffer** out)
{
}

void HCGraphicDX11::GetShader(const std::string& shaderName, IHCShader** out)
{
}

int HCGraphicDX11::GetTextureIndex(const std::wstring& textureName) const
{
	int result = -1;
	int bufferIndex = 0;
	int TextureIndex = 0;
	std::wstring directory = StringHelper::GetDirectoryFromPath(textureName);
	std::wstring texture = StringHelper::GetFileNameFromPath(textureName);

	if (directory.length() == 0)
	{
		directory = StringHelper::GetFileNameFromPath(HC::GO.GRAPHIC.TextureFolderPath);
		texture = directory + L"/" + textureName;
	}

	auto bufferIndexIter = m_textureBufferIndex.find(directory);
	COM_THROW_IF_FAILED(bufferIndexIter != m_textureBufferIndex.end(), "This TextureBuffer is not loaded");
	bufferIndex = bufferIndexIter->second;

	auto textureIter = m_textures[bufferIndex].TextureIndex.find(texture);
	COM_THROW_IF_FAILED(textureIter != m_textures[bufferIndex].TextureIndex.end(), "This Texture is not loaded");
	TextureIndex = textureIter->second;

	result = (bufferIndex << 16) + TextureIndex;

	return result;
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

			HC::MainPass mainPass;
			DirectX::XMMATRIX orthoP = DirectX::XMMatrixOrthographicOffCenterLH(
				0.0f, static_cast<float>(HC::GO.WIN.WindowsizeX),
				static_cast<float>(HC::GO.WIN.WindowsizeY), 0,
				D3D11_MIN_DEPTH, D3D11_MAX_DEPTH);

			DirectX::XMStoreFloat4x4(&mainPass.OrthoMatrix, orthoP);

			m_mainPassCB->CopyData(&mainPass);
			m_font = std::make_unique<HCFont>();
			m_font.get()->Init((void*)m_device.Get(), (void*)m_deviceContext.Get());
			IHCFont::TextData tempData = { L"test", DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT2(1.0f, 1.0f) };
			m_font->SetText(tempData);
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
	m_swapchain->Present();
}

void HCGraphicDX11::SetPipeLineObject(const HCGraphicPipeLine* pipeLine)
{
	for (unsigned int i = 0; i < static_cast<unsigned int>(HC::SHADER_TYPE::COUNT); i++)
	{
		void* currShader = nullptr;

		if (pipeLine->m_shaders[i])
		{
			currShader = pipeLine->m_shaders[i]->GetShaderData();
		}

		switch (static_cast<HC::SHADER_TYPE>(i))
		{
		case HC::SHADER_TYPE::VS:
		{
			m_deviceContext->VSSetShader(static_cast<ID3D11VertexShader*>(currShader), nullptr, 0);
		}
		break;
		case HC::SHADER_TYPE::GS:
		{
			m_deviceContext->GSSetShader(static_cast<ID3D11GeometryShader*>(currShader), nullptr, 0);
		}
		break;
		case HC::SHADER_TYPE::HS:
		{
			m_deviceContext->HSSetShader(static_cast<ID3D11HullShader*>(currShader), nullptr, 0);
		}
		break;
		case HC::SHADER_TYPE::DS:
		{
			m_deviceContext->DSSetShader(static_cast<ID3D11DomainShader*>(currShader), nullptr, 0);
		}
		break;
		case HC::SHADER_TYPE::PS:
		{
			m_deviceContext->PSSetShader(static_cast<ID3D11PixelShader*>(currShader), nullptr, 0);
		}
		break;
		case HC::SHADER_TYPE::COUNT:
		default:
			COM_THROW_IF_FAILED(false, "this shader type is not corrected");
			break;
		}
	}

	switch (pipeLine->m_primitive)
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

	COM_THROW_IF_FAILED(pipeLine->GetInputLayoutVector() != nullptr, "inputSample is not attached to pipeline : " + pipeLine->GetPipeLineName());

	auto iter = m_inputLayout.find(pipeLine->GetInputLayoutHash());
	if (iter == m_inputLayout.end())
	{
		CreateInputLayout(pipeLine->GetInputLayoutHash(), pipeLine->GetInputLayoutVector(),
			static_cast<HCDX11Shader*>(pipeLine->m_shaders[static_cast<unsigned int>(HC::SHADER_TYPE::VS)]));

		iter = m_inputLayout.find(pipeLine->GetInputLayoutHash());
	}

	m_deviceContext->IASetInputLayout(iter->second.Get());

	HCDX11VertexBuffer* currBuffer = static_cast<HCDX11VertexBuffer*>(pipeLine->GetVertexBuffer());
	ID3D11Buffer* vertexBuffers[] = { currBuffer->Buffer.Get() };
	const UINT strides[] = { pipeLine->GetInputDataSize() };
	const UINT offsets[] = { 0 };
	m_deviceContext->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);

	if (pipeLine->m_rasterizer)
	{

	}
	else
	{
		m_deviceContext->RSSetState(m_baseRasterizer.Get());
	}

	if (pipeLine->m_depthStencilState)
	{

	}
	else
	{
		m_deviceContext->OMSetDepthStencilState(m_baseDepthStencilState.Get(), 0);
	}

	if (pipeLine->m_blendState)
	{

	}
	else
	{
		m_deviceContext->OMSetBlendState(m_baseBlendState.Get(), NULL, 0xFFFFFFFF);
	}

	ID3D11Buffer* baseCBs[] = { static_cast<ID3D11Buffer*>(m_mainPassCB->GetBuffer()) };

	m_deviceContext->VSSetConstantBuffers(0, 1, baseCBs);
	m_deviceContext->HSSetConstantBuffers(0, 1, baseCBs);
	m_deviceContext->DSSetConstantBuffers(0, 1, baseCBs);
	m_deviceContext->GSSetConstantBuffers(0, 1, baseCBs);
	m_deviceContext->PSSetConstantBuffers(0, 1, baseCBs);

	if (size_t cbufferNum = pipeLine->m_CBuffers.size())
	{
		std::vector<ID3D11Buffer*> buffers;
		for (size_t i = 0; i < cbufferNum; i++)
		{
			buffers.push_back(static_cast<ID3D11Buffer*>(pipeLine->m_CBuffers[i]->GetBuffer()));
		}

		m_deviceContext->VSSetConstantBuffers(1, cbufferNum, &buffers[0]);
		m_deviceContext->HSSetConstantBuffers(1, cbufferNum, &buffers[0]);
		m_deviceContext->DSSetConstantBuffers(1, cbufferNum, &buffers[0]);
		m_deviceContext->GSSetConstantBuffers(1, cbufferNum, &buffers[0]);
		m_deviceContext->PSSetConstantBuffers(1, cbufferNum, &buffers[0]);
	}
}

void HCGraphicDX11::RenderObjects(HCGraphicPipeLine* pipeLine)
{
	HCDX11VertexBuffer* currBuffer = static_cast<HCDX11VertexBuffer*>(pipeLine->GetVertexBuffer());
	auto reservedOBs = pipeLine->GetReservedObjects();
	UINT dataSize = pipeLine->GetInputDataSize();

	size_t numObjects = 0;
	{
		for (auto& it : reservedOBs)
		{
			numObjects += it.size();
		}

		if (numObjects * dataSize > currBuffer->BufferSize)
		{
			//TODO: resize vertex buffer
		}
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	size_t accumulatedByte = 0;
	{
		COM_HRESULT_IF_FAILED(m_deviceContext->Map(currBuffer->Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource),
			"Failed to map constant buffer.");

		for (auto& it2 : reservedOBs)
		{
			size_t bufferSize = it2.size();
			CopyMemory(static_cast<BYTE*>(mappedResource.pData) + accumulatedByte, it2.data(), bufferSize);

			accumulatedByte += bufferSize;
		}
		m_deviceContext->Unmap(currBuffer->Buffer.Get(), 0);
	}

	size_t currOffset = 0;
	for (size_t i = 0; i < reservedOBs.size(); i++)
	{
		if (reservedOBs[i].size())
		{
			ID3D11ShaderResourceView* views[] = { m_textures[i].TextureInfoView.Get(),
												  m_textures[i].TextureView.Get() };

			m_deviceContext->PSSetShaderResources(0, _countof(views), views);
			m_deviceContext->DrawInstanced(static_cast<UINT>(reservedOBs[i].size()), 1, static_cast<UINT>(currOffset), 0);

			currOffset += reservedOBs[i].size();
		}
	}
}

void HCGraphicDX11::RenderFont()
{
	m_deviceContext->GSSetShader(nullptr, nullptr, 0);
	m_font.get()->Render();
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

void HCGraphicDX11::CreateTextures()
{
	std::unordered_map<std::wstring, std::vector<std::wstring>> filePathes;
	StringHelper::SearchAllFileFromDirectory(HC::GO.GRAPHIC.TextureFolderPath, filePathes);

	m_textures.resize(filePathes.size());

	// COM_THROW_IF_FAILED(bufferIndex < 0x0000ffff, "This TextureBuffer is overflow");
	// COM_THROW_IF_FAILED(TextureIndex < 0x0000ffff, "This Texture is overflow");

	UINT currIndex = 0;
	for (auto& it : filePathes)
	{
		ComPtr<ID3D11Texture2D>					arrayTexture2D;
		Texture2DArrayData& texture2DArrayData = m_textures[currIndex];
		UINT									arraySize = static_cast<UINT>(it.second.size());
		UINT									maximumX = 0;
		UINT									maximumY = 0;

		std::vector<ComPtr<ID3D11Texture2D>>	currFolderTextures;
		std::vector<D3D11_TEXTURE2D_DESC>		textureDesces;

		m_textureBufferIndex[it.first] = currIndex++;

		for (auto& it2 : it.second)
		{
			ComPtr<ID3D11Resource> texture = nullptr;
			ComPtr<ID3D11ShaderResourceView> textureView = nullptr;
			D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = {};

			COM_HRESULT_IF_FAILED(DirectX::CreateWICTextureFromFile(m_device.Get(), it2.c_str(), texture.GetAddressOf(), textureView.GetAddressOf()),
				"Texture load Fail");

			textureView->GetDesc(&viewDesc);

			switch (viewDesc.ViewDimension)
			{
			case D3D11_SRV_DIMENSION_TEXTURE2D:
			{
				textureDesces.push_back({});
				auto texture2D = static_cast<ID3D11Texture2D*>(texture.Get());
				texture2D->GetDesc(&textureDesces.back());

				maximumX = (maximumX > textureDesces.back().Width) ? maximumX : textureDesces.back().Width;
				maximumY = (maximumY > textureDesces.back().Height) ? maximumY : textureDesces.back().Height;

				currFolderTextures.push_back(texture2D);
			}
			break;
			default:
				COM_THROW_IF_FAILED(false, "This type texture is not supported");
				break;
			}
		}

		{
			D3D11_TEXTURE2D_DESC textureArrayDesc = {};
			textureArrayDesc.Width = maximumX;
			textureArrayDesc.Height = maximumY;
			textureArrayDesc.ArraySize = arraySize;
			textureArrayDesc.Format = textureDesces.back().Format;
			textureArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			textureArrayDesc.Usage = D3D11_USAGE_DEFAULT;
			textureArrayDesc.SampleDesc.Count = 1;
			textureArrayDesc.MipLevels = 1;

			m_device->CreateTexture2D(&textureArrayDesc, nullptr, arrayTexture2D.GetAddressOf());

			D3D11_SHADER_RESOURCE_VIEW_DESC texArrayViewDesc = {};
			texArrayViewDesc.Format = textureArrayDesc.Format;
			texArrayViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			texArrayViewDesc.Texture2DArray.ArraySize = arraySize;
			texArrayViewDesc.Texture2DArray.MipLevels = 1;
			texArrayViewDesc.Texture2DArray.MostDetailedMip = 0;

			m_device->CreateShaderResourceView(arrayTexture2D.Get(), &texArrayViewDesc, texture2DArrayData.TextureView.GetAddressOf());
		}

		for (size_t i = 0; i < it.second.size(); i++)
		{
			//TODO : Fill texture to maxSizeTexture in step by step 
			UINT currTextureSizeX = textureDesces[i].Width;
			UINT currTextureSizeY = textureDesces[i].Height;
			TextureInTextureData locationData;

			locationData.Index = static_cast<UINT>(i);
			locationData.StartUV = { 0.0f,0.0f };
			locationData.EndUV = { static_cast<float>(currTextureSizeX) / maximumX, static_cast<float>(currTextureSizeY) / maximumY };

			texture2DArrayData.TextureIndex[it.first + L"/" + StringHelper::GetFileNameFromPath(it.second[i])] = i;
			texture2DArrayData.TextureDatas.push_back(locationData);

			D3D11_BOX srcBox = {};
			srcBox.left = 0;
			srcBox.right = currTextureSizeX;
			srcBox.top = 0;
			srcBox.bottom = currTextureSizeY;
			srcBox.front = 0;
			srcBox.back = 1;

			m_deviceContext->CopySubresourceRegion(arrayTexture2D.Get(), static_cast<UINT>(i), 0, 0, 0,
				currFolderTextures[i].Get(), 0, &srcBox);
		}

		{
			ComPtr<ID3D11Buffer> textureInfoBuffer;

			D3D11_BUFFER_DESC bufferDesc = {};
			bufferDesc.ByteWidth = arraySize * sizeof(TextureInTextureData);
			bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.StructureByteStride = sizeof(TextureInTextureData);
			bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = texture2DArrayData.TextureDatas.data();
			data.SysMemPitch = bufferDesc.ByteWidth;
			data.SysMemSlicePitch = data.SysMemPitch;

			m_device->CreateBuffer(&bufferDesc, &data, textureInfoBuffer.GetAddressOf());

			D3D11_SHADER_RESOURCE_VIEW_DESC bufferViewDesc = {};
			bufferViewDesc.Format = DXGI_FORMAT_UNKNOWN;
			bufferViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			bufferViewDesc.Buffer.ElementOffset = 0;
			bufferViewDesc.Buffer.ElementWidth = sizeof(TextureInTextureData);
			bufferViewDesc.Buffer.NumElements = arraySize;

			m_device->CreateShaderResourceView(textureInfoBuffer.Get(), &bufferViewDesc, texture2DArrayData.TextureInfoView.GetAddressOf());
		}

		m_deviceContext->Flush();
	}
}

void HCGraphicDX11::CreateGraphicPipeLineBaseSettings()
{
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

	CreateCB("", sizeof(HC::MainPass), 0, m_mainPassCB);
	ID3D11Buffer* baseCBs[] = { static_cast<ID3D11Buffer*>(m_mainPassCB->GetBuffer()) };

	m_deviceContext->VSSetConstantBuffers(0, 1, baseCBs);
	m_deviceContext->HSSetConstantBuffers(0, 1, baseCBs);
	m_deviceContext->DSSetConstantBuffers(0, 1, baseCBs);
	m_deviceContext->GSSetConstantBuffers(0, 1, baseCBs);
	m_deviceContext->PSSetConstantBuffers(0, 1, baseCBs);
}

void HCGraphicDX11::CreateInputLayout(size_t inputLayoutHash, const std::vector<HCInputLayoutElement>* inputLayoutEle, HCDX11Shader* vs)
{
	ComPtr<ID3D11InputLayout> layout = nullptr;
	std::vector<D3D11_INPUT_ELEMENT_DESC> dx11Elements;

	for (size_t i = 0; i < inputLayoutEle->size(); i++)
	{
		D3D11_INPUT_ELEMENT_DESC temp = {};
		temp.SemanticName = (*inputLayoutEle)[i].SemanticName.c_str();
		temp.SemanticIndex = (*inputLayoutEle)[i].SemanticIndex;
		temp.Format = (*inputLayoutEle)[i].Format;
		if (i == 0)
		{
			temp.AlignedByteOffset = 0;
		}
		else
		{
			temp.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		}

		temp.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		dx11Elements.push_back(temp);
	}

	COM_HRESULT_IF_FAILED(
		m_device->CreateInputLayout(&dx11Elements.front(), static_cast<UINT>(dx11Elements.size()),
			vs->GetCPUData()->GetBufferPointer(), vs->GetCPUData()->GetBufferSize(),
			layout.GetAddressOf()),
		"fail to create inputlayout");

	m_inputLayout[inputLayoutHash] = layout;
}
