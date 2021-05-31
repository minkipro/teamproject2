#pragma once
#include "HCSceneObject.h"
#include "Graphics/HCInputDataSamples.h"

class HCCharacter : public HCSceneObject
{
public:
	HCCharacter();
	virtual ~HCCharacter() override;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;

private:
	HCTextureData			m_textureData;
	HCPointRenderInfo		m_renderInfos;
};

