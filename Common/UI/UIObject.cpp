#include "stdafx.h"
#include "UIObject.h"

HCGraphicPipeLine* UIObject::s_graphicPipeLine = nullptr;

void UIObject::CreateUIRenderPipeLine(HCGraphic* device)
{
	if (s_graphicPipeLine == nullptr)
	{
		device->CreateGraphicPipeLine("UIRenderPipeLine", &s_graphicPipeLine);

		IHCShader* vs = nullptr;
		IHCShader* ps = nullptr;
		IHCShader* gs = nullptr;

		device->CreateShader("UIVS", HC::SHADER_TYPE::VS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "VS", &vs);
		device->CreateShader("UIGS", HC::SHADER_TYPE::GS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "GS", &gs);
		device->CreateShader("UIPS", HC::SHADER_TYPE::PS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "PS", &ps);

		s_graphicPipeLine->m_primitive = HC::PRIMITIVE_TOPOLOGY::POINT;
		s_graphicPipeLine->SelectInputSample<RenderPoint>();

		s_graphicPipeLine->SetShader(HC::SHADER_TYPE::VS, vs);
		s_graphicPipeLine->SetShader(HC::SHADER_TYPE::GS, gs);
		s_graphicPipeLine->SetShader(HC::SHADER_TYPE::PS, ps);

		device->NumberingGraphicPipeLineSlot(5, s_graphicPipeLine);
	}
}

void UIObject::Update()
{
	if (m_parent)
	{
		m_renderPoint.Position.x = m_parent->m_pos.x + (m_pos.x - m_renderPoint.Size.x * m_benchUV.x);
		m_renderPoint.Position.y = m_parent->m_pos.y + (m_pos.y - m_renderPoint.Size.y * m_benchUV.y);
		m_renderPoint.Position.z = m_parent->m_pos.z + m_pos.z;
	}

	for (auto& it : m_childs)
	{
		it->Update();
	}
}

void UIObject::UIOn()
{

}

void UIObject::UIOff()
{
}

void UIObject::SetTexture(const std::wstring& path, const DirectX::XMFLOAT2& size)
{
	auto device = HCDEVICE(HCGraphic);
	TextureData texData = device->GetTextureIndex(path);

	m_renderPoint.TextureIndex = texData.textureIndex;
	m_renderPoint.Size = size;
}

void UIObject::AddChild(UIObject* child)
{
	UIObject* rootUIOb = GetRootUIObject();



	for (auto& it : m_childs)
	{
		COM_THROW_IF_FAILED(it != child || it != m_parent, "this child UIObject is already in this object's tree");
	}

	m_childs.push_back(child);
}

void UIObject::SetParent(UIObject* ui)
{
	COM_THROW_IF_FAILED(CheckDependency(ui), "this child UIObject is already in this object's tree");

	m_parent = ui;
}

void UIObject::SetPos(const DirectX::XMFLOAT3& pos)
{
	m_renderPoint.Position = pos;
}

void UIObject::SetSize(const DirectX::XMFLOAT2& size)
{
	m_renderPoint.Size = size;
}

void UIObject::AddFunc(HCColFunc func)
{
	m_ColliderData.ColFuncs.push_back(func);
}

bool UIObject::CheckDependency(UIObject* object)
{
	if (this != object)
	{
		for (auto& it : m_childs)
		{
			if (!it->CheckDependency(object))
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

UIObject* UIObject::GetRootUIObject()
{
	if (m_parent)
	{
		return m_parent->GetRootUIObject();
	}
	else
	{
		return this;
	}
}
