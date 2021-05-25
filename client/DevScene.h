#pragma once
#include "HCScene.h"
#include "Graphics/HCGraphic.h"
#include "HCSceneObject.h"

class DevScene final : public HCScene
{
public:
	DevScene();
	virtual ~DevScene() override;

	virtual void Init() override;
	virtual void Update() override;

private:
	std::vector<HC::SceneObject*> m_sceneObjects;
};