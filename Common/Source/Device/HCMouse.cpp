#include "stdafx.h"
#include "HCMouse.h"

void HCMouse::Init()
{
}

void HCMouse::Update()
{
	DirectX::Mouse::ButtonStateTracker::Update(DirectX::Mouse::Get().GetState());
}

LRESULT HCMouse::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return LRESULT();
}
