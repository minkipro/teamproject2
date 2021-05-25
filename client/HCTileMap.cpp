#include "stdafx.h"
#include "HCTileMap.h"
using namespace HC;
TileMap::TileMap(float sizeX, float sizeY, int xNum, int yNum)
{
	auto graphic = HCDEVICE(HCGraphic);
	HCGraphicPipeLine* pipeLine;
	graphic->GetGraphicPipeLine("testPipe", &pipeLine);

	TextureData textureData = graphic->GetTextureIndex(L"Texture/Pipoya RPG Tileset 16x16/sp_8x249_[Base]BaseChip_pipo.png");
	m_spriteNum = textureData.spriteNum;
	m_renderPoint.resize(xNum);
	for (int i = 0; i < xNum; i++)
	{
		m_renderPoint[i].resize(yNum);
		for (int j = 0; j < yNum; j++)
		{
			m_renderPoint[i][j].Size = { sizeX,sizeY };
			m_renderPoint[i][j].Position = { i * 128.0f, j * 128.0f , 0.6f };
			
			
			m_renderPoint[i][j].TextureIndex = textureData.textureIndex + (i + j * 100) % m_spriteNum;
			
		}
	}
}

TileMap::~TileMap()
{
}

void TileMap::Update()
{
	auto graphic = HCDEVICE(HCGraphic);


	HCGraphicPipeLine* pipeLine;
	graphic->GetGraphicPipeLine("testPipe", &pipeLine);

	size_t xSize = m_renderPoint.size();
	if (xSize == 0)
	{
		COM_THROW_IF_FAILED(false, "tileMap is empty");
		return;
	}
	size_t ySize = m_renderPoint[0].size();

	for (size_t i = 0; i < xSize; i++)
	{
		for (size_t j = 0; j < ySize; j++)
		{
			pipeLine->RenderReserveObject(&m_renderPoint[i][j], m_renderPoint[i][j].TextureIndex);
		}
	}
	
}
