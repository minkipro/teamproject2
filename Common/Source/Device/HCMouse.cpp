#include "HCMouse.h"

void HCMouse::Init()
{
}

void HCMouse::Update()
{
	DirectX::Mouse::ButtonStateTracker::Update(DirectX::Mouse::Get().GetState());
}
