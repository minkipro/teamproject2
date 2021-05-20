#include "stdafx.h"

HC::CameraManager HC::CameraManager::instance;
HC::CameraManager::CameraManager()
{
	m_mainCamera = nullptr;
	m_viewMatrix = DirectX::XMMatrixIdentity();
}
HC::Camera* HC::CameraManager::CreateCamera()
{
	m_cameras.push_back(std::make_unique<Camera>());
	if (nullptr == m_mainCamera)
	{
		m_mainCamera = m_cameras.back().get();
	}
	return m_cameras.back().get();
}
void HC::CameraManager::Update()
{
	auto keyboard = HCDEVICE(HCKeyboard);
	auto state = keyboard->GetLastState();
	/*if (state.IsKeyDown(DirectX::Keyboard::Keys::Up))
	{
		m_mainCamera->position.y -= 100.0f*
	}*/
	m_viewMatrix = DirectX::XMMatrixScaling(m_mainCamera->scale.x, m_mainCamera->scale.y, 1.0f)*DirectX::XMMatrixRotationZ(m_mainCamera->rot)*DirectX::XMMatrixTranslation(m_mainCamera->position.x, m_mainCamera->position.y, 0.0f);
}


