#pragma once

class HCScene
{
public:
	HCScene();
	virtual ~HCScene() = default;

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;

private:

};