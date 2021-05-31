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
	m_textureData = graphic->GetTextureIndex(L"Texture/Pipoya RPG Tileset 16x16/sp_8x249_[Base]BaseChip_pipo.png");

	HC::GRAPHIC_RESOURCE_DESC renderInfoBufferDesc;
	std::vector<HCPointRenderInfo>	renderInfos;

	renderInfoBufferDesc.Type = HC::GRAPHIC_RESOURCE_TYPE::GRAPHIC_RESOURCE_BUFFER;
	renderInfoBufferDesc.UsageType = HC::GRAPHIC_RESOURCE_USAGE_TYPE::GRAPHIC_RESOURCE_USAGE_DEFAULT;
	renderInfoBufferDesc.BindFlags = HC::GRAPHIC_RESOURCE_BIND_SHADERRESOURCE_ALL;
	renderInfoBufferDesc.Flags = HC::GRAPHIC_RESOURCE_FLAG_STRUCTURED_BUFFER;
	renderInfoBufferDesc.Stride = sizeof(HCPointRenderInfo);
	renderInfoBufferDesc.Buffer.StrideNum = m_numX*m_numY;

	renderInfos.resize(renderInfoBufferDesc.Buffer.StrideNum);
	for (UINT i = 0; i < renderInfoBufferDesc.Buffer.StrideNum; i++)
	{
		UINT indexX = i % m_numX;
		UINT indexY = i / m_numY;

		renderInfos[i].Size = { m_sizeX, m_sizeY };
		renderInfos[i].Position = { indexX * m_sizeX, indexY * m_sizeY , 0.6f };
		renderInfos[i].SpriteInfoIndex = m_textureData.spriteStartIndex + i % m_textureData.spriteNum;
	}

	renderInfoBufferDesc.DefaultData = renderInfos.data();
	graphic->CreateResource(renderInfoBufferDesc, m_renderInfoBuffer);
}

void HCTileMap::Update()
{
}

void HCTileMap::Render()
{
	auto graphic = HCDEVICE(HCGraphic);

	graphic->SetTexture(m_textureData.textureIndex, 0);
	graphic->SetShaderResource(m_renderInfoBuffer, 1);

	graphic->DrawInsatance(m_mesh, m_numX*m_numY);
}
