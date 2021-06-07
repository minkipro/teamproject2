#include "stdafx.h"
#include "HCFont.h"
#include "Util\StringHelper.h"
#include "GlobalOption.h"

using namespace DirectX;

void DX11FontMG::Init(void* device, void* dc)
{
	DX11TextData::s_fontMG = this;

	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>((ID3D11DeviceContext*)dc);
	LoadSpriteFonts(device);
}

void DX11FontMG::Render()
{
	m_spriteBatch->Begin();

	for (auto& it : m_texts)
	{
		SpriteFont* spFont = static_cast<SpriteFont*>(it->m_font);
		XMVECTOR	color = XMLoadFloat4(&it->m_color);
		XMVECTOR	pos = XMLoadFloat2(reinterpret_cast<XMFLOAT2*>(&it->m_position));
		XMVECTOR	scale = XMLoadFloat2(&it->m_scale);

		spFont->DrawString(m_spriteBatch.get(), it->m_text.c_str(), pos, color, 0.0f, XMVectorZero(), scale, SpriteEffects::SpriteEffects_None, it->m_position.z);
	}
	
	m_spriteBatch->End();
}

DirectX::SpriteFont* DX11FontMG::GetFont(const std::wstring& fontName)
{
	auto iter = m_spriteFonts.find(fontName);
	COM_THROW_IF_FAILED(iter != m_spriteFonts.end(), fontName + L": is not correct fontName");

	return iter->second.get();
}

void DX11FontMG::GetFontNames(std::vector<std::wstring>& out)
{
	for (auto& it : m_spriteFonts)
	{
		out.push_back(it.first);
	}
}

void DX11FontMG::LoadSpriteFonts(void* device)
{
	std::unordered_map<std::wstring, std::vector<std::wstring>> filePathes;
	StringHelper::SearchAllFileFromDirectory(HC::GO.GRAPHIC.FontFolderPath, filePathes);
	for (auto it : filePathes)
	{
		size_t fileNum = it.second.size();

		for (size_t i = 0; i < fileNum; i++)
		{
			m_spriteFonts[it.second[i]] = std::make_unique<DirectX::SpriteFont>((ID3D11Device*)device, it.second[i].c_str());
		}
	}
}

IHCTextData* DX11FontMG::CreateTextData()
{
	auto newData = new DX11TextData;

	if (!m_indexBuffer.empty())
	{
		m_texts[m_indexBuffer.back()] = newData;
		newData->m_Index = m_indexBuffer.back();

		m_indexBuffer.pop_back();
	}
	else
	{
		newData->m_Index = SizeTTransUINT(m_texts.size());
		m_texts.emplace_back(newData);
	}

	return newData;
}

void DX11FontMG::ReleaseFontData(DX11TextData* fontData)
{
	m_indexBuffer.push_back(fontData->m_Index);
	m_texts[fontData->m_Index] = nullptr;
}