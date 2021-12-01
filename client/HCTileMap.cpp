#include "stdafx.h"
#include "HCTileMap.h"
#include "Graphics\HCMeshManager.h"

HCTileMap::HCTileMap(float sizeX, float sizeY, UINT xNum, UINT yNum)
	: m_sizeX(sizeX)
	, m_sizeY(sizeY)
	, m_numX(xNum)
	, m_numY(yNum)
{
	
}

HCTileMap::~HCTileMap()
{
}

void HCTileMap::Init()
{
	auto graphic = HCDEVICE(HCGraphic);

	m_mesh = HCMeshManager::Get()->GetMesh(typeid(HCOnePointExtToRect).name());
	m_textureData = graphic->GetTextureIndex(L"Texture/sp_cat.png");

	HC::GRAPHIC_RESOURCE_DESC renderInfoBufferDesc;
	HCPointRenderInfo	renderInfos;

	renderInfoBufferDesc.Type = HC::GRAPHIC_RESOURCE_TYPE::GRAPHIC_RESOURCE_BUFFER;
	renderInfoBufferDesc.UsageType = HC::GRAPHIC_RESOURCE_USAGE_TYPE::GRAPHIC_RESOURCE_USAGE_DEFAULT;
	renderInfoBufferDesc.BindFlags = HC::GRAPHIC_RESOURCE_BIND_SHADERRESOURCE_ALL;
	renderInfoBufferDesc.Flags = HC::GRAPHIC_RESOURCE_FLAG_STRUCTURED_BUFFER;
	renderInfoBufferDesc.Stride = sizeof(HCPointRenderInfo);
	renderInfoBufferDesc.Buffer.StrideNum = 1;

	renderInfos.Size = { m_sizeX, m_sizeY };
	renderInfos.Position = { m_sizeX, m_sizeY , 0.6f };
	renderInfos.SpriteInfoIndex = m_textureData.spriteStartIndex;
	

	renderInfoBufferDesc.DefaultData = &renderInfos;
	graphic->CreateResource(renderInfoBufferDesc, m_renderInfoBuffer);
}

void HCTileMap::Update(float deltaTime)
{
}

void HCTileMap::Render()
{
	auto graphic = HCDEVICE(HCGraphic);

	graphic->SetTexture(m_textureData.textureIndex, 0);
	graphic->SetShaderResource(m_renderInfoBuffer, 1);

	graphic->DrawInsatance(m_mesh, 1);
}
