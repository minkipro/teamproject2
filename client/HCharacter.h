#pragma once
#include "HCSceneObject.h"
class HCharacter : public HCSceneObject
{
public:
	virtual ~HCharacter() override;
	virtual void Init() override;
	virtual void Update(float deltaTime) override;
	virtual void Render() override;

	HCPointRenderInfo			m_renderInfo;
	shared_ptr<IHCResource>		m_renderInfoBuffer;
	const HCMesh*				m_mesh = nullptr;
};

