#pragma once
#include "HCGraphic.h"
#include "DX11TextData.h"
#include <SpriteFont.h>

class DX11FontMG
{
public:
	DX11FontMG() = default;
	~DX11FontMG() = default;

	void					Init(void* device, void* dc);
	void					Render();

	DirectX::SpriteFont*	GetFont(const std::wstring& fontName);
	void					GetFontNames(std::vector<std::wstring>& out);

	IHCTextData*			CreateTextData();
	void					ReleaseFontData(DX11TextData* fontData);

private:
	void					LoadSpriteFonts(void* device);

private:
	std::unique_ptr<DirectX::SpriteBatch>									m_spriteBatch;
	std::unordered_map<std::wstring, std::unique_ptr<DirectX::SpriteFont>>	m_spriteFonts;
	std::vector<DX11TextData*>												m_texts;
	std::vector<unsigned int>												m_indexBuffer;
};