#pragma once
#include "WindowContainer.h"
#include "GameObject.h"
class Engine : WindowContainer
{
private:
	GameObject gameObject;
public:
	bool Initialize(HINSTANCE hInstance, int width, int height);
	bool ProcessMessages();
	void Update();
	void RenderFrame();
};

