#pragma once
#include <string>
#include <typeinfo>

class IHCDevice
{
public:
	IHCDevice() {}
	virtual ~IHCDevice() = default;

public:
	virtual void		Init() = 0;
	virtual void		Update() = 0;
	virtual std::string GetDeviceName() const = 0;

	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return LRESULT(0); }

protected:

private:
};
