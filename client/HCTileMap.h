#pragma once
#include "HCSceneObject.h"
#include "Graphics/HCInputDataSamples.h"
#define TILESIZEX 100
#define TILESIZEY 100

namespace HC
{
	class TileMap : public HC::SceneObject
	{
	public:
		TileMap();
		virtual ~TileMap() override;
		virtual void Update() override;

	private:
		RenderPoint m_renderPoint[TILESIZEX][TILESIZEY];
	};
}

