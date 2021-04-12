#pragma once
#include <Keyboard.h>
#include "HCDevice.h"
#include <queue>
struct KeyboardEvent
{
	enum class EventType
	{
		Invalid = 0,
		Press = 1,
		Release = 2
	};

	KeyboardEvent(EventType type_, unsigned char key_) : type(type_), key(key_) {}
	EventType type;
	unsigned char key;
};
class HCKeyboard : public IHCDevice, DirectX::Keyboard::KeyboardStateTracker
{
public:
	HCKeyboard() = default;
	virtual ~HCKeyboard() = default;

	virtual void		Init();
	virtual void		Update();
	virtual std::string GetDeviceName() const { return typeid(HCKeyboard).name(); }
	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	bool m_autoRepeat = false;
	bool m_keyStates[256];
	std::queue<KeyboardEvent> m_keyBuffer;
	std::queue<unsigned char> m_charBuffer;
};

