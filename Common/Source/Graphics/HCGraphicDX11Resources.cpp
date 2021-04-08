#include "stdafx.h"
#include "HCGraphicDX11Resources.h"

void* HCDX11Shader::GetShaderData()
{
	return m_ShaderData.Get();
}

void HCDX11TextureBuffer::SetTexture(size_t slot, IHCTexture* texture)
{
	if (m_TextureSlots.size() <= slot)
	{
		m_TextureSlots.insert(m_TextureSlots.end(), m_TextureSlots.size() - (slot + 1), nullptr);
		m_TextureViews.insert(m_TextureViews.end(), m_TextureViews.size() - (slot + 1), nullptr);
	}

	ComPtr<ID3D11ShaderResourceView> temp;

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc = {};
	resourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resourceViewDesc.Texture2D.MipLevels = -1;
	resourceViewDesc.Texture2D.MostDetailedMip = 0;

	COM_HRESULT_IF_FAILED(m_Device->CreateShaderResourceView(
		static_cast<ID3D11Texture2D*>(texture->GetTextureData()), &resourceViewDesc, temp.GetAddressOf()), "fail to create SRV");

	m_TextureViews[slot] = temp;
}

std::vector<ID3D11ShaderResourceView*> HCDX11TextureBuffer::GetTextureViews()
{

	return std::vector<ID3D11ShaderResourceView*>();
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
