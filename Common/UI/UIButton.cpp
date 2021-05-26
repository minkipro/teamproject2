#include "stdafx.h"
#include "UIButton.h"

void UIButton::Init()
{

}

void UIButton::Update()
{
	if (m_isOn)
	{
		UIObject::Update();

		if (!m_isOnlyFontMode)
		{
			s_graphicPipeLine->RenderReserveObject(&m_renderPoint, m_renderPoint.TextureIndex);
		}
	}
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

void UIButton::ClearFunc()
{
	m_ColliderData.ColFuncs.clear();
}
