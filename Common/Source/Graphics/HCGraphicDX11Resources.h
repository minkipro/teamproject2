#pragma once
#include <d3d11.h>
#include <wrl/client.h>

#include "HCGraphic.h"

using Microsoft::WRL::ComPtr;

class HCDX11Shader : public IHCShader
{
public:
	HCDX11Shader()
	{
	}

	virtual ~HCDX11Shader() = default;

	virtual void* GetShaderData() override;
};


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