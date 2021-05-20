#pragma once
#include "HCScene.h"
#include "Graphics/HCGraphic.h"
#include "HCSceneObject.h"
#include "Graphics/HCCameraManager.h"

class DevScene final : public HCScene
{
public:
	DevScene();
	virtual ~DevScene() = default;

	virtual void Init() override;
	virtual void Update() override;

private:
	std::vector<HC::SceneObject*> m_sceneObjects;
	HC::CameraManager m_cameraManager;
};