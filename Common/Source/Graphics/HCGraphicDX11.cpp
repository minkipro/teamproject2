#include "stdafx.h"
#include <algorithm>
#include "HCGraphicDX11.h"
#include "GlobalOption.h"
#include <DirectXColors.h>
#include <D3Dcompiler.h>
#include <WICTextureLoader.h>
#include "Util\StringHelper.h"
#include "HCFont.h"
#include "HCCameraManager.h"
#include "Xml\Xml.h"

using Microsoft::WRL::ComPtr;

HCGraphicDX11::~HCGraphicDX11()
{
}

void HCGraphicDX11::Init()
{
	m_swapchain = std::make_unique<HCSwapchainDX11>();
	m_swapchain->Init(HC::GO.GRAPHIC.NumSwapchainBuffer,
		m_windowHandle, m_device.GetAddressOf(), m_deviceContext.GetAddressOf());

	HCDX11Resource::SetDeviceContext(m_deviceContext.Get());

	m_swapchain->Resize(HC::GO.WIN.WindowsizeX, HC::GO.WIN.WindowsizeY);

	CreateBaseSamplers();
	CreateTextures();
	CreateGraphicPipeLineBaseSettings();

	m_font.Init((void*)m_device.Get(), (void*)m_deviceContext.Get());
}

void HCGraphicDX11::Update()
{
	
}

