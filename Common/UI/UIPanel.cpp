#include "stdafx.h"
#include "UIPanel.h"

void UIPanel::Init()
{
}

void UIPanel::SetBackGroundColor(const DirectX::XMFLOAT4 color)
{
	m_renderPoint.Color = color;
}

void UIPanel::SetName(const std::wstring& name)
{
	//m_Font->m_Text = name;
}

void UIPanel::Update()
{
	UIObject::Update();

	s_graphicPipeLine->RenderReserveObject(&m_renderPoint, m_renderPoint.TextureIndex);
}