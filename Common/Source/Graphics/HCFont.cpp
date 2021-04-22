#include "stdafx.h"
#include "HCFont.h"
#include "Util\StringHelper.h"
#include "GlobalOption.h"


void HCFont::Init(void* device, void* dc)
{
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>((ID3D11DeviceContext*)dc);
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
	if (!m_spriteFonts.empty())
	{
		m_currentFont = m_spriteFonts.begin()->second.get();
	}
}

size_t HCFont::GetFontNum()
{
	return m_spriteFonts.size();
}

void HCFont::GetFontNames(std::vector<std::wstring>& out)
{
	if (!out.empty())
	{
		COM_THROW_IF_FAILED(false, "out parameter vector is not empty");
		return;
	}
	for (auto it = m_spriteFonts.begin(); it != m_spriteFonts.end(); it++)
	{
		out.push_back(it->first);
	}
}

void HCFont::SetFont(unsigned int index)
{
	if (index >= m_spriteFonts.size())
	{
		COM_THROW_IF_FAILED(false, "font index is larger than size");
		if (m_spriteFonts.empty())
		{
			return;
		}
		else
		{
			while (index >= m_spriteFonts.size())
			{
				index -= m_spriteFonts.size();
			}
		}
	}

	auto it = m_spriteFonts.begin();
	for (int i = 0; i < index; i++)
	{
		it++;
	}
	m_currentFont = it->second.get();
}

void HCFont::SetFont(std::wstring fileName)
{
	auto it = m_spriteFonts.find(fileName);
	if (it == m_spriteFonts.end())
	{
		COM_THROW_IF_FAILED(false, "there is no such font name");
		return;
	}
	m_currentFont = m_spriteFonts[fileName].get();
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
		if (m_currentFont)
		{
			m_currentFont->DrawString(m_spriteBatch.get(), m_texts[i].Text.c_str(), m_texts[i].Position, color, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), m_texts[i].Scale);
		}
	}
	m_spriteBatch->End();
}