#include "stdafx.h"
#include "HCCharacterControlTest.h"

void HCCharacterControlTest::Init()
{
	enum
	{
		CHAR_IDLE,
		CHAR_UP,
		CHAR_LEFT,
		CHAR_DOWN,
		CHAR_RIGHT,
		CHAR_COUNT,
	};

	m_animations.resize(CHAR_COUNT);
	HCTextureData textureData = HCDEVICE(HCGraphic)->GetTextureIndex(L"Texture/PIPOYA FREE RPG Character Sprites NEKONIN/sp_3x4_pipo-nekonin001.png");
	for (auto& it : m_animations)
	{
		it.SetTexture(textureData);
	}

	const float baseAniTime = 0.3f;
	m_animations[CHAR_IDLE].SetSpriteIndices({ 1 }, baseAniTime, true);
	m_animations[CHAR_UP].SetSpriteIndices({ 9, 10, 11 }, baseAniTime, true);
	m_animations[CHAR_LEFT].SetSpriteIndices({ 3, 4, 5 }, baseAniTime, true);
	m_animations[CHAR_DOWN].SetSpriteIndices({ 0, 1, 2 }, baseAniTime, true);
	m_animations[CHAR_RIGHT].SetSpriteIndices({ 6, 7, 8 }, baseAniTime, true);

	m_character.Init();
	m_character.SetZ(0.1f);
	m_character.SetSize({ 100.0f, 100.0f });
	m_character.SetAnimation(&m_animations[CHAR_IDLE]);

	auto charIdle = [this](HC2DCharacter* character, float deltaTime)->void
	{
		character->SetAnimation(&m_animations[CHAR_IDLE]);
	};

	auto charMoveUp = [this](HC2DCharacter* character, float deltaTime)->void
	{
		DirectX::XMFLOAT2 pos = { 0.0f, -200.0f * deltaTime };
		character->SetAnimation(&m_animations[CHAR_UP]);
		character->AddPos(pos);
	};

	auto charMoveLeft = [this](HC2DCharacter* character, float deltaTime)->void
	{
		DirectX::XMFLOAT2 pos = { -200.0f * deltaTime, 0.0f };
		character->SetAnimation(&m_animations[CHAR_LEFT]);
		character->AddPos(pos);
	};

	auto charMoveDown = [this](HC2DCharacter* character, float deltaTime)->void
	{
		DirectX::XMFLOAT2 pos = { 0.0f, 200.0f * deltaTime };
		character->SetAnimation(&m_animations[CHAR_DOWN]);
		character->AddPos(pos);
	};

	auto charMoveRight = [this](HC2DCharacter* character, float deltaTime)->void
	{
		DirectX::XMFLOAT2 pos = { 200.0f * deltaTime, 0.0f };
		character->SetAnimation(&m_animations[CHAR_RIGHT]);
		character->AddPos(pos);
	};

	m_controler.SetCharacter(&m_character);

	m_controler.AddControlData(HCINPUT_DEVICE_TYPE::KEYBOARD, { HCKEYBOARD_KEY_TYPE::W, HCKEYBOARD_KEY_STATE::HELD,  charMoveUp });
	m_controler.AddControlData(HCINPUT_DEVICE_TYPE::KEYBOARD, { HCKEYBOARD_KEY_TYPE::S, HCKEYBOARD_KEY_STATE::HELD,  charMoveDown });
	m_controler.AddControlData(HCINPUT_DEVICE_TYPE::KEYBOARD, { HCKEYBOARD_KEY_TYPE::A, HCKEYBOARD_KEY_STATE::HELD,  charMoveLeft });
	m_controler.AddControlData(HCINPUT_DEVICE_TYPE::KEYBOARD, { HCKEYBOARD_KEY_TYPE::D, HCKEYBOARD_KEY_STATE::HELD,  charMoveRight });

	m_controler.AddControlData(HCINPUT_DEVICE_TYPE::KEYBOARD, { HCKEYBOARD_KEY_TYPE::W, HCKEYBOARD_KEY_STATE::RELEASED,  charIdle });
	m_controler.AddControlData(HCINPUT_DEVICE_TYPE::KEYBOARD, { HCKEYBOARD_KEY_TYPE::S, HCKEYBOARD_KEY_STATE::RELEASED,  charIdle });
	m_controler.AddControlData(HCINPUT_DEVICE_TYPE::KEYBOARD, { HCKEYBOARD_KEY_TYPE::A, HCKEYBOARD_KEY_STATE::RELEASED,  charIdle });
	m_controler.AddControlData(HCINPUT_DEVICE_TYPE::KEYBOARD, { HCKEYBOARD_KEY_TYPE::D, HCKEYBOARD_KEY_STATE::RELEASED,  charIdle });
}

void HCCharacterControlTest::Update(float deltaTime)
{
	m_controler.Update();
	m_character.Update(deltaTime);
}

void HCCharacterControlTest::Render()
{
	m_character.Render();
}
