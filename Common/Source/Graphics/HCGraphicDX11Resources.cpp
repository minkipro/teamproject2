#include "stdafx.h"
#include "HCGraphicDX11Resources.h"

void* HCDX11Shader::GetShaderData()
{
	return m_ShaderData.Get();
}

HCDX11Texture::~HCDX11Texture()
{
}

void* HCDX11Texture::GetTextureData()
{
	return static_cast<void*>(m_textureView.Get());
}

POINT HCDX11Texture::GetTextureSize()
{
	ComPtr<ID3D11Resource> res = 0;
	ID3D11Texture2D* tex2d = nullptr;
	D3D11_TEXTURE2D_DESC desc = {};

	m_textureView->GetResource(res.GetAddressOf());
	tex2d = static_cast<ID3D11Texture2D*>(res.Get());
	tex2d->GetDesc(&desc);
	return { static_cast<LONG>(desc.Width), static_cast<LONG>(desc.Height) };
}

void HCDX11TextureBuffer::SetTexture(size_t slot, IHCTexture* texture)
{
	if (m_TextureSlots.size() <= slot)
	{
		m_TextureSlots.insert(m_TextureSlots.end(), (m_TextureSlots.size() - slot) + 1, nullptr);
		m_TextureViews.insert(m_TextureViews.end(), (m_TextureViews.size() - slot) + 1, nullptr);
	}

	m_TextureSlots[slot] = texture;
	m_TextureViews[slot] = static_cast<ID3D11ShaderResourceView*>(texture->GetTextureData());
}

const std::vector<ID3D11ShaderResourceView*>& HCDX11TextureBuffer::GetTextureViews()
{
	return m_TextureViews;
}
