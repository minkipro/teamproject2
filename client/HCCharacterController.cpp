#include "stdafx.h"
#include "HCCharacterController.h"

HC::CharacterController::CharacterController(DirectX::XMFLOAT3* position, DirectX::XMFLOAT4* uv, int indexSizeX, int indexSizeY, std::vector<DirectX::XMUINT2> animationIndex[])
{
	m_keyMapping[(int)BehaviorState::UP] = { true, DirectX::Keyboard::W };
	m_keyMapping[(int)BehaviorState::DOWN] = { true, DirectX::Keyboard::S };
	m_keyMapping[(int)BehaviorState::LEFT] = { true, DirectX::Keyboard::A };
	m_keyMapping[(int)BehaviorState::RIGHT] = { true, DirectX::Keyboard::D };
	m_keyMapping[(int)BehaviorState::A] = { true, DirectX::Keyboard::F };
	m_keyMapping[(int)BehaviorState::B] = { true, DirectX::Keyboard::F };
	m_keyMapping[(int)BehaviorState::C] = { true, DirectX::Keyboard::F };
	m_keyMapping[(int)BehaviorState::D] = { true, DirectX::Keyboard::F };
	m_state = CharacterState::IDLE;
	m_prevState = CharacterState::IDLE;
	m_continuos = true;
	m_isThereInput = false;
	m_position = position;
	m_uv = uv;
	m_deltaTime = 0.0f;
	m_animationIndexChangeTime = 0.3f;
	m_currentAnimationIndex = 0;
	for (int i = 0; i < (int)CharacterState::COUNT; i++)
	{
		size_t animationIndexNum = animationIndex[i].size();
		for (size_t j = 0; j < animationIndexNum; j++)
		{
			float startX = (float)animationIndex[i][j].x / indexSizeX;
			float endX = (float)(animationIndex[i][j].x + 1) / indexSizeX;
			float startY = (float)(animationIndex[i][j].y) / indexSizeY;
			float endY = (float)(animationIndex[i][j].y + 1) / indexSizeY;
			m_animationIndex[i].push_back(DirectX::XMFLOAT4(startX, startY, endX, endY));
		}
	}
	auto timer = HCDEVICE(HCTimer);
	m_behaviorStateFunction[(unsigned long long)BehaviorState::UP][(unsigned long long)KeyState::PRESSED].push_back([this, timer] 
		{ 
			m_characterMoveState[(int)CharacterMoveState::UP] = true;
		});
	m_behaviorStateFunction[(unsigned long long)BehaviorState::LEFT][(unsigned long long)KeyState::PRESSED].push_back([this, timer] 
		{ 
			m_characterMoveState[(int)CharacterMoveState::LEFT] = true;
		});
	m_behaviorStateFunction[(unsigned long long)BehaviorState::DOWN][(unsigned long long)KeyState::PRESSED].push_back([this, timer] 
		{ 
			m_characterMoveState[(int)CharacterMoveState::DOWN] = true;
		});
	m_behaviorStateFunction[(unsigned long long)BehaviorState::RIGHT][(unsigned long long)KeyState::PRESSED].push_back([this, timer] 
		{
			m_characterMoveState[(int)CharacterMoveState::RIGHT] = true;
		});

	auto graphic = HCDEVICE(HCGraphic);
	auto texts = graphic->GetFont()->GetText();
	IHCFont::TextData tempData = { L"test2", DirectX::XMFLOAT2(0.0f, 10.0f), DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT2(1.0f, 1.0f) };
	texts->push_back(tempData);
	m_textIndex = texts->size() - 1;

	for (int i = 0; i < (int)CharacterMoveState::COUNT; i++)
	{
		m_characterMoveState[i] = false;
	}
}

