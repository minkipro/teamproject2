#pragma once
#include "HCGraphic.h"

class DX11FontMG;

class DX11TextData : public IHCTextData
{
	friend class DX11FontMG;

public:
	DX11TextData() = default;
	virtual ~DX11TextData();

	virtual void SetFont(const std::wstring& fontName) override;
	virtual void GetFontNames(std::vector<std::wstring>& out) override;

private:
	static DX11FontMG*	s_fontMG;

	UINT				m_Index = 0;
	void*				m_font = nullptr;
};