#pragma once
#include "RenderWindow.h"
#include "Graphics/Graphics.h"
#include "InputDevice/KeyboardClass.h"
#include "InputDevice/MouseClass.h"

class WindowContainer
{
public:
	WindowContainer();
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	RenderWindow render_window;
	KeyboardClass keyboard;
	MouseClass mouse;
	Graphics gfx;
};