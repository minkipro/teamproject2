#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include "HCDevice.h"
#include "DevScene.h"
#include "Window/HCWindow.h"
#include "Graphics/HCGraphic.h"

#define HCDEVICE(Type) Engine::Get()->GetDevice<Type>()

class Engine
{
public:
	Engine() = default;
	~Engine() = default;

	static Engine*	Get() { return m_Engine; }
	void			Init(HINSTANCE hInstance);
	int				Run();

	void			ClearProc() { m_Window->ClearProc(); }
	template<typename T, typename ...Types> T* CreateDevice(const char* name, Types ...args);
	template<typename T> T* GetDevice();

private:
	static Engine*								m_Engine;

	DevScene									m_Scene;
	std::vector<std::unique_ptr<IHCDevice>>		m_Devices;
	std::unordered_map<std::string, IHCDevice*>	m_DeviceMap;
	std::unique_ptr<HCWindow>					m_Window;
	HCGraphic*									m_Graphic;
};

template<typename T, typename ...Types>
inline T* Engine::CreateDevice(const char* name,Types ...args)
{
	T* result = nullptr;
	auto iter = m_DeviceMap.find(name);
	
	if (iter == m_DeviceMap.end())
	{
		m_Devices.emplace_back(std::make_unique<T>(args...));
		result = static_cast<T*>(m_Devices.back().get());
		m_DeviceMap[name] = m_Devices.back().get();
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
	auto iter = m_DeviceMap.find(typeid(T).name());

	if (iter == m_DeviceMap.end())
	{
		COM_THROW_IF_FAILED(false, "");
	}
	else
	{
		result = static_cast<T*>(iter->second);
	}

	return result;
}
