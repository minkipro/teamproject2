#pragma once
#include "Scene/HCSceneObject.h"
#include "Graphics/HCInputDataSamples.h"
#include "HC2DAnimation.h"

class HBasicSceneObject : public HCSceneObject
{
public:
	virtual void Init(const wchar_t* textureName) override;
	virtual void Update(float deltaTime) override;
	virtual void Render() override;

	void SetAnimation(int index);

private:
	HCPointRenderInfo				m_renderInfo;
	std::shared_ptr<IHCResource>	m_renderInfoBuffer;
	const HCMesh* m_mesh = nullptr;
	HCTextureData					m_textureData;
	vector<HC2DAnimation>			m_animations;
	int								m_curAnimIndex;
	std::shared_ptr<IHCTextData>	m_textRender;

	XMFLOAT2						m_pos;
};

