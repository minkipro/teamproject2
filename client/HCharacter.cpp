#include "stdafx.h"
#include "HCharacter.h"
#include "Graphics\HCMeshManager.h"

HCharacter::~HCharacter()
{
}

void HCharacter::Init()
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

void HCharacter::Update(float deltaTime)
{
	
}

void HCharacter::Render()
{
	/*m_renderInfoBuffer->Map();
	m_renderInfoBuffer->CopyCpuDataToGpu(&m_renderInfo);
	m_renderInfoBuffer->UnMap();

	auto graphic = HCDEVICE(HCGraphic);

	graphic->SetTexture(m_currAniamtion->GetTextureIndex(), 0);
	graphic->SetShaderResource(m_renderInfoBuffer, 1);

	graphic->Draw(m_mesh);*/
}
