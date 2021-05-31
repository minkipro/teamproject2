#pragma once
#include "HCDevice.h"
class HCTimer : public IHCDevice
{
public:
	HCTimer();

	virtual void		Init() override;
	virtual void		Update() override;
	virtual std::string GetDeviceName() const { return typeid(HCTimer).name(); };

	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	float				TotalTime()const;
	float				GetDeltatime()const;

	void				Reset();
	void				Start();
	void				Stop();

private:
	double m_secondsPerCount;
	double m_deltaTime;

	__int64 m_baseTime;
	__int64 m_pausedTime;
	__int64 m_stopTime;
	__int64 m_prevTime;
	__int64 m_currTime;

	bool m_stopped;
};