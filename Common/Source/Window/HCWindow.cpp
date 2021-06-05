#include "stdafx.h"
#include "HCWindow.h"
#include "GlobalOption.h"
#include "COMException.h"
#include <Mouse.h>
#include <Keyboard.h>

std::vector<IHCDevice*> HCWindow::s_Proces;
bool					HCWindow::s_resizing = false;
bool					HCWindow::s_minimized = false;
bool					HCWindow::s_maximized = false;

DirectX::Mouse mouse;
DirectX::Keyboard keybaord;

HCWindow::~HCWindow()
{
}

void HCWindow::Init(HINSTANCE hInstance)
{
	m_hAppInst = hInstance;

	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = HCWindow::WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";

	COM_THROW_IF_FAILED(RegisterClass(&wc), L"RegisterClass Failed.");

	RECT R = { 0, 0, HC::GO.WIN.WindowsizeX, HC::GO.WIN.WindowsizeY };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	m_hMainWnd = CreateWindow(L"MainWnd", L"Created",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_hAppInst, 0);

	COM_THROW_IF_FAILED(m_hMainWnd != 0, L"CreateWindow Failed.");

	ShowWindow(m_hMainWnd, SW_SHOW);
	UpdateWindow(m_hMainWnd);

	DirectX::Mouse::Get().SetWindow(m_hMainWnd);
}

void HCWindow::RegisterProc(IHCDevice* device)
{
	s_Proces.push_back(device);
}

LRESULT HCWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DirectX::Keyboard::ProcessMessage(uMsg, wParam, lParam);
	DirectX::Mouse::ProcessMessage(uMsg, wParam, lParam);

	for (auto it : s_Proces)
	{
		it->WindowProc(hwnd, uMsg, wParam, lParam);
	}

	switch (uMsg)
	{
	case WM_SIZE:
	{
		int clientWidth = LOWORD(lParam);
		int clientHeight = HIWORD(lParam);

		if (s_Proces.size())
		{
			HC::GO.WIN.WindowsizeX = clientWidth;
			HC::GO.WIN.WindowsizeY = clientHeight;

			if (wParam == SIZE_MINIMIZED)
			{
				s_minimized = true;
				s_maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				s_minimized = false;
				s_maximized = true;
				HCDEVICE(HCGraphic)->Resize(HC::GO.WIN.WindowsizeX, HC::GO.WIN.WindowsizeY);
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (s_minimized)
				{
					s_minimized = false;
					HCDEVICE(HCGraphic)->Resize(HC::GO.WIN.WindowsizeX, HC::GO.WIN.WindowsizeY);
				}
				else if (s_maximized)
				{
					s_maximized = false;
					HCDEVICE(HCGraphic)->Resize(HC::GO.WIN.WindowsizeX, HC::GO.WIN.WindowsizeY);
				}
				else if (s_resizing)
				{
					HCDEVICE(HCGraphic)->Resize(HC::GO.WIN.WindowsizeX, HC::GO.WIN.WindowsizeY);
				}
			}
		}

		return 0;
	}
	case WM_ENTERSIZEMOVE:
	{
		s_resizing = true;
		return 0;
	}
	case WM_EXITSIZEMOVE:
	{
		if (s_Proces.size())
		{
			HCDEVICE(HCGraphic)->Resize(HC::GO.WIN.WindowsizeX, HC::GO.WIN.WindowsizeY);
			s_resizing = false;
		}

		return 0;
	}
	case WM_GETMINMAXINFO:
	{
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);
	case WM_IME_STARTCOMPOSITION:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	{
		uMsg = 0; //박스 윈도우 생성 막기 //system키를 인게임중에 막기(keystate는 위에서 변경)
	}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}