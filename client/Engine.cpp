#include "Engine.h"
#include <DirectXMath.h>
bool Engine::Initialize(HINSTANCE hInstance, int width, int height)
{
	if (!render_window.Initialize(this, hInstance, "win_title", "win_class", width, height))
		return false;

	if (!gfx.Initialize(render_window.GetHWND(), width, height))
		return false;

	scene.Init(gfx.GetDevice(), gfx.GetDeviceContext(), &keyboard, &gfx);
	return true;
}

bool Engine::ProcessMessages()
{
	return render_window.ProcessMessages();
}

void Engine::Update()
{
	while (!keyboard.CharBufferIsEmpty())
	{
		unsigned char ch = keyboard.ReadChar();
	}

	while (!keyboard.KeyBufferIsEmpty())
	{
		KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
	}

	while (!mouse.EventBufferIsEmpty())
	{
		MouseEvent me = mouse.ReadEvent();
	}
	scene.Update();
}

void Engine::RenderFrame()
{
	gfx.ClearFrame();
	scene.Draw();
	gfx.Present();
}
