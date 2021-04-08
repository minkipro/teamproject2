#pragma once
#include <d3d11.h>
#include <wrl/client.h>

#include "HCGraphic.h"

using Microsoft::WRL::ComPtr;

class HCDX11Shader final : public IHCShader
{
public:
	HCDX11Shader(ID3D11DeviceChild* data)
		:m_ShaderData(data)
	{
	}

	virtual ~HCDX11Shader() = default;

	virtual void* GetShaderData() override;

private:
	ComPtr<ID3D11DeviceChild> m_ShaderData;
};

class HCDX11Texture : public IHCTexture, ID3D11Texture2D
{
public:
	HCDX11Texture( ) {}

	virtual ~HCDX11Texture() = default;

	virtual void* GetTextureData();
	virtual POINT GetTextureSize();
};

class IHCDX11ConstBuffer: public IHCCBuffer, ID3D11Buffer
{
public:
	IHCDX11ConstBuffer() {}
	virtual ~IHCDX11ConstBuffer() = default;

	virtual void CopyData(size_t stride, size_t num, const void* data) override;

protected:

};

class HCDX11TextureBuffer final : public IHCTextureBuffer
{
public:
	HCDX11TextureBuffer(ID3D11Device* device)
		: m_Device(device)
	{
	}
	virtual ~HCDX11TextureBuffer() = default;

	virtual void SetTexture(size_t slot, IHCTexture* texture);

	std::vector<ID3D11ShaderResourceView*> GetTextureViews();

class HCTexture : public IHCTexture
{
public:
	HCTexture() {}

	virtual ~HCTexture();

	virtual void*	GetTextureData() override;
	virtual POINT	GetTextureSize() override;
	virtual void	SetName(const std::string& name_) override;

private:
	std::string m_name;
	ComPtr<ID3D11ShaderResourceView> m_textureView;
};
//class IHCTextureBuffer
//{
//public:
//	IHCTextureBuffer() {}
//	virtual ~IHCTextureBuffer() = default;
//
//	virtual void SetTexture(size_t slot, IHCTexture* texture) = 0;
//	IHCTexture* GetTexture(size_t slot) { return m_TextureSlots[slot]; }
//
//protected:
//	std::vector<IHCTexture*> m_TextureSlots;
//};
private:
	ID3D11Device*									m_Device;
	std::vector<ComPtr<ID3D11ShaderResourceView>>	m_TextureViews;
};