void HC::CharacterController::Update()
{
	auto keyboard = HCDEVICE(HCKeyboard);
	auto mouse = HCDEVICE(HCMouse);
	auto timer = HCDEVICE(HCTimer);
	auto state = keyboard->GetLastState();
	static std::function<bool(DirectX::Keyboard::Keys key)> keyStateConditionalFunction[(unsigned long long)KeyState::COUNT];
	keyStateConditionalFunction[(unsigned long long)KeyState::ONCE] = [keyboard](DirectX::Keyboard::Keys key){return keyboard->IsKeyPressed(key); };
	keyStateConditionalFunction[(unsigned long long)KeyState::PRESSED] = [state](DirectX::Keyboard::Keys key){return state.IsKeyDown(key); };
	keyStateConditionalFunction[(unsigned long long)KeyState::RELEASED] = [keyboard](DirectX::Keyboard::Keys key){return keyboard->IsKeyReleased(key); };
	m_isThereInput = false;

	for (int i = 0; i < (int)CharacterMoveState::COUNT; i++)
	{
		m_characterMoveState[i] = false;
	}

	for(int i = 0; i < (int)BehaviorState::COUNT; i++)
	{
		for (int k = 0; k < (int)KeyState::COUNT; k++)
		{
			size_t functionNum = m_behaviorStateFunction[i][k].size();
			for (size_t j = 0; j < functionNum; j++)
			{
				if (m_keyMapping[i].isKeyboard)
				{
					if (keyStateConditionalFunction[k]((DirectX::Keyboard::Keys)m_keyMapping[i].data))
					{
						m_behaviorStateFunction[i][k][j]();
					}
				}
				else//마우스입력일때
				{

				}
			}
		}
	}

	if (m_characterMoveState[(int)CharacterMoveState::LEFT] && m_characterMoveState[(int)CharacterMoveState::RIGHT])//왼쪽 오른쪽 동시에 눌렀을때 처리
	{
		m_characterMoveState[(int)CharacterMoveState::LEFT] = m_characterMoveState[(int)CharacterMoveState::RIGHT] = false;
	}
	if (m_characterMoveState[(int)CharacterMoveState::UP] && m_characterMoveState[(int)CharacterMoveState::DOWN])//위아래 처리
	{
		m_characterMoveState[(int)CharacterMoveState::UP] = m_characterMoveState[(int)CharacterMoveState::DOWN] = false;
	}
	static std::function<void()> moveFunctions[(int)CharacterMoveState::COUNT];
	moveFunctions[(int)CharacterMoveState::UP] = [this, timer]()
	{
		m_continuos = m_prevState == CharacterState::UP;
		m_state = CharacterState::UP;
		m_isThereInput = true;
		m_position->y -= 50.0f * timer->GetDeltatime();
	};
	moveFunctions[(int)CharacterMoveState::LEFT] = [this, timer]()
	{
		m_continuos = m_prevState == CharacterState::LEFT;
		m_state = CharacterState::LEFT;
		m_isThereInput = true;
		m_position->x -= 50.0f * timer->GetDeltatime();
	};
	moveFunctions[(int)CharacterMoveState::DOWN] = [this, timer]()
	{
		m_continuos = m_prevState == CharacterState::DOWN;
		m_state = CharacterState::DOWN;
		m_isThereInput = true;
		m_position->y += 50.0f * timer->GetDeltatime();
	};
	moveFunctions[(int)CharacterMoveState::RIGHT] = [this, timer]()
	{
		m_continuos = m_prevState == CharacterState::RIGHT;
		m_state = CharacterState::RIGHT;
		m_isThereInput = true;
		m_position->x += 50.0f * timer->GetDeltatime();
	};

	for (int i = 0; i < (int)CharacterMoveState::COUNT; i++)
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
			if (m_animationIndex[(unsigned long long)m_state].size() <= m_currentAnimationIndex)
			{
				m_currentAnimationIndex = 0;
			}

			*m_uv = m_animationIndex[(unsigned long long)m_state][(unsigned long long)m_currentAnimationIndex];
		}
	}
	else
	{
		m_deltaTime = 0.0f;
		m_currentAnimationIndex = 0;
		*m_uv = m_animationIndex[(unsigned long long)m_state][(unsigned long long)m_currentAnimationIndex];
	}

	auto graphic = HCDEVICE(HCGraphic);
	auto texts = graphic->GetFont()->GetText();
	(*texts)[m_textIndex].Text
		= /*L"m_position->x : " + std::to_wstring(m_position->x)
		+ L"m_position->y : " + std::to_wstring(m_position->y)
		+ L"m_deltaTime : " + std::to_wstring(m_deltaTime)
		+ *//*L"m_currentAnimationIndex : " + std::to_wstring(m_currentAnimationIndex)
		+ L"m_uv->x : " + std::to_wstring(m_uv->x)
		+ L"m_uv->y : " + std::to_wstring(m_uv->y)
		+ L"m_uv->z : " + std::to_wstring(m_uv->z)
		+ L"m_uv->w : " + std::to_wstring(m_uv->w)*/
		L"m_state : " + std::to_wstring((int)m_state);
}
