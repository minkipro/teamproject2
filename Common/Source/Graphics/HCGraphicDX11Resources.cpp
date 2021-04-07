#include "stdafx.h"
#include "HCGraphicDX11Resources.h"

void* HCDX11Shader::GetShaderData()
{
	return nullptr;
}

HCTexture::~HCTexture()
{
}

void* HCTexture::GetTextureData()
{
	return static_cast<void*>(m_textureView.GetAddressOf());
}

POINT HCTexture::GetTextureSize()
{
	ID3D11Resource* res = 0;
	m_textureView->GetResource(&res);
	ID3D11Texture2D* tex2d = static_cast<ID3D11Texture2D*>(res);
	D3D11_TEXTURE2D_DESC desc;
	tex2d->GetDesc(&desc);
	return { static_cast<LONG>(desc.Width), static_cast<LONG>(desc.Height) };
}

void HCTexture::SetName(const std::string& name_)
{
	m_name = name_;
}