void HCGraphicDX11::CreateResource(const std::string& resourceName, const HC::GRAPHIC_RESOURCE_DESC& desc, std::shared_ptr<IHCResource>& out)
{
	out = nullptr;

	HC::GRAPHIC_RESOURCE_DESC				resultDesc = desc;
	ID3D11Resource*							result = nullptr;
	ID3D11View*								resultView = nullptr;
	std::unique_ptr<D3D11_SUBRESOURCE_DATA>	subResource = nullptr;
	
	switch (resultDesc.Type)
	{
	case HC::GRAPHIC_RESOURCE_TYPE::GRAPHIC_RESOURCE_CONSTANT_BUFFER:
	{
		COM_THROW_IF_FAILED(resultDesc.Stride % 16 == 0, "constantBuffer should be a magnification of 16 bytes");

		resultDesc.UsageType = HC::GRAPHIC_RESOURCE_USAGE_TYPE::GRAPHIC_RESOURCE_USAGE_DYNAMIC;
		resultDesc.BindFlags = static_cast<HC::GRAPHIC_RESOURCE_BIND_FLAGS>((resultDesc.BindFlags & HC::GRAPHIC_RESOURCE_BIND_SHADERRESOURCE_ALL) | HC::GRAPHIC_RESOURCE_BIND_FLAGS::GRAPHIC_RESOURCE_BIND_CONSTANT_BUFFER);
		resultDesc.Flags = HC::GRAPHIC_RESOURCE_FLAG_NONE;

		D3D11_BUFFER_DESC bufferDesc = {};

		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = GetResourceBindFlags(resultDesc);
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.ByteWidth = resultDesc.Stride;
		bufferDesc.StructureByteStride = 0;

		COM_HRESULT_IF_FAILED(m_device->CreateBuffer(&bufferDesc, subResource.get(), reinterpret_cast<ID3D11Buffer**>(&result)), "Failed to create buffer.");
	}
	break;
	case HC::GRAPHIC_RESOURCE_TYPE::GRAPHIC_RESOURCE_STRUCTURED_BUFFER:
	{
		COM_THROW_IF_FAILED(resultDesc.Stride % 16 == 0, "structuredSize should be a magnification of 16 bytes");

		D3D11_BUFFER_DESC				bufferDesc = {};
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = {};

		bufferDesc.Usage = GetResourceUsage(resultDesc);
		bufferDesc.BindFlags = GetResourceBindFlags(resultDesc);
		bufferDesc.CPUAccessFlags = GetResourceCpuAcessFlags(resultDesc);
		bufferDesc.MiscFlags = GetResourceMiscFlags(resultDesc) | D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDesc.ByteWidth = resultDesc.Stride * resultDesc.Buffer.StrideNum;
		bufferDesc.StructureByteStride = resultDesc.Stride;

		viewDesc.Format = DXGI_FORMAT_UNKNOWN;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		viewDesc.Buffer.ElementOffset = 0;
		viewDesc.Buffer.NumElements = resultDesc.Buffer.StrideNum;

		if (resultDesc.DefaultData)
		{
			subResource = std::make_unique<D3D11_SUBRESOURCE_DATA>();
			subResource->pSysMem = resultDesc.DefaultData;
			subResource->SysMemPitch = bufferDesc.ByteWidth;
			subResource->SysMemSlicePitch = 0;
		}

		COM_HRESULT_IF_FAILED(m_device->CreateBuffer(&bufferDesc, subResource.get(), reinterpret_cast<ID3D11Buffer**>(&result)), "Failed to create buffer.");
		COM_HRESULT_IF_FAILED(m_device->CreateShaderResourceView(result, &viewDesc, reinterpret_cast<ID3D11ShaderResourceView**>(&resultView)), "Failed to create buffer view.");
	}
	break;
	case HC::GRAPHIC_RESOURCE_TYPE::GRAPHIC_RESOURCE_TEXTURE2D:
	{
		D3D11_TEXTURE2D_DESC			texture2dDesc = {};
		D3D11_SHADER_RESOURCE_VIEW_DESC	viewDesc = {};

		texture2dDesc.Usage = GetResourceUsage(resultDesc);
		texture2dDesc.BindFlags = GetResourceBindFlags(resultDesc);
		texture2dDesc.CPUAccessFlags = GetResourceCpuAcessFlags(resultDesc);
		texture2dDesc.MiscFlags = GetResourceMiscFlags(resultDesc);
		texture2dDesc.Format = resultDesc.Texture.Format;
		texture2dDesc.MipLevels = 1;
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.Width = resultDesc.Texture.Size.x;
		texture2dDesc.Height = resultDesc.Texture.Size.y;
		texture2dDesc.ArraySize = resultDesc.Texture.ArrayNum;

		viewDesc.Format = resultDesc.Texture.Format;

		if (resultDesc.DefaultData)
		{
			subResource = std::make_unique<D3D11_SUBRESOURCE_DATA>();
			subResource->pSysMem = resultDesc.DefaultData;
			subResource->SysMemPitch = resultDesc.Stride* resultDesc.Texture.Size.x;
			subResource->SysMemSlicePitch = 0;
		}

		COM_HRESULT_IF_FAILED(m_device->CreateTexture2D(&texture2dDesc, subResource.get(), reinterpret_cast<ID3D11Texture2D**>(&result)), "Failed to create texture.");

		if (texture2dDesc.ArraySize > 1)
		{
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			viewDesc.Texture2DArray.ArraySize = resultDesc.Texture.ArrayNum;
			viewDesc.Texture2DArray.MipLevels = 1;
			viewDesc.Texture2DArray.MostDetailedMip = 0;
		}
		else
		{
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			viewDesc.Texture2D.MipLevels = 1;
			viewDesc.Texture2D.MostDetailedMip = 0;
		}

		COM_HRESULT_IF_FAILED(m_device->CreateShaderResourceView(result, &viewDesc, reinterpret_cast<ID3D11ShaderResourceView**>(&resultView)), "Failed to create texture.");
	}
	break;
	default:
		COM_THROW_IF_FAILED(false, "this type is not supported type");
		break;
	}

	out = std::make_shared<HCDX11Resource>(std::shared_ptr<ID3D11Resource>(result), std::shared_ptr<ID3D11View>(resultView), resultDesc);
}

