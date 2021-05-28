#include "stdafx.h"
#include "HCCharacterControllerByKeyboard.h"

HC::CharacterControllerByKeyboard::CharacterControllerByKeyboard(DirectX::XMFLOAT3* position, int* textureIndex, int spriteNum, std::vector<int> animationIndex[])
{
	m_state = CharacterState::IDLE;
	m_prevState = CharacterState::IDLE;
	m_continuos = true;
	m_isThereInput = false;
	m_position = position;
	m_textureIndex = textureIndex;
	m_originTextureIndex = *m_textureIndex;
	m_deltaTime = 0.0f;
	m_animationIndexChangeTime = 0.3f;
	m_currentAnimationIndex = 0;
	m_textRender = nullptr;

	for (unsigned char i = 0; i < (unsigned char)CharacterMoveState::COUNT; i++)
	{
		m_characterMoveState[i] = false;
	}

	for (unsigned char i = 0; i < (unsigned char)CharacterState::COUNT; i++)
	{
		size_t animationIndexNum = animationIndex[i].size();
		for (size_t j = 0; j < animationIndexNum; j++)
		{
			m_animationIndex[i].push_back(animationIndex[i][j]);
		}
	}

	m_keyMapping[(unsigned char)BehaviorState::UP] = { true, DirectX::Keyboard::W };
	m_keyMapping[(unsigned char)BehaviorState::DOWN] = { true, DirectX::Keyboard::S };
	m_keyMapping[(unsigned char)BehaviorState::LEFT] = { true, DirectX::Keyboard::A };
	m_keyMapping[(unsigned char)BehaviorState::RIGHT] = { true, DirectX::Keyboard::D };
	m_keyMapping[(unsigned char)BehaviorState::A] = { false, HCMouse::MouseButton::LBUTTON };
	m_keyMapping[(unsigned char)BehaviorState::B] = { false, HCMouse::MouseButton::RBUTTON };
	m_keyMapping[(unsigned char)BehaviorState::C] = { true, DirectX::Keyboard::None };
	m_keyMapping[(unsigned char)BehaviorState::D] = { true, DirectX::Keyboard::None };
	
	
	auto timer = HCDEVICE(HCTimer);
	for (unsigned char i = (unsigned char)BehaviorState::UP; i <= (unsigned char)BehaviorState::RIGHT; i++)
	{
		m_behaviorStateFunction[i][(int)KeyState::PRESSED].push_back([this, i]
			{
				m_characterMoveState[i] = true;
			});
	}
	m_behaviorStateFunction[(unsigned char)BehaviorState::A][(unsigned char)KeyState::PRESSED].push_back([this]
		{
			m_buttonState[0] = true;//test
		});
	m_behaviorStateFunction[(unsigned char)BehaviorState::B][(unsigned char)KeyState::PRESSED].push_back([this]
		{
			m_buttonState[1] = true;//test
		});


	//debug용 코드
	auto graphic = HCDEVICE(HCGraphic);
	graphic->CreateTextData(m_textRender);

	std::vector<std::wstring> fontNames;
	m_textRender->GetFontNames(fontNames);
	m_textRender->SetFont(fontNames.front());
	m_textRender->m_position.y += 25;

	m_buttonState[0] = m_buttonState[1] = false;
}

