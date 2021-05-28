#pragma once
#include "HCSceneObject.h"
#include "Graphics/HCInputDataSamples.h"

namespace HC
{
	class TileMap : public HC::SceneObject
	{
	public:
		TileMap(float sizeX, float sizeY, int xNum, int yNum);
		virtual ~TileMap() override;
		virtual void Update() override;
		virtual void Render(HCGraphicPipeLine* pipeLine) override;

	private:
		std::vector<std::vector<RenderPoint>> m_renderPoint;
		int m_spriteNum;
	};
}

