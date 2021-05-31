#pragma once
#include "HCSceneObject.h"
#include "Graphics/HCInputDataSamples.h"

class HCTileMap : public HCSceneObject
{
public:
	HCTileMap(float sizeX, float sizeY, UINT xNum, UINT yNum);
	virtual ~HCTileMap() override;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;

private:
	std::shared_ptr<IHCResource>	m_renderInfoBuffer;
	const HCMesh*					m_mesh = nullptr;
	float							m_sizeX;
	float							m_sizeY;
	UINT							m_numX;
	UINT							m_numY;
	HCTextureData					m_textureData;
};

