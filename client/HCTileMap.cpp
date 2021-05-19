#include "stdafx.h"
#include "HCTileMap.h"
using namespace HC;
TileMap::TileMap()
{
	/*auto graphic = HCDEVICE(HCGraphic);
	HCGraphicPipeLine* pipeLine;
	graphic->GetGraphicPipeLine("testPipe2", &pipeLine);
	for (int i = 0; i < TILESIZEX; i++)
	{
		for (int j = 0; j < TILESIZEY; j++)
		{
			m_renderPointUV[i][j].Size = { 100,100 };
			m_renderPointUV[i][j].Position = { i*100, j*100 , 0.4f};
			m_renderPointUV[i][j].TextureIndex = graphic->GetTextureIndex(L"Texture/Pipoya RPG Tileset 16x16/[Base]BaseChip_pipo.png");
			m_renderPointUV[i][j].Uv = { 0.0f, 0.0f, 1.0f / 8.0f, 1.0f / 249.0f };
			pipeLine->RenderReserveObject(&m_renderPointUV[i][j]);
		}
	}*/
}

TileMap::~TileMap()
{
}

void TileMap::Update()
{

}
