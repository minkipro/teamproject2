#include "stdafx.h"
#include "UIObject.h"
#include "Graphics\HCMeshManager.h"

std::shared_ptr<HCGraphicPipeLine> UIObject::s_graphicPipeLine = nullptr;
const HCMesh* UIObject::s_mesh = nullptr;

void UIObject::CreateUIRenderPipeLine(HCGraphic* device)
{
	if (s_graphicPipeLine == nullptr)
	{
		auto graphic = HCDEVICE(HCGraphic);
		s_graphicPipeLine = std::make_shared<HCGraphicPipeLine>();
		s_mesh = HCMeshManager::Get()->GetMesh(typeid(HCOnePointExtToRect).name());

		std::shared_ptr<IHCShader> vs = nullptr;
		std::shared_ptr<IHCShader> ps = nullptr;
		std::shared_ptr<IHCShader> gs = nullptr;

		s_graphicPipeLine->SetVertexType(typeid(HCOnePointExtToRect).hash_code(), sizeof(HCOnePointExtToRect), &HCOnePointExtToRect::InputLayout);

		graphic->CreateShader(HC::SHADER_TYPE::HCSHADER_VS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "VS", s_graphicPipeLine->m_shaders[HC::SHADER_TYPE::HCSHADER_VS]);
		graphic->CreateShader(HC::SHADER_TYPE::HCSHADER_GS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "GS", s_graphicPipeLine->m_shaders[HC::SHADER_TYPE::HCSHADER_GS]);
		graphic->CreateShader(HC::SHADER_TYPE::HCSHADER_PS, L"./../Common/Shader/PointToPlaneSahder.hlsl", "PS", s_graphicPipeLine->m_shaders[HC::SHADER_TYPE::HCSHADER_PS]);

		s_graphicPipeLine->m_primitive = HC::PRIMITIVE_TOPOLOGY::POINT;
	}
}

void UIObject::Init()
{
	auto graphic = HCDEVICE(HCGraphic);

	HC::GRAPHIC_RESOURCE_DESC renderInfoBufferDesc;
	renderInfoBufferDesc.Type = HC::GRAPHIC_RESOURCE_TYPE::GRAPHIC_RESOURCE_BUFFER;
	renderInfoBufferDesc.UsageType = HC::GRAPHIC_RESOURCE_USAGE_TYPE::GRAPHIC_RESOURCE_USAGE_DYNAMIC;
	renderInfoBufferDesc.BindFlags = HC::GRAPHIC_RESOURCE_BIND_SHADERRESOURCE_ALL;
	renderInfoBufferDesc.Flags = HC::GRAPHIC_RESOURCE_FLAG_STRUCTURED_BUFFER;
	renderInfoBufferDesc.Stride = sizeof(HCPointRenderInfo);
	renderInfoBufferDesc.Buffer.StrideNum = 1;

	graphic->CreateResource(renderInfoBufferDesc, m_renderInfoBuffer);
}

void UIObject::Update()
{
	if (m_parent)
	{
		m_renderInfo.Position.x = m_parent->m_renderInfo.Position.x + (m_pos.x - m_renderInfo.Size.x * m_benchUV.x);
		m_renderInfo.Position.y = m_parent->m_renderInfo.Position.y + (m_pos.y - m_renderInfo.Size.y * m_benchUV.y);
		m_renderInfo.Position.z = m_parent->m_renderInfo.Position.z + m_pos.z;
	}
	else
	{
		m_renderInfo.Position.x = (m_pos.x - m_renderInfo.Size.x * m_benchUV.x);
		m_renderInfo.Position.y = (m_pos.y - m_renderInfo.Size.y * m_benchUV.y);
		m_renderInfo.Position.z = m_pos.z;
	}

	for (auto& it : m_childs)
	{
		it->Update();
	}

	m_renderInfoBuffer->Map();
	m_renderInfoBuffer->CopyCpuDataToGpu(&m_renderInfo);
	m_renderInfoBuffer->UnMap();
}

void UIObject::Render()
{
	if (m_isOn)
	{
		auto graphic = HCDEVICE(HCGraphic);

		graphic->SetTexture(m_textureData.textureIndex, 0);
		graphic->SetShaderResource(m_renderInfoBuffer, 1);

		graphic->Draw(s_mesh);

		for (auto& it : m_childs)
		{
			it->Render();
		}
	}
}

void UIObject::SetTexture(const std::wstring& path, const DirectX::XMFLOAT2& size)
{
	auto device = HCDEVICE(HCGraphic);
	m_textureData = device->GetTextureIndex(path);
	m_renderInfo.Size = size;
}

void UIObject::SetTexture(const HCTextureData& textureData, const DirectX::XMFLOAT2& size)
{
	m_textureData = textureData;
	m_renderInfo.Size = size;
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
	m_renderInfo.Position = pos;
}

void UIObject::SetSize(const DirectX::XMFLOAT2& size)
{
	m_renderInfo.Size = size;
}

void UIObject::ClearFunc()
{

}

UINT UIObject::GetUINum()
{
	UINT result = 1;
	
	for (auto& it : m_childs)
	{
		result += it->GetUINum();
	}

	return result;
}

void UIObject::AddFunc(HCColFunc func)
{
	//m_colliderData.ColFuncs.push_back(func);
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
