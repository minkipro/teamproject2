#include "stdafx.h"
#include "UIButton.h"

void UIButton::Init()
{

}

void UIButton::Update()
{
	if (!m_isOnlyFontMode)
	{

	}

	UIObject::Update();
}

void UIButton::SetTexture(const std::wstring& path, const DirectX::XMFLOAT2& size)
{
	UIObject::SetTexture(path, size);

	m_isOnlyFontMode = true;
}

void UIButton::OnlyFontMode()
{
	m_isOnlyFontMode = true;
}

void UIButton::SetText(const std::wstring& text)
{

}

void UIButton::SetTextHeight(int height)
{

}