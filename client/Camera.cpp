#include "Camera.h"
#include "Keyboard/KeyboardClass.h"
void Camera::Init()
{
	orthoMatrix = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, 800.0f, 600.0f, 0.0f, 0.0f, 1.0f);

	pos = { 0.0f, 0.0f };
}

void Camera::Update(KeyboardClass* keyboard)
{
	if (keyboard->KeyIsPressed('W'))
	{
		pos.y -= 0.1f;
	}
	if (keyboard->KeyIsPressed('A'))
	{
		pos.x -= 0.1f;
	}
	if (keyboard->KeyIsPressed('S'))
	{
		pos.y += 0.1f;
	}
	if (keyboard->KeyIsPressed('D'))
	{
		pos.x += 0.1f;
	}
	worldMatrix = DirectX::XMMatrixTranslation(-pos.x, -pos.y, 0.0f);
}

DirectX::XMMATRIX* Camera::GetWorld()
{
	return &worldMatrix;
}

DirectX::XMMATRIX* Camera::GetOrtho()
{
	return &orthoMatrix;
}
