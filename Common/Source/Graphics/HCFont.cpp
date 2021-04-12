#include "stdafx.h"
#include "HCFont.h"


void HCFont::Init(void* device, void* dc)
{
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>((ID3D11DeviceContext*)dc);
	m_spriteFont = std::make_unique<DirectX::SpriteFont>((ID3D11Device*)device, L"./../Common/Fonts/comic_sans_ms_16.spritefont");
}

void HCFont::SetText(const IHCFont::TextData& textData)
{
	m_texts.push_back(textData);
}

void HCFont::Render()
{
	m_spriteBatch->Begin();
	size_t textNum = m_texts.size();
	for (size_t i = 0; i < textNum; i++)
	{
		DirectX::XMVECTORF32 color = { m_texts[i].Color.x,m_texts[i].Color.y,m_texts[i].Color.z,1.0f };
		m_spriteFont->DrawString(m_spriteBatch.get(), m_texts[i].Text.c_str(), m_texts[i].Position, color, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), m_texts[i].Scale);
	}
	m_spriteBatch->End();
}