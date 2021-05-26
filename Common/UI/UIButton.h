#pragma once
#include "UIObject.h"
#include <string>
#include <functional>

class UIButton :public UIObject
{
public:
	UIButton()
		: m_isOnlyFontMode(false)
	{

	}
	virtual ~UIButton() = default;

	virtual void	Init() override;
	virtual void	Update() override;
	virtual void	SetTexture(const std::wstring& path, const DirectX::XMFLOAT2& size);

	void			SetText(const std::wstring& text);
	void			SetTextHeight(int height);

	void			ClearFunc();
	void			OnlyFontMode();

private:
	bool m_isOnlyFontMode;
};