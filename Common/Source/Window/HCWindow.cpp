#include "HCWindow.h"
#include <Keyboard.h>
#include <Mouse.h>
#include "GlobalOption.h"
#include "COMException.h"

std::vector<HCDevice*> HCWindow::s_Proces;
DirectX::Mouse mouse;
DirectX::Keyboard keyboard;

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

void HCWindow::RegisterProc(HCDevice* device)
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

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}