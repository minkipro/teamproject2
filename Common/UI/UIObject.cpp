#include "stdafx.h"
#include "UIObject.h"

std::shared_ptr<HCGraphicPipeLine> UIObject::s_graphicPipeLine = nullptr;

void UIObject::CreateUIRenderPipeLine(HCGraphic* device)
{
	if (s_graphicPipeLine == nullptr)
	{
		s_graphicPipeLine = std::make_shared<HCGraphicPipeLine>();

		std::shared_ptr<IHCShader> vs = nullptr;
		std::shared_ptr<IHCShader> ps = nullptr;
		std::shared_ptr<IHCShader> gs = nullptr;

		device->CreateShader("UIVS", HC::SHADER_TYPE::HCSHADER_VS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "VS", vs);
		device->CreateShader("UIGS", HC::SHADER_TYPE::HCSHADER_GS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "GS", gs);
		device->CreateShader("UIPS", HC::SHADER_TYPE::HCSHADER_PS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "PS", ps);

		s_graphicPipeLine->m_primitive = HC::PRIMITIVE_TOPOLOGY::POINT;
		s_graphicPipeLine->SelectRenderInfo<HCPointRenderInfo>();

		s_graphicPipeLine->SetShader(HC::SHADER_TYPE::HCSHADER_VS, vs);
		s_graphicPipeLine->SetShader(HC::SHADER_TYPE::HCSHADER_GS, gs);
		s_graphicPipeLine->SetShader(HC::SHADER_TYPE::HCSHADER_PS, ps);

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

void UIObject::Render()
{

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
	HCTextureData texData = device->GetTextureIndex(path);

	m_renderPoint.TextureIndex = texData.textureIndex;
	m_renderPoint.Size = size;
}

void UIObject::AddChild(UIObject* child)
{
	COM_THROW_IF_FAILED(GetRootUIObject()->CheckDependency(child), "this child UIObject is already in this object's tree");

	m_childs.push_back(child);
}

void UIObject::SetParent(UIObject* object)
{
	COM_THROW_IF_FAILED(CheckDependency(object), "this UIObject is already in this object's tree");

	m_parent = object;
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
