#pragma once
#include <d3d11.h>
#include <wrl/client.h>

#include "HCGraphic.h"

using Microsoft::WRL::ComPtr;

struct HCDX11VertexBuffer
{
	ComPtr<ID3D11Buffer>	Buffer;
	size_t					BufferSize = 0;
};

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

template<class T>
class IHCDX11ConstBuffer final : public IHCCBuffer
{
public:
	IHCDX11ConstBuffer() = delete;
	IHCDX11ConstBuffer(const IHCDX11ConstBuffer& rhs) = delete;
	IHCDX11ConstBuffer(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~IHCDX11ConstBuffer() = default;

	virtual void		CopyData(const void* data) override;
	virtual void*		GetBuffer() override;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_buffer;
	ID3D11DeviceContext*					m_deviceContext = nullptr;
};

template<class T>
inline IHCDX11ConstBuffer<T>::IHCDX11ConstBuffer(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	m_deviceContext = deviceContext;

	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));
	desc.StructureByteStride = 0;

	COM_HRESULT_IF_FAILED(device->CreateBuffer(&desc, 0, m_buffer.GetAddressOf()),
		"fail to create CBBuffer");
}

template<class T>
inline void IHCDX11ConstBuffer<T>::CopyData(const void* data)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	COM_HRESULT_IF_FAILED(m_deviceContext->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)
		, "Failed to map constant buffer.");
	
	CopyMemory(mappedResource.pData, data, sizeof(T));
	
	m_deviceContext->Unmap(m_buffer.Get(), 0);
}

template<class T>
inline void* IHCDX11ConstBuffer<T>::GetBuffer()
{
	return m_buffer.Get();
}