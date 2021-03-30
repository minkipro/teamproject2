#pragma once
#include <Keyboard.h>
#include "HCDevice.h"
#include <queue>

struct KeyboardEvent
{
	enum EventType
	{
		Invalid,
		Press,
		Release
	};

	KeyboardEvent(EventType type_, unsigned char key_) : type(type_), key(key_){}
	EventType type;
	unsigned char key;
};
class HCKeyboard : public HCDevice, DirectX::Keyboard::KeyboardStateTracker
{
public:
	HCKeyboard() = default;
	virtual ~HCKeyboard() = default;

	virtual void		Init();
	virtual void		Update();
	virtual std::string GetDeviceName() const { return typeid(HCKeyboard).name(); }
	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	bool autoRepeat = false;
	bool keyStates[256];
	std::queue<KeyboardEvent> keyBuffer;
	std::queue<unsigned char> charBuffer;
};

