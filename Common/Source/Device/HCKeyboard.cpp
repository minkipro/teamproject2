#include "HCKeyboard.h"


void HCKeyboard::Init()
{

}

void HCKeyboard::Update()
{
	DirectX::Keyboard::KeyboardStateTracker::Update(DirectX::Keyboard::Get().GetState());
}
