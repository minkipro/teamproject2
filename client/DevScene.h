#pragma once
#include "Scene/HCScene.h"
#include "Scene/HCSceneObject.h"
#include "Graphics/HCGraphic.h"
#include "Graphics\HCPickingTexture.h"

class DevScene final : public HCScene
{
public:
	DevScene();
	virtual ~DevScene() override;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;

private:
	std::vector<HCSceneObject*>			m_sceneObjects;
	std::shared_ptr<HCGraphicPipeLine>	m_testPipeLine;
	HC::MainPass						m_mainPass;
	std::shared_ptr<IHCResource>		m_mainPassCB;
};