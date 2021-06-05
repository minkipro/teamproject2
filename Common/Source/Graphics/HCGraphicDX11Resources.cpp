#include "stdafx.h"
#include "HCGraphicDX11Resources.h"

ID3D11DeviceContext* HCDX11Resource::s_deviceContext = nullptr;

void* HCDX11Shader::GetShaderData()
{
	return m_shaderData.Get();
}

HCDX11Resource::HCDX11Resource(ID3D11Resource* resource, ID3D11View* shaderResource_cb_view, ID3D11View* renderTargetView, ID3D11View* depthStencilView, const HC::GRAPHIC_RESOURCE_DESC& desc)
{
	m_resource = resource;
	m_resourceView = shaderResource_cb_view;
	m_renderTargetView = renderTargetView;
	m_depthStencilView = depthStencilView;

	ZeroMemory(&m_sub, sizeof(m_sub));
	m_desc = desc;
}

HCDX11Resource::HCDX11Resource(ID3D11Resource* resource, const HC::GRAPHIC_RESOURCE_DESC& desc)
{
	m_resource = resource;

	ZeroMemory(&m_sub, sizeof(m_sub));
	m_desc = desc;
}

void HCDX11Resource::Map()
{
	switch (m_desc.UsageType)
	{
	case HC::GRAPHIC_RESOURCE_USAGE_TYPE::GRAPHIC_RESOURCE_USAGE_DYNAMIC:
	{
		COM_HRESULT_IF_FAILED(s_deviceContext->Map(m_resource.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &m_sub), "fail to map");
	}
		break;
	case HC::GRAPHIC_RESOURCE_USAGE_TYPE::GRAPHIC_RESOURCE_USAGE_STAGING:
	{
		COM_HRESULT_IF_FAILED(s_deviceContext->Map(m_resource.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &m_sub), "fail to map");
	}
		break;
	default:
		COM_THROW_IF_FAILED(false, "this usage type buffer not support to cpu read/write");
		break;
	}

	m_currIndex = 0;
}

void HCDX11Resource::UnMap()
{
	m_sub.pData = nullptr;
	s_deviceContext->Unmap(m_resource.Get(), 0);
}

void HCDX11Resource::CopyCpuDataToGpu(void* oneStrideData)
{
	COM_THROW_IF_FAILED(m_desc.UsageType == HC::GRAPHIC_RESOURCE_USAGE_TYPE::GRAPHIC_RESOURCE_USAGE_DYNAMIC, "this resource can't copy data to gpu");
	COM_THROW_IF_FAILED(m_sub.pData, "this resource is not mapped");

	BYTE* mappedData = static_cast<BYTE*>(m_sub.pData);
	CopyMemory(mappedData + (static_cast<size_t>(m_desc.Stride) * static_cast<size_t>(m_currIndex)), oneStrideData, m_desc.Stride);
	m_currIndex++;
}

void HCDX11Resource::CopyCpuDataToGpu(void* data, size_t byteSize, size_t byteOffset)
{
	COM_THROW_IF_FAILED(m_desc.UsageType == HC::GRAPHIC_RESOURCE_USAGE_TYPE::GRAPHIC_RESOURCE_USAGE_DYNAMIC, "this resource can't copy data to gpu");
	COM_THROW_IF_FAILED(m_sub.pData, "this resource is not mapped");

	BYTE* mappedData = static_cast<BYTE*>(m_sub.pData);
	CopyMemory((mappedData +byteOffset), data, byteSize);
}

void HCDX11Resource::CopyCpuDataToGpu(void* data, size_t offsetSrtide)
{
	COM_THROW_IF_FAILED(m_desc.UsageType == HC::GRAPHIC_RESOURCE_USAGE_TYPE::GRAPHIC_RESOURCE_USAGE_DYNAMIC, "this resource can't copy data to gpu");
	COM_THROW_IF_FAILED(m_sub.pData, "this resource is not mapped");

	BYTE* mappedData = static_cast<BYTE*>(m_sub.pData);
	CopyMemory(mappedData + (m_desc.Stride* offsetSrtide), data, m_desc.Stride);
}

BYTE* HCDX11Resource::GetMappedDataPtr()
{
	COM_THROW_IF_FAILED(m_sub.pData, "this resource is not mapped");

	return  static_cast<BYTE*>(m_sub.pData);
}

UINT HCDX11Resource::GetMappedDataRowPitch()
{
	return m_sub.RowPitch;
}

void HCDX11Resource::SetDeviceContext(ID3D11DeviceContext* context)
{
	s_deviceContext = context;
}
