#pragma once
#include <d3d11.h>
#include <wrl/client.h>

#include "HCGraphic.h"

using Microsoft::WRL::ComPtr;

class HCDX11Shader final : public IHCShader
{
public:
	HCDX11Shader(ID3D11DeviceChild* data)
		:m_shaderData(data)
	{
	}
	HCDX11Shader(ID3D11DeviceChild* data, ID3DBlob* cpuData)
		:m_shaderData(data)
		,m_cpuData(cpuData)
	{
	}

	virtual ~HCDX11Shader() = default;

	virtual void*	GetShaderData() override;
	ID3DBlob*		GetCPUData() { return m_cpuData.Get(); }

private:
	ComPtr<ID3D11DeviceChild>	m_shaderData;
	ComPtr<ID3DBlob>			m_cpuData;
};

class HCDX11Resource final : public IHCResource
{
public:
	HCDX11Resource() = delete;
	HCDX11Resource(const HCDX11Resource& rhs) = delete;
	HCDX11Resource(ID3D11Resource* resource, ID3D11View* view, const HC::GRAPHIC_RESOURCE_DESC& desc);
	HCDX11Resource(ID3D11Resource* resource, const HC::GRAPHIC_RESOURCE_DESC& desc);
	virtual ~HCDX11Resource() = default;

	virtual void*	GetResource() override { return m_resource.Get(); }
	virtual void*	GetResourceView() override { return m_resourceView.Get(); }

	virtual void	Map() override;
	virtual void	UnMap() override;
	virtual void	CpuDataCopyToGpu(void* oneStrideData) override;
	virtual void	CpuDataCopyToGpu(void* data, size_t byteSize, size_t byteOffset) override;
	virtual void	CpuDataCopyToGpu(void* data, size_t offsetStride) override;
	virtual void	GpuDataCopyToCpu(const RECT& rect, std::vector<std::vector<BYTE>>& out) override;

	static void		SetDeviceContext(ID3D11DeviceContext* context);

private:
	static ID3D11DeviceContext*		s_deviceContext;

	ComPtr<ID3D11Resource>			m_resource;
	ComPtr<ID3D11View>				m_resourceView;

	D3D11_MAPPED_SUBRESOURCE		m_sub;
	UINT							m_currIndex = 0;
};