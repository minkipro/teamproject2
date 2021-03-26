#pragma once
#define WIN32_LEAN_AND_MEAN      
#include <Windows.h>
#include <string>
#include <typeinfo>

class HCDevice
{
public:
	HCDevice() {}
	virtual ~HCDevice() = default;

public:
	virtual void		Init() = 0;
	virtual void		Update() = 0;
	virtual std::string GetDeviceName() const = 0;

	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return LRESULT(0); }

protected:

private:
};