void HCGraphicDX11::CreateShader(const std::string& shaderName, HC::SHADER_TYPE type, const std::wstring& filePath, const std::string& entryPoint, std::shared_ptr<IHCShader>& out)
{
	out = nullptr;
	std::string target;

	switch (type)
	{
	case HC::SHADER_TYPE::HCSHADER_VS:
	{
		target = "vs";
	}
	break;
	case HC::SHADER_TYPE::HCSHADER_GS:
	{
		target = "gs";
	}
	break;
	case HC::SHADER_TYPE::HCSHADER_HS:
	{
		target = "hs";
	}
	break;
	case HC::SHADER_TYPE::HCSHADER_DS:
	{
		target = "ds";
	}
	break;
	case HC::SHADER_TYPE::HCSHADER_PS:
	{
		target = "ps";
	}
	break;
	case HC::SHADER_TYPE::HCSHADER_COUNT:
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
	case HC::SHADER_TYPE::HCSHADER_VS:
	{
		ComPtr<ID3D11VertexShader> temp = nullptr;
		m_device->CreateVertexShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &temp);
		out = std::make_shared<HCDX11Shader>(temp.Get(), byteCode.Get());
	}
	break;
	case HC::SHADER_TYPE::HCSHADER_GS:
	{
		ComPtr<ID3D11GeometryShader> temp = nullptr;
		m_device->CreateGeometryShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &temp);
		out = std::make_shared<HCDX11Shader>(temp.Get());
	}
	break;
	case HC::SHADER_TYPE::HCSHADER_HS:
	{
		ComPtr<ID3D11HullShader> temp = nullptr;
		m_device->CreateHullShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &temp);
		out = std::make_shared<HCDX11Shader>(temp.Get());
	}
	break;
	case HC::SHADER_TYPE::HCSHADER_DS:
	{
		ComPtr<ID3D11DomainShader> temp = nullptr;
		m_device->CreateDomainShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &temp);
		out = std::make_shared<HCDX11Shader>(temp.Get());
	}
	break;
	case HC::SHADER_TYPE::HCSHADER_PS:
	{
		ComPtr<ID3D11PixelShader> temp = nullptr;
		m_device->CreatePixelShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &temp);
		out = std::make_shared<HCDX11Shader>(temp.Get());
	}
	break;
	default:
		assert(false);
	break;
	}
	
}

void HCGraphicDX11::CreateTextData(std::shared_ptr<IHCTextData>& out)
{
	out = nullptr;
	out = std::shared_ptr<IHCTextData>(m_font.CreateTextData());
}

void HCGraphicDX11::CopyResource(std::shared_ptr<IHCResource> dest, const std::shared_ptr<IHCResource> src)
{
	m_deviceContext->CopyResource(reinterpret_cast<ID3D11Resource*>(dest->GetResource()), reinterpret_cast<ID3D11Resource*>(src->GetResource()));
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
	if (m_currPipeLine == pipeLine)
	{
		return;
	}

	m_currPipeLine = pipeLine;

	for (unsigned int i = 0; i < static_cast<unsigned int>(HC::SHADER_TYPE::HCSHADER_COUNT); i++)
	{
		void* currShader = nullptr;

		if (pipeLine->m_shaders[i])
		{
			currShader = pipeLine->m_shaders[i]->GetShaderData();
		}

		switch (static_cast<HC::SHADER_TYPE>(i))
		{
		case HC::SHADER_TYPE::HCSHADER_VS:
		{
			m_deviceContext->VSSetShader(static_cast<ID3D11VertexShader*>(currShader), nullptr, 0);
		}
		break;
		case HC::SHADER_TYPE::HCSHADER_GS:
		{
			m_deviceContext->GSSetShader(static_cast<ID3D11GeometryShader*>(currShader), nullptr, 0);
		}
		break;
		case HC::SHADER_TYPE::HCSHADER_HS:
		{
			m_deviceContext->HSSetShader(static_cast<ID3D11HullShader*>(currShader), nullptr, 0);
		}
		break;
		case HC::SHADER_TYPE::HCSHADER_DS:
		{
			m_deviceContext->DSSetShader(static_cast<ID3D11DomainShader*>(currShader), nullptr, 0);
		}
		break;
		case HC::SHADER_TYPE::HCSHADER_PS:
		{
			m_deviceContext->PSSetShader(static_cast<ID3D11PixelShader*>(currShader), nullptr, 0);
		}
		break;
		case HC::SHADER_TYPE::HCSHADER_COUNT:
		default:
			COM_THROW_IF_FAILED(false, "this shader type is not corrected");
			break;
		}
	}

	m_deviceContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(pipeLine->m_primitive));

	auto iter = m_vertexLayout.find(pipeLine->GetVertexTypeHash());
	if (iter == m_vertexLayout.end())
	{
		CreateInputLayout(pipeLine->GetVertexTypeHash(), pipeLine->GetVertexInputLayout(),
			static_cast<HCDX11Shader*>(pipeLine->m_shaders[HC::SHADER_TYPE::HCSHADER_VS].get()));

		iter = m_vertexLayout.find(pipeLine->GetVertexTypeHash());
	}

	m_deviceContext->IASetInputLayout(iter->second.Get());

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
}

