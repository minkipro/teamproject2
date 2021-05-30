#include "stdafx.h"
#include "HCGraphicDX11Resources.h"

ID3D11DeviceContext* HCDX11Resource::s_deviceContext = nullptr;

void* HCDX11Shader::GetShaderData()
{
	return m_shaderData.Get();
}

HCDX11Resource::HCDX11Resource(std::shared_ptr<ID3D11Resource> resource, std::shared_ptr<ID3D11View> view, const HC::GRAPHIC_RESOURCE_DESC& desc)
{
	m_resource = resource;
	m_resourceView = view;

	ZeroMemory(&m_sub, sizeof(m_sub));
	m_desc = desc;
}

HCDX11Resource::HCDX11Resource(std::shared_ptr<ID3D11Resource> resource, const HC::GRAPHIC_RESOURCE_DESC& desc)
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
		COM_HRESULT_IF_FAILED(s_deviceContext->Map(m_resource.get(), 0, D3D11_MAP::D3D11_MAP_WRITE, 0, &m_sub), "fail to map");
	}
		break;
	case HC::GRAPHIC_RESOURCE_USAGE_TYPE::GRAPHIC_RESOURCE_USAGE_STAGING:
	{
		COM_HRESULT_IF_FAILED(s_deviceContext->Map(m_resource.get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &m_sub), "fail to map");
	}
		break;
	default:
		COM_THROW_IF_FAILED(false, "this usage type buffer not support to cpu read/write");
		break;
	}
}

void HCDX11Resource::UnMap()
{
	m_sub.pData = nullptr;
	s_deviceContext->Unmap(m_resource.get(), 0);
}

void HCDX11Resource::CpuDataCopyToGpu(void* data, size_t byteSize, size_t byteOffset)
{
	COM_THROW_IF_FAILED(m_desc.UsageType == HC::GRAPHIC_RESOURCE_USAGE_TYPE::GRAPHIC_RESOURCE_USAGE_DYNAMIC, "this resource can't copy data to gpu");
	COM_THROW_IF_FAILED(m_sub.pData, "this resource is not mapped");

	BYTE* mappedData = static_cast<BYTE*>(m_sub.pData);
	CopyMemory((mappedData +byteOffset), data, byteSize);
}

void HCDX11Resource::CpuDataCopyToGpu(void* data, size_t offsetSrtide)
{
	COM_THROW_IF_FAILED(m_desc.UsageType == HC::GRAPHIC_RESOURCE_USAGE_TYPE::GRAPHIC_RESOURCE_USAGE_DYNAMIC, "this resource can't copy data to gpu");
	COM_THROW_IF_FAILED(m_sub.pData, "this resource is not mapped");

	BYTE* mappedData = static_cast<BYTE*>(m_sub.pData);
	CopyMemory(mappedData + (m_desc.Stride* offsetSrtide), data, m_desc.Stride);
}

void HCDX11Resource::GpuDataCopyToCpu(const RECT& rect, std::vector<std::vector<BYTE>>& out)
{
	COM_THROW_IF_FAILED(m_desc.UsageType == HC::GRAPHIC_RESOURCE_USAGE_TYPE::GRAPHIC_RESOURCE_USAGE_STAGING, "this resource can't copy data to cpu");
	COM_THROW_IF_FAILED(m_sub.pData, "this resource is not mapped");

	const BYTE* mappedData = static_cast<BYTE*>(m_sub.pData);
	size_t dataStrid = m_desc.Stride;
	size_t rowPitch = m_sub.RowPitch;
	size_t dataHeight = (static_cast<size_t>(rect.bottom) - rect.top) * dataStrid;
	size_t datawidth = (static_cast<size_t>(rect.right) - rect.left) * dataStrid;

	if (out.size() != dataHeight)
	{
		out.resize(dataHeight);
	}

	if (out.front().size() != datawidth)
	{
		for (auto& it : out)
		{
			it.resize(datawidth);
		}
	}

	const BYTE* currDataPtr = mappedData + (rect.left * dataStrid) + (rowPitch * rect.top);
	for (auto& it : out)
	{
		CopyMemory(it.data(), currDataPtr, datawidth);
		currDataPtr += rowPitch;
	}
}

void HCDX11Resource::SetDeviceContext(ID3D11DeviceContext* context)
{
	s_deviceContext = context;
}
