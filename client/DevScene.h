#pragma once
#include "HCScene.h"

class DevScene final : public HCScene
{
public:
	DevScene();
	virtual ~DevScene() = default;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;

private:

};