void HC::CharacterControllerByKeyboard::Update()
{
	auto keyboard = HCDEVICE(HCKeyboard);
	auto mouse = HCDEVICE(HCMouse);
	auto timer = HCDEVICE(HCTimer);
	auto state = keyboard->GetLastState();

	static std::function<bool(unsigned char key)> keyStateConditionalFunction[2][(int)KeyState::COUNT];//0 : 키보드, 1 : 마우스
	keyStateConditionalFunction[0][(unsigned char)KeyState::ONCE] = [keyboard](unsigned char key) {return keyboard->IsKeyPressed((DirectX::Keyboard::Keys)key); };
	keyStateConditionalFunction[0][(unsigned char)KeyState::PRESSED] = [state](unsigned char key) {return state.IsKeyDown((DirectX::Keyboard::Keys)key); };
	keyStateConditionalFunction[0][(unsigned char)KeyState::RELEASED] = [keyboard](unsigned char key) {return keyboard->IsKeyReleased((DirectX::Keyboard::Keys)key); };
	keyStateConditionalFunction[1][(unsigned char)KeyState::ONCE] = [mouse](unsigned char key) {return mouse->GetButtonState((HCMouse::MouseButton)key) == DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED; };
	keyStateConditionalFunction[1][(unsigned char)KeyState::PRESSED] = [mouse](unsigned char key) {return mouse->GetButtonState((HCMouse::MouseButton)key) == DirectX::Mouse::ButtonStateTracker::ButtonState::HELD; };
	keyStateConditionalFunction[1][(unsigned char)KeyState::RELEASED] = [mouse](unsigned char key) {return mouse->GetButtonState((HCMouse::MouseButton)key) == DirectX::Mouse::ButtonStateTracker::ButtonState::RELEASED; };
	m_isThereInput = false;
	m_buttonState[0] = m_buttonState[1] = false;//test
	for (unsigned char i = 0; i < (unsigned char)CharacterMoveState::COUNT; i++)
	{
		m_characterMoveState[i] = false;
	}

	for (unsigned char i = 0; i < (unsigned char)BehaviorState::COUNT; i++)
	{
		int isKeyboard = m_keyMapping[i].isKeyboard ? 0 : 1;
		for (unsigned char k = 0; k < (unsigned char)KeyState::COUNT; k++)
		{
			if (keyStateConditionalFunction[isKeyboard][k](m_keyMapping[i].data))
			{
				size_t functionNum = m_behaviorStateFunction[i][k].size();
				for (size_t j = 0; j < functionNum; j++)
				{
					m_behaviorStateFunction[i][k][j]();
				}
			}
		}
	}

	if (m_characterMoveState[(unsigned char)CharacterMoveState::LEFT] && m_characterMoveState[(unsigned char)CharacterMoveState::RIGHT])//왼쪽 오른쪽 동시에 눌렀을때 처리
	{
		m_characterMoveState[(unsigned char)CharacterMoveState::LEFT] = m_characterMoveState[(unsigned char)CharacterMoveState::RIGHT] = false;
	}
	if (m_characterMoveState[(unsigned char)CharacterMoveState::UP] && m_characterMoveState[(unsigned char)CharacterMoveState::DOWN])//위아래 처리
	{
		m_characterMoveState[(unsigned char)CharacterMoveState::UP] = m_characterMoveState[(unsigned char)CharacterMoveState::DOWN] = false;
	}
	static std::function<void()> moveFunctions[(unsigned char)CharacterMoveState::COUNT];
	moveFunctions[(unsigned char)CharacterMoveState::UP] = [this, timer]()
	{
		m_continuos = m_prevState == CharacterState::UP;
		m_state = CharacterState::UP;
		m_isThereInput = true;
		m_position->y -= 50.0f * timer->GetDeltatime();
	};
	moveFunctions[(unsigned char)CharacterMoveState::LEFT] = [this, timer]()
	{
		m_continuos = m_prevState == CharacterState::LEFT;
		m_state = CharacterState::LEFT;
		m_isThereInput = true;
		m_position->x -= 50.0f * timer->GetDeltatime();
	};
	moveFunctions[(unsigned char)CharacterMoveState::DOWN] = [this, timer]()
	{
		m_continuos = m_prevState == CharacterState::DOWN;
		m_state = CharacterState::DOWN;
		m_isThereInput = true;
		m_position->y += 50.0f * timer->GetDeltatime();
	};
	moveFunctions[(unsigned char)CharacterMoveState::RIGHT] = [this, timer]()
	{
		m_continuos = m_prevState == CharacterState::RIGHT;
		m_state = CharacterState::RIGHT;
		m_isThereInput = true;
		m_position->x += 50.0f * timer->GetDeltatime();
	};

	for (unsigned char i = 0; i < (unsigned char)CharacterMoveState::COUNT; i++)
	{
		if (m_characterMoveState[i])
		{
			moveFunctions[i]();
		}
	}

	if (!m_isThereInput)
	{
		m_continuos = m_prevState == CharacterState::IDLE;
		m_state = CharacterState::IDLE;
	}

	m_prevState = m_state;
	if (m_continuos)
	{
		m_deltaTime += timer->GetDeltatime();
		if (m_deltaTime >= m_animationIndexChangeTime)
		{
			m_deltaTime -= m_animationIndexChangeTime;
			m_currentAnimationIndex += 1;
			if (m_animationIndex[(unsigned char)m_state].size() <= m_currentAnimationIndex)
			{
				m_currentAnimationIndex = 0;
			}

			*m_textureIndex = m_originTextureIndex + m_animationIndex[(unsigned char)m_state][m_currentAnimationIndex];//애니메이션이 없는 경우에는 오류발생
		}
	}
	else
	{
		m_deltaTime = 0.0f;
		m_currentAnimationIndex = 0;
		*m_textureIndex = m_originTextureIndex + m_animationIndex[(unsigned char)m_state][m_currentAnimationIndex];
	}

	auto graphic = HCDEVICE(HCGraphic);
	std::wstring imtrue = L"true";
	std::wstring imfalse = L"false";
	m_textRender->m_text=/*L"m_position->x : " + std::to_wstring(m_position->x)
		+ L"m_position->y : " + std::to_wstring(m_position->y)
		+ L"m_deltaTime : " + std::to_wstring(m_deltaTime)
		+ *//*L"m_currentAnimationIndex : " + std::to_wstring(m_currentAnimationIndex)
		+ L"m_uv->x : " + std::to_wstring(m_uv->x)
		+ L"m_uv->y : " + std::to_wstring(m_uv->y)
		+ L"m_uv->z : " + std::to_wstring(m_uv->z)
		+ L"m_uv->w : " + std::to_wstring(m_uv->w)*/
		//L"leftButtonState : " + std::to_wstring((int)mouse->leftButton);

		L" m_buttonState[0] : " + (m_buttonState[0] ? imtrue : imfalse)
		+ L" m_buttonState[1] : " + (m_buttonState[1] ? imtrue : imfalse);
}
