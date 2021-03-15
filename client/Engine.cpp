#include "Engine.h"
#include <DirectXMath.h>
bool Engine::Initialize(HINSTANCE hInstance, int width, int height)
{
	if (!render_window.Initialize(this, hInstance, "win_title", "win_class", width, height))
		return false;

	if (!gfx.Initialize(render_window.GetHWND(), width, height))
		return false;

	gameObject.Initialize(gfx.GetDevice(), gfx.GetDeviceContext());
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

	DirectX::XMMATRIX orthoMatrix = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, 800.0f, 600.0f, 0.0f, 0.0f, 1.0f);

	static float pos[2] = { 0.0f, 0.0f };
	if (keyboard.KeyIsPressed('W'))
	{
		pos[1] -= 0.1f;
	}
	if (keyboard.KeyIsPressed('A'))
	{
		pos[0] -= 0.1f;
	}
	if (keyboard.KeyIsPressed('S'))
	{
		pos[1] += 0.1f;
	}
	if (keyboard.KeyIsPressed('D'))
	{
		pos[0] += 0.1f;
	}
	DirectX::XMMATRIX cameraWorldMatirx = DirectX::XMMatrixTranslation(-pos[0], -pos[1], 0.0f);
	gameObject.Update(cameraWorldMatirx * orthoMatrix);
}

void Engine::RenderFrame()
{
	gfx.RenderFrame();
	gameObject.Draw(gfx.GetDeviceContext());
	gfx.Present();
}
