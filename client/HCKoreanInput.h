#pragma once

#include "HCDevice.h"
class HCKoreanInput : public IHCDevice
{
public:
	HCKoreanInput();

	virtual void		Init() override;
	virtual void		Update() override;
	virtual std::string GetDeviceName() const override;

	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};