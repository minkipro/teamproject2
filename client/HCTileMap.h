#pragma once
#include "HCSceneObject.h"
#include "Graphics/HCInputDataSamples.h"

namespace HC
{
	class TileMap : public HC::SceneObject
	{
	public:
		TileMap(float sizeX, float sizeY, UINT xNum, UINT yNum);
		virtual ~TileMap() override;
		virtual void Update() override;
		virtual void Render() override;

	private:
		std::shared_ptr<IHCResource>	m_renderInfoBuffer;
		const HCMesh*					m_mesh = nullptr;
		UINT							m_tileNum;
		HCTextureData					m_textureData;
	};
}