void HCGraphicDX11::SetTexture(UINT textureIndex, UINT shaderResourceSlot)
{
	COM_THROW_IF_FAILED(shaderResourceSlot < 20, "max shaderResourceSlot is 19");

	ID3D11ShaderResourceView* view = m_textures[m_allSpriteDatas[textureIndex].TextureIndex].TextureView.Get();
	m_deviceContext->VSSetShaderResources(shaderResourceSlot, 1, &view);
	m_deviceContext->DSSetShaderResources(shaderResourceSlot, 1, &view);
	m_deviceContext->HSSetShaderResources(shaderResourceSlot, 1, &view);
	m_deviceContext->GSSetShaderResources(shaderResourceSlot, 1, &view);
	m_deviceContext->PSSetShaderResources(shaderResourceSlot, 1, &view);
}

void HCGraphicDX11::SetShaderResource(std::shared_ptr<IHCResource> resource, UINT shaderResourceSlot)
{
	COM_THROW_IF_FAILED(shaderResourceSlot < 20, "max shaderResourceSlot is 19");
	COM_THROW_IF_FAILED(resource->GetDesc().Type != HC::GRAPHIC_RESOURCE_TYPE::GRAPHIC_RESOURCE_CONSTANT_BUFFER, "this resource type is constantBuffer");

	ID3D11ShaderResourceView* view = static_cast<ID3D11ShaderResourceView*>(resource->GetResourceView());
	m_deviceContext->VSSetShaderResources(shaderResourceSlot, 1, &view);
	m_deviceContext->DSSetShaderResources(shaderResourceSlot, 1, &view);
	m_deviceContext->HSSetShaderResources(shaderResourceSlot, 1, &view);
	m_deviceContext->GSSetShaderResources(shaderResourceSlot, 1, &view);
	m_deviceContext->PSSetShaderResources(shaderResourceSlot, 1, &view);
}

void HCGraphicDX11::SetShaderResources(const std::vector<std::shared_ptr<IHCResource>>& resources, UINT shaderResourceStartSlot)
{
	COM_THROW_IF_FAILED(shaderResourceStartSlot + resources.size() - 1 < 20, "max shaderResourceSlot is 19");

	static std::vector<ID3D11ShaderResourceView*> views = {};
	views.clear();

	for (auto& it : resources)
	{
#ifdef _DEBUG
		COM_THROW_IF_FAILED(it->GetDesc().Type != HC::GRAPHIC_RESOURCE_TYPE::GRAPHIC_RESOURCE_CONSTANT_BUFFER, "this resource type is constantBuffer");
#endif
		views.push_back(static_cast<ID3D11ShaderResourceView*>(it->GetResourceView()));
	}

	m_deviceContext->VSSetShaderResources(shaderResourceStartSlot, views.size(), views.data());
	m_deviceContext->DSSetShaderResources(shaderResourceStartSlot, views.size(), views.data());
	m_deviceContext->HSSetShaderResources(shaderResourceStartSlot, views.size(), views.data());
	m_deviceContext->GSSetShaderResources(shaderResourceStartSlot, views.size(), views.data());
	m_deviceContext->PSSetShaderResources(shaderResourceStartSlot, views.size(), views.data());
}

