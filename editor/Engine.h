#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include "HCDevice.h"
#include "DevScene.h"
#include "Window/HCWindow.h"

#include "Graphics/HCGraphic.h"
#include "Device\HCPhysics.h"
#include "Device/HCKeyboard.h"
#include "Device/HCMouse.h"
#include "Device/HCKoreanInput.h"
#include "Device/HCTimer.h"

#define HCDEVICE(Type) Engine::Get()->GetDevice<Type>()

class Engine
{
public:
	Engine() = default;
	~Engine() = default;

	static Engine* Get() { return m_engine; }
	void			Init(HINSTANCE hInstance);
	int				Run();

	void			ClearProc() { m_window->ClearProc(); }
	template<typename T, typename ...Types> T* CreateDevice(const char* name, Types ...args);
	template<typename T> T* GetDevice();

private:
	void			CreateBaseMeshs();
	void			CalculateFrame();

private:
	static Engine* m_engine;

	std::vector<std::unique_ptr<IHCDevice>>		m_devices;
	std::unordered_map<std::string, IHCDevice*>	m_deviceMap;
	std::unique_ptr<HCWindow>					m_window;
	HCGraphic* m_graphic;
	HCPhysics* m_physics;
	HCTimer* m_timer;
	HCMouse* m_mouse;
	HCKeyboard* m_keyboard;
	HCKoreanInput* m_koreanInput;


	DevScene									m_scene;
};

template<typename T, typename ...Types>
inline T* Engine::CreateDevice(const char* name, Types ...args)
{
	T* result = nullptr;
	auto iter = m_deviceMap.find(name);

	if (iter == m_deviceMap.end())
	{
		m_devices.emplace_back(std::make_unique<T>(args...));
		result = static_cast<T*>(m_devices.back().get());
		m_deviceMap[name] = m_devices.back().get();
	}
	else
	{
		COM_THROW_IF_FAILED(false, "");
	}

	return result;
}

template<typename T>
inline T* Engine::GetDevice()
{
	T* result = nullptr;
	auto iter = m_deviceMap.find(typeid(T).name());

	if (iter == m_deviceMap.end())
	{
		COM_THROW_IF_FAILED(false, "");
	}
	else
	{
		result = static_cast<T*>(iter->second);
	}

	return result;
}
