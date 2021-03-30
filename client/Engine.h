#pragma once
#include <vector>
#include <memory>
#include "HCDevice.h"
#include "Window/HCWindow.h"

class Engine
{
public:
	Engine() = default;
	~Engine() = default;

	void	Init(HINSTANCE hInstance);
	int		Run();

private:
	std::vector<std::unique_ptr<IHCDevice>>	m_Devices;
	std::unique_ptr<HCWindow>				m_Window;
};

