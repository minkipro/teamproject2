#pragma once
#include "Window/WindowContainer.h"

class Engine : WindowContainer
{
public:
	bool Initialize(HINSTANCE hInstance, int width, int height);
	bool ProcessMessages();
	void Update();
	void RenderFrame();

private:
};

