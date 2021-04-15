#pragma once
#include "HCScene.h"
#include "Graphics/HCGraphic.h"

class DevScene final : public HCScene
{
public:
	DevScene();
	virtual ~DevScene() = default;

	virtual void Init() override;
	virtual void Update() override;

private:
	RenderVertexSkeleton m_test;
};