void HCGraphicDX11::SetConstantBuffer(std::shared_ptr<IHCResource> buffer, UINT constantBufferSlot)
{
	COM_THROW_IF_FAILED(buffer->GetDesc().Type == HC::GRAPHIC_RESOURCE_TYPE::GRAPHIC_RESOURCE_CONSTANT_BUFFER, "this resource type is not constantBuffer");

	ID3D11Buffer* currBuffer = static_cast<ID3D11Buffer*>(buffer->GetResource());
	m_deviceContext->VSSetConstantBuffers(constantBufferSlot, 1, &currBuffer);
	m_deviceContext->DSSetConstantBuffers(constantBufferSlot, 1, &currBuffer);
	m_deviceContext->HSSetConstantBuffers(constantBufferSlot, 1, &currBuffer);
	m_deviceContext->GSSetConstantBuffers(constantBufferSlot, 1, &currBuffer);
	m_deviceContext->PSSetConstantBuffers(constantBufferSlot, 1, &currBuffer);
}

void HCGraphicDX11::SetConstantBuffers(const std::vector<std::shared_ptr<IHCResource>>& buffers, UINT constantBufferStartSlot)
{
	static std::vector<ID3D11Buffer*> currBuffers = {};
	currBuffers.clear();

	for (auto& it : buffers)
	{
#ifdef _DEBUG
		COM_THROW_IF_FAILED(it->GetDesc().Type == HC::GRAPHIC_RESOURCE_TYPE::GRAPHIC_RESOURCE_CONSTANT_BUFFER, "this resource type is not constantBuffer");
#endif
		currBuffers.push_back(static_cast<ID3D11Buffer*>(it->GetResource()));
	}

	m_deviceContext->VSSetConstantBuffers(constantBufferStartSlot, currBuffers.size(), currBuffers.data());
	m_deviceContext->DSSetConstantBuffers(constantBufferStartSlot, currBuffers.size(), currBuffers.data());
	m_deviceContext->HSSetConstantBuffers(constantBufferStartSlot, currBuffers.size(), currBuffers.data());
	m_deviceContext->GSSetConstantBuffers(constantBufferStartSlot, currBuffers.size(), currBuffers.data());
	m_deviceContext->PSSetConstantBuffers(constantBufferStartSlot, currBuffers.size(), currBuffers.data());
}

void HCGraphicDX11::Draw(const HCMesh* mesh)
{
	UINT pipeLineVertexSize = m_currPipeLine->GetVertexSize();
	UINT vertexOffset = 0;
	auto& vertexBufferDesc = mesh->VertexBuffer->GetDesc();
	auto vertexBuffer = static_cast<ID3D11Buffer*>(mesh->VertexBuffer->GetResource());

	COM_THROW_IF_FAILED(pipeLineVertexSize == vertexBufferDesc.Stride, "this mesh vertex size is not match to currPipeLine");
	COM_THROW_IF_FAILED(mesh->Primitive == m_currPipeLine->m_primitive, "this mesh primitive is not match to currPipeLine");

	m_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &pipeLineVertexSize, &vertexOffset);
	m_deviceContext->Draw(mesh->VertexCount, mesh->VertexOffset);
}

void HCGraphicDX11::DrawInsatance(const HCMesh* mesh, UINT numInstance, UINT InstanceOffset)
{
	UINT pipeLineVertexSize = m_currPipeLine->GetVertexSize();
	UINT vertexOffset = 0;
	auto& vertexBufferDesc = mesh->VertexBuffer->GetDesc();
	auto vertexBuffer = static_cast<ID3D11Buffer*>(mesh->VertexBuffer->GetResource());

	COM_THROW_IF_FAILED(pipeLineVertexSize == vertexBufferDesc.Stride, "this mesh vertex size is not match to currPipeLine");
	COM_THROW_IF_FAILED(mesh->Primitive == m_currPipeLine->m_primitive, "this mesh primitive is not match to currPipeLine");

	m_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &pipeLineVertexSize, &vertexOffset);
	m_deviceContext->DrawInstanced(mesh->VertexCount, numInstance, mesh->VertexOffset, InstanceOffset);
}

void HCGraphicDX11::DrawIndexed(const HCMesh* mesh)
{
	UINT pipeLineVertexSize = m_currPipeLine->GetVertexSize();
	UINT vertexOffset = 0;
	auto& vertexBufferDesc = mesh->VertexBuffer->GetDesc();
	auto vertexBuffer = static_cast<ID3D11Buffer*>(mesh->VertexBuffer->GetResource());

	COM_THROW_IF_FAILED(pipeLineVertexSize == vertexBufferDesc.Stride, "this mesh vertex size is not match to currPipeLine");
	COM_THROW_IF_FAILED(mesh->Primitive == m_currPipeLine->m_primitive, "this mesh primitive is not match to currPipeLine");

	m_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &pipeLineVertexSize, &vertexOffset);
	m_deviceContext->IASetIndexBuffer(static_cast<ID3D11Buffer*>(mesh->IndexBuffer->GetResource()), DXGI_FORMAT_R32_UINT, 0);
	m_deviceContext->DrawIndexed(mesh->IndexCount, mesh->IndexOffset, 0);
}

void HCGraphicDX11::DrawIndexedInsatance(const HCMesh* mesh, UINT numInstance, UINT InstanceOffset)
{
	UINT pipeLineVertexSize = m_currPipeLine->GetVertexSize();
	UINT vertexOffset = 0;
	auto& vertexBufferDesc = mesh->VertexBuffer->GetDesc();
	auto vertexBuffer = static_cast<ID3D11Buffer*>(mesh->VertexBuffer->GetResource());

	COM_THROW_IF_FAILED(pipeLineVertexSize == vertexBufferDesc.Stride, "this mesh vertex size is not match to currPipeLine");
	COM_THROW_IF_FAILED(mesh->Primitive == m_currPipeLine->m_primitive, "this mesh primitive is not match to currPipeLine");

	m_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &pipeLineVertexSize, &vertexOffset);
	m_deviceContext->IASetIndexBuffer(static_cast<ID3D11Buffer*>(mesh->IndexBuffer->GetResource()), DXGI_FORMAT_R32_UINT, 0);
	m_deviceContext->DrawIndexedInstanced(mesh->IndexCount, numInstance, mesh->IndexOffset, 0, InstanceOffset);
}

void HCGraphicDX11::DrawFont()
{
	m_deviceContext->GSSetShader(nullptr, nullptr, 0);
	m_deviceContext->DSSetShader(nullptr, nullptr, 0);
	m_deviceContext->HSSetShader(nullptr, nullptr, 0);
	m_font.Render();
}

HCTextureData HCGraphicDX11::GetTextureIndex(const std::wstring& textureName) const
{
	HCTextureData result;
	std::wstring directory = StringHelper::GetDirectoryFromPath(textureName);

	if (directory.length() == 0)
	{
		directory = StringHelper::GetFileNameFromPath(HC::GO.GRAPHIC.TextureFolderPath);
	}

	auto indexBufferIter = m_textureIndex.find(directory);
	COM_THROW_IF_FAILED(indexBufferIter != m_textureIndex.end(), "This TextureBuffer is not loaded");

	result.textureIndex = indexBufferIter->second;
	result.spriteNum = m_textures[result.textureIndex].SpriteNum;

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

			DirectX::XMMATRIX orthoP = DirectX::XMMatrixOrthographicOffCenterLH(
				0.0f, static_cast<float>(HC::GO.WIN.WindowsizeX),
				static_cast<float>(HC::GO.WIN.WindowsizeY), 0,
				D3D11_MIN_DEPTH, D3D11_MAX_DEPTH);
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

	std::vector<TextureSpriteData> spriteDatas;
	UINT currIndex = 0;
	UINT currTextureIndex = 0;
	for (auto& it : filePathes)
	{
		for (auto& it2 : it.second)
		{
			m_textureIndex[it2] = currIndex;

			std::wstring textureName = StringHelper::GetFileNameFromPath(it2);
			TextureResourceData currTextureData;
			TextureSpriteData locationData;
			locationData.TextureIndex = currTextureIndex++;

			COM_HRESULT_IF_FAILED(DirectX::CreateWICTextureFromFile(m_device.Get(), it2.c_str(), nullptr, currTextureData.TextureView.GetAddressOf()),
				"Texture load Fail");

			currTextureData.TextureView->GetDesc(&currTextureData.TextureDesc);
			COM_THROW_IF_FAILED(currTextureData.TextureDesc.ViewDimension== D3D11_SRV_DIMENSION_TEXTURE2D, "This type texture is not supported");

			std::wstring postString = std::wstring(textureName.c_str(), HC::GO.GRAPHIC.SpriteTextureSuffix.length());
			if (postString == HC::GO.GRAPHIC.SpriteTextureSuffix)
			{
				spriteDatas.clear();
				GetSpriteData(it2, &spriteDatas);
				currTextureData.SpriteNum = SizeTTransUINT(spriteDatas.size());

				for (auto& it2 : spriteDatas)
				{
					locationData.StartUV = { it2.StartUV.x,  it2.StartUV.y };
					locationData.EndUV = { it2.EndUV.x,  it2.EndUV.y };

					m_allSpriteDatas.push_back(locationData);
					currIndex++;
				}
			}
			else
			{
				locationData.StartUV = { 0.0f,0.0f };
				locationData.EndUV = { 1.0f,1.0f };

				m_allSpriteDatas.push_back(locationData);
				currIndex++;
			}

			m_textures.push_back(currTextureData);
		}
	}

	{
		ComPtr<ID3D11Buffer> textureInfoBuffer;

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = m_allSpriteDatas.size() * sizeof(TextureSpriteData);
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.StructureByteStride = sizeof(TextureSpriteData);
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = m_allSpriteDatas.data();
		data.SysMemPitch = bufferDesc.ByteWidth;
		data.SysMemSlicePitch = data.SysMemPitch;

		m_device->CreateBuffer(&bufferDesc, &data, textureInfoBuffer.GetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC bufferViewDesc = {};
		bufferViewDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufferViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		bufferViewDesc.Buffer.ElementOffset = 0;
		bufferViewDesc.Buffer.ElementWidth = sizeof(TextureSpriteData);
		bufferViewDesc.Buffer.NumElements = m_allSpriteDatas.size();

		m_device->CreateShaderResourceView(textureInfoBuffer.Get(), &bufferViewDesc, m_textureInfoView.GetAddressOf());
	}

	m_deviceContext->Flush();
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

	ID3D11ShaderResourceView* textureInfoView = m_textureInfoView.Get();
	m_deviceContext->PSSetShaderResources(20, 1, &textureInfoView);
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
	
	m_vertexLayout[inputLayoutHash] = layout;
}

D3D11_USAGE HCGraphicDX11::GetResourceUsage(const HC::GRAPHIC_RESOURCE_DESC& desc)
{
	return static_cast<D3D11_USAGE>(desc.UsageType);
}

D3D11_CPU_ACCESS_FLAG HCGraphicDX11::GetResourceCpuAcessFlags(const HC::GRAPHIC_RESOURCE_DESC& desc)
{
	static const D3D11_CPU_ACCESS_FLAG flags[static_cast<UINT>(HC::GRAPHIC_RESOURCE_USAGE_TYPE::GRAPHIC_RESOURCE_USAGE_STAGING) + 1] =
	{
		static_cast<D3D11_CPU_ACCESS_FLAG>(0),
		static_cast<D3D11_CPU_ACCESS_FLAG>(0),
		static_cast<D3D11_CPU_ACCESS_FLAG>(D3D11_CPU_ACCESS_WRITE),
		static_cast<D3D11_CPU_ACCESS_FLAG>(D3D11_CPU_ACCESS_READ),

		/*D3D11_USAGE_DEFAULT,
		D3D11_USAGE_IMMUTABLE,
		D3D11_USAGE_DYNAMIC,
		D3D11_USAGE_STAGING*/
	};

	return flags[static_cast<UINT>(desc.UsageType)];
}

D3D11_RESOURCE_MISC_FLAG HCGraphicDX11::GetResourceMiscFlags(const HC::GRAPHIC_RESOURCE_DESC& desc)
{
	return D3D11_RESOURCE_MISC_FLAG(0);
}

D3D11_BIND_FLAG HCGraphicDX11::GetResourceBindFlags(const HC::GRAPHIC_RESOURCE_DESC& desc)
{
	UINT result = 0;
	HC::GRAPHIC_RESOURCE_BIND_FLAGS currBindFlag = desc.BindFlags;

	if (currBindFlag & HC::GRAPHIC_RESOURCE_BIND_VERTEX_BUFFER)
	{
		result |= D3D11_BIND_VERTEX_BUFFER;
	}

	if (currBindFlag & HC::GRAPHIC_RESOURCE_BIND_INDEX_BUFFER)
	{
		result |= D3D11_BIND_INDEX_BUFFER;
	}

	if (currBindFlag & HC::GRAPHIC_RESOURCE_BIND_CONSTANT_BUFFER)
	{
		result |= D3D11_BIND_CONSTANT_BUFFER;
	}

	if (currBindFlag & HC::GRAPHIC_RESOURCE_BIND_SHADERRESOURCE_ALL)
	{
		result |= D3D11_BIND_SHADER_RESOURCE;
	}

	if (currBindFlag & HC::GRAPHIC_RESOURCE_BIND_STREAM_OUTPUT)
	{
		result |= D3D11_BIND_STREAM_OUTPUT;
	}

	if (currBindFlag & HC::GRAPHIC_RESOURCE_BIND_RENDER_TARGET)
	{
		result |= D3D11_BIND_RENDER_TARGET;
	}

	if (currBindFlag & HC::GRAPHIC_RESOURCE_BIND_DEPTH_STENCIL)
	{
		result |= D3D11_BIND_DEPTH_STENCIL;
	}

	if (currBindFlag & HC::GRAPHIC_RESOURCE_BIND_UNORDERED_ACCESS)
	{
		result |= D3D11_BIND_UNORDERED_ACCESS;
	}

	return D3D11_BIND_FLAG(result);
}

void HCGraphicDX11::GetSpriteData(const std::wstring& texturePath, std::vector<TextureSpriteData>* out)
{
	std::wstring textureName = StringHelper::GetFileNameFromPath(texturePath);
	std::unique_ptr<Xml::XMLDocument> document(new XmlDocument);

	document->LoadFile((std::string(StringHelper::WideToString(texturePath), texturePath.find_last_of(L'.', 0)) + ".xml").c_str());

	if (document->Error())
	{
		//don't has SpriteData.xml
		//ex)) sp_5x5_textureName

		size_t gridInfoTextStartIndex = HC::GO.GRAPHIC.SpriteTextureSuffix.length();
		size_t gridInfoTextEndIndex = textureName.find_first_of(L'_', HC::GO.GRAPHIC.SpriteTextureSuffix.length());
		size_t gridInfoSplitIndex= textureName.find_first_of(L'x');

		COM_THROW_IF_FAILED((gridInfoTextEndIndex != std::wstring::npos) && (gridInfoSplitIndex != std::wstring::npos),
			textureName + L"this Texture name has not sprtie grid info");
		
		int sizeX = _wtoi(std::wstring(textureName.c_str() + gridInfoTextStartIndex, gridInfoSplitIndex - gridInfoTextStartIndex).c_str());
		int sizeY = _wtoi(std::wstring(textureName.c_str() + gridInfoSplitIndex + 1, gridInfoTextEndIndex - (gridInfoSplitIndex + 1)).c_str());
		float offsetX = 1.0f / sizeX;
		float offsetY = 1.0f / sizeY;

		TextureSpriteData spData;
		for (int y = 0; y < sizeY; y++)
		{
			spData.StartUV.y = y * offsetY;
			spData.EndUV.y = (y + 1) * offsetY;

			for (int x = 0; x < sizeX; x++)
			{
				spData.StartUV.x = x * offsetX;
				spData.EndUV.x = (x + 1) * offsetX;

				out->push_back(spData);
			}
		}
	}
	else
	{
		XmlElement* list;
		XmlElement* element;

		list = document->FirstChildElement("Sprites");
		element = list->FirstChildElement();

		TextureSpriteData spData;
		for (; element != nullptr; element = element->NextSiblingElement())
		{
			spData.StartUV.x = element->FloatAttribute("StartUV_X");
			spData.StartUV.y = element->FloatAttribute("StartUV_Y");
			spData.EndUV.x = element->FloatAttribute("EndUV_X");
			spData.EndUV.y = element->FloatAttribute("EndUV_Y");

			out->push_back(spData);
		}
	}
}