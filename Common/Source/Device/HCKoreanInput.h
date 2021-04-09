#pragma once

#include "HCDevice.h"
class HCKoreanInput : public IHCDevice
{
public:
	HCKoreanInput();

	virtual void		Init() override;
	virtual void		Update() override;
	virtual std::string GetDeviceName() const { return typeid(HCKoreanInput).name(); };
	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	wchar_t m_text[255] = { 0, };
	wchar_t m_cstr[10] = { 0, };
};