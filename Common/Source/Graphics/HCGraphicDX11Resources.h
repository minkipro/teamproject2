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
		:m_ShaderData(data)
	{
	}
	HCDX11Shader(ID3D11DeviceChild* data, ID3DBlob* cpuData)
		:m_ShaderData(data)
		,m_CpuData(cpuData)
	{
	}

	virtual ~HCDX11Shader() = default;

	virtual void*	GetShaderData() override;
	ID3DBlob*		GetCPUData() { return m_CpuData.Get(); }

private:
	ComPtr<ID3D11DeviceChild>	m_ShaderData;
	ComPtr<ID3DBlob>			m_CpuData;
};

class HCDX11Texture final : public IHCTexture
{
public:
	HCDX11Texture(ID3D11ShaderResourceView* view) : m_textureView(view){}

	virtual ~HCDX11Texture();

	virtual void*	GetTextureData() override;
	virtual POINT	GetTextureSize() override;

private:
	ComPtr<ID3D11ShaderResourceView> m_textureView;
};

class HCDX11TextureBuffer final : public HCTextureBuffer
{
public:
	HCDX11TextureBuffer()
	{
	}
	virtual ~HCDX11TextureBuffer() = default;

	virtual void									SetTexture(size_t slot, IHCTexture* texture);
	const std::vector<ID3D11ShaderResourceView*>&	GetTextureViews();

private:
	std::vector<ID3D11ShaderResourceView*>			m_TextureViews;
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
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_Buffer;
	ID3D11DeviceContext*					m_DeviceContext = nullptr;
};

template<class T>
inline IHCDX11ConstBuffer<T>::IHCDX11ConstBuffer(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	m_DeviceContext = deviceContext;

	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));
	desc.StructureByteStride = 0;

	COM_HRESULT_IF_FAILED(device->CreateBuffer(&desc, 0, m_Buffer.GetAddressOf()),
		"fail to create CBBuffer");
}

template<class T>
inline void IHCDX11ConstBuffer<T>::CopyData(const void* data)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	COM_HRESULT_IF_FAILED(m_DeviceContext->Map(m_Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)
		, "Failed to map constant buffer.");
	
	CopyMemory(mappedResource.pData, data, sizeof(T));
	
	m_DeviceContext->Unmap(m_Buffer.Get(), 0);
}

template<class T>
inline void* IHCDX11ConstBuffer<T>::GetBuffer()
{
	return m_Buffer.Get();
}