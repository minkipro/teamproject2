#pragma once
#include <vector>
#include <functional>
#include "HCDevice.h"

class HCWindow
{
public:
	HCWindow()
		: m_hAppInst(nullptr)
		, m_hMainWnd(nullptr)
	{
	}

	virtual ~HCWindow();

	void	Init(HINSTANCE hInstance);
	void	RegisterProc(HCDevice* device);

private:
	static LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	static std::vector<HCDevice*>			s_Proces;
	HINSTANCE								m_hAppInst;
	HWND									m_hMainWnd;
};