#include "stdafx.h"
#include "HCKeyboard.h"

void HCKeyboard::Init()
{

}

void HCKeyboard::Update()
{
	float deltaTime = HCDEVICE(HCTimer)->GetDeltatime();
	auto keyboardState = DirectX::Keyboard::Get().GetState();
	auto currPtr = reinterpret_cast<const uint32_t*>(&keyboardState);
	auto prevPtr = reinterpret_cast<const uint32_t*>(&lastState);
	auto releasedPtr = reinterpret_cast<uint32_t*>(&released);
	auto pressedPtr = reinterpret_cast<uint32_t*>(&pressed);
	auto heldPtr = reinterpret_cast<uint32_t*>(&held);

	for (size_t j = 0; j < (256 / 32); ++j)
	{
		*pressedPtr = *currPtr & ~(*prevPtr);
		*releasedPtr = ~(*currPtr) & *prevPtr;
		*heldPtr = *currPtr & *prevPtr;

		++currPtr;
		++prevPtr;
		++releasedPtr;
		++pressedPtr;
		++heldPtr;
	}

	lastState = keyboardState;

	ZeroMemory(m_usedBaseKeyFuncStates, sizeof(m_usedBaseKeyFuncStates));

	for (auto& it : m_currObjectFuncs)
	{
		m_usedBaseKeyFuncStates[it.KeyType] = true;

		if (IsKeyCurrState(it.KeyType, it.KeyState))
		{
			it.ControlFunc(m_currObject, deltaTime);
		}
	}

	for (int i = 0; i < UCHAR_MAX; i++)
	{
		if (m_baseKeyFuncs[i] != nullptr && !m_usedBaseKeyFuncStates[i] && released.IsKeyDown(static_cast<HCKEYBOARD_KEY_TYPE>(i)))
		{
			m_baseKeyFuncs[i](deltaTime);
		}
	}
}

bool HCKeyboard::IsKeyCurrState(HCKEYBOARD_KEY_TYPE key, HCKEYBOARD_KEY_STATE state)
{
	bool result = false;

	switch (state)
	{
	case HCKEYBOARD_KEY_STATE::HELD:
	{
		result = held.IsKeyDown(key);
	}
	break;
	case HCKEYBOARD_KEY_STATE::RELEASED:
	{
		result = released.IsKeyDown(key);
	}
	break;
	case HCKEYBOARD_KEY_STATE::PRESSED:
	{
		result = pressed.IsKeyDown(key);
	}
	break;
	default:
		break;
	}

	return result;
}

void HCKeyboard::SetBaseKeyFunc(HCKEYBOARD_KEY_TYPE key, std::function<void(float)> func)
{
	m_baseKeyFuncs[key] = func;
}

void HCKeyboard::SetObject(void* object, const std::vector<HCKEYBOARD_CONTROL_FUNC>* funcs)
{
	m_currObjectFuncs.clear();
	m_currObject = object;
	m_currObjectFuncs = *funcs;
}

//
//HCKeyboardController::HCKeyboardController()
//{
//	for (auto& it : m_currKeyFuncIndex)
//	{
//		it = -1;
//	}
//}
//
//void HCKeyboardController::Update()
//{
//	auto keyDeivce = HCDEVICE(HCKeyboard);
//	float deltaTime = HCDEVICE(HCTimer)->GetDeltatime();
//
//	for (UINT i = 0; i < UCHAR_MAX; i++)
//	{
//		if (m_currKeyFuncIndex[i] == 0)
//		{
//			auto& currFunc = m_baseKeyFuncs[i][m_currKeyFuncIndex[m_currKeyFuncIndex[i]]];
//
//			if (keyDeivce->IsKeyCurrState(static_cast<HCKEYBOARD_KEY_TYPE>(i), currFunc.KeyState))
//			{
//				currFunc.ControlFunc(m_currObject, deltaTime);
//			}
//		}
//		else if (m_currKeyFuncIndex[i] == 1)
//		{
//			auto& currFunc = m_baseKeyFuncs[i][m_currKeyFuncIndex[m_currKeyFuncIndex[i]]];
//
//			if (keyDeivce->IsKeyReleased(static_cast<HCKEYBOARD_KEY_TYPE>(i)))
//			{
//				currFunc.ControlFunc(nullptr, deltaTime);
//			}
//		}
//	}
//}
//
//void HCKeyboardController::SetBaseKey(HCKEYBOARD_KEY_TYPE type, std::function<void(void*, float)> func)
//{
//	COM_THROW_IF_FAILED(type < UCHAR_MAX, "this type is over 254");
//
//	m_baseKeyFuncs[type][0].ControlFunc = func;
//	m_currKeyFuncIndex[type] = m_currKeyFuncIndex[type] < 0 ? 0 : m_currKeyFuncIndex[type];
//}
//
//void HCKeyboardController::AddControlData(const HCKEYBOARD_CONTROL_FUNC& data)
//{
//
//}