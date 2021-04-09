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

class HCDX11Texture : public IHCTexture
{
public:
	HCDX11Texture(ID3D11ShaderResourceView* view) : m_textureView(view){}

	virtual ~HCDX11Texture();

	virtual void* GetTextureData() override;
	virtual POINT	GetTextureSize() override;

private:
	std::string m_name;
	ComPtr<ID3D11ShaderResourceView> m_textureView;
};

class IHCDX11ConstBuffer: public IHCCBuffer
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

private:
	ID3D11Device*									m_Device;
	std::vector<ComPtr<ID3D11ShaderResourceView>>	m_TextureViews;
};