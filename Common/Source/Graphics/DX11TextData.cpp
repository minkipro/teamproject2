#include "stdafx.h"
#include "DX11TextData.h"
#include "HCFont.h"

DX11FontMG* DX11TextData::s_fontMG = nullptr;

void DX11TextData::Release()
{
	s_fontMG->ReleaseFontData(this);
}

void DX11TextData::SetFont(const std::wstring& fontName)
{
	m_font = s_fontMG->GetFont(fontName);
}

void DX11TextData::GetFontNames(std::vector<std::wstring>& out)
{
	s_fontMG->GetFontNames(out);
}