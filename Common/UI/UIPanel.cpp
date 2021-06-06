#include "stdafx.h"
#include "UIPanel.h"

void UIPanel::Init()
{
}

void UIPanel::SetBackGroundColor(const DirectX::XMFLOAT4 color)
{
	m_renderInfo.Color = color;
}

void UIPanel::SetName(const std::wstring& name)
{
}

void UIPanel::Update()
{
	UIObject::Update();
}