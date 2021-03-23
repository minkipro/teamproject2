#pragma once
#include "WindowContainer.h"
#include "Scene.h"
class Engine : WindowContainer
{
private:
	Scene scene;
public:
	bool Initialize(HINSTANCE hInstance, int width, int height);
	bool ProcessMessages();
	void Update();
	void RenderFrame();
};

