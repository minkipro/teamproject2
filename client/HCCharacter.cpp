#include "stdafx.h"
#include "HCCharacter.h"
#include "Graphics\HCMeshManager.h"

HC2DCharacter::~HC2DCharacter()
{
}

void HC2DCharacter::Init()
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

	m_mesh = HCMeshManager::Get()->GetMesh(typeid(HCOnePointExtToRect).name());
}

void HC2DCharacter::Update(float deltaTime)
{
	if (m_currAniamtion)
	{
		int spriteInfoIndex = m_currAniamtion->Update(deltaTime);
		
		COM_THROW_IF_FAILED(spriteInfoIndex > -1, "current animation don't have spriteData");

		m_renderInfo.SpriteInfoIndex = spriteInfoIndex;
	}

	m_renderInfoBuffer->Map();
	m_renderInfoBuffer->CopyCpuDataToGpu(&m_renderInfo);
	m_renderInfoBuffer->UnMap();
}

void HC2DCharacter::Render()
{
	if (m_currAniamtion)
	{
		auto graphic = HCDEVICE(HCGraphic);

		graphic->SetTexture(m_currAniamtion->GetTextureIndex(), 0);
		graphic->SetShaderResource(m_renderInfoBuffer, 1);

		graphic->Draw(m_mesh);
	}
}

void HC2DCharacter::AddPos(const DirectX::XMFLOAT2& vec)
{
	m_renderInfo.Position.x += vec.x;
	m_renderInfo.Position.y += vec.y;
}

void HC2DCharacter::SetZ(float z)
{
	m_renderInfo.Position.z = z;
}

void HC2DCharacter::SetPos(const DirectX::XMFLOAT2& pos)
{
	m_renderInfo.Position.x = pos.x;
	m_renderInfo.Position.y = pos.y;
}

void HC2DCharacter::SetSize(const DirectX::XMFLOAT2& size)
{
	m_renderInfo.Size = size;
}

void HC2DCharacter::SetAnimation(HC2DAnimation* ani)
{
	if (m_currAniamtion != ani)
	{
		m_currAniamtion = ani;
		ani->Start();
	}
}

