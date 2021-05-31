#include "stdafx.h"
#include "HCTileMap.h"
#include "Graphics\HCMeshManager.h"

using namespace HC;
TileMap::TileMap(float sizeX, float sizeY, UINT xNum, UINT yNum)
	: m_tileNum(xNum* yNum)
{
	auto graphic = HCDEVICE(HCGraphic);

	m_mesh = HCMeshManager::Get()->GetMesh("NormalRect");
	m_textureData = graphic->GetTextureIndex(L"Texture/Pipoya RPG Tileset 16x16/sp_8x249_[Base]BaseChip_pipo.png");

	HC::GRAPHIC_RESOURCE_DESC renderInfoBufferDesc;
	std::vector<HCPointRenderInfo>	renderInfos;

	renderInfoBufferDesc.Type = HC::GRAPHIC_RESOURCE_TYPE::GRAPHIC_RESOURCE_BUFFER;
	renderInfoBufferDesc.UsageType = HC::GRAPHIC_RESOURCE_USAGE_TYPE::GRAPHIC_RESOURCE_USAGE_DEFAULT;
	renderInfoBufferDesc.BindFlags = HC::GRAPHIC_RESOURCE_BIND_SHADERRESOURCE_ALL;
	renderInfoBufferDesc.Flags = HC::GRAPHIC_RESOURCE_FLAG_STRUCTURED_BUFFER;
	renderInfoBufferDesc.Stride = sizeof(HCPointRenderInfo);
	renderInfoBufferDesc.Buffer.StrideNum = m_tileNum;

	renderInfos.resize(m_tileNum);
	for (UINT i = 0; i < m_tileNum; i++)
	{
		UINT indexX = i % xNum;
		UINT indexY = i / yNum;

		renderInfos[i].Size = { sizeX,sizeY };
		renderInfos[i].Position = { indexX * sizeX, indexY * sizeY , 0.6f };
		renderInfos[i].TextureIndex = m_textureData.textureIndex;
		renderInfos[i].SpriteIndex = m_textureData.spriteStartIndex + i % m_textureData.spriteNum;
	}

	renderInfoBufferDesc.DefaultData = renderInfos.data();
	graphic->CreateResource(renderInfoBufferDesc, m_renderInfoBuffer);
}

TileMap::~TileMap()
{
}

void TileMap::Update()
{
}

void HC::TileMap::Render()
{
	auto graphic = HCDEVICE(HCGraphic);

	graphic->SetTexture(m_textureData.textureIndex, 0);
	graphic->SetShaderResource(m_renderInfoBuffer, 1);

	graphic->DrawInsatance(m_mesh, m_tileNum);
}
