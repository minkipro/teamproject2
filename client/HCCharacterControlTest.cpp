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

	auto charIdle = [this](void* character, float deltaTime)->void
	{
		static_cast<HC2DCharacter*>(character)->SetAnimation(&m_animations[CHAR_IDLE]);
	};

	auto charMoveUp = [this](void* character, float deltaTime)->void
	{
		DirectX::XMFLOAT2 pos = { 0.0f, -200.0f * deltaTime };
		static_cast<HC2DCharacter*>(character)->SetAnimation(&m_animations[CHAR_UP]);
		static_cast<HC2DCharacter*>(character)->AddPos(pos);
	};

	auto charMoveLeft = [this](void* character, float deltaTime)->void
	{
		DirectX::XMFLOAT2 pos = { -200.0f * deltaTime, 0.0f };
		static_cast<HC2DCharacter*>(character)->SetAnimation(&m_animations[CHAR_LEFT]);
		static_cast<HC2DCharacter*>(character)->AddPos(pos);
	};

	auto charMoveDown = [this](void* character, float deltaTime)->void
	{
		DirectX::XMFLOAT2 pos = { 0.0f, 200.0f * deltaTime };
		static_cast<HC2DCharacter*>(character)->SetAnimation(&m_animations[CHAR_DOWN]);
		static_cast<HC2DCharacter*>(character)->AddPos(pos);
	};

	auto charMoveRight = [this](void* character, float deltaTime)->void
	{
		DirectX::XMFLOAT2 pos = { 200.0f * deltaTime, 0.0f };
		static_cast<HC2DCharacter*>(character)->SetAnimation(&m_animations[CHAR_RIGHT]);
		static_cast<HC2DCharacter*>(character)->AddPos(pos);
	};

	auto keyboard = HCDEVICE(HCKeyboard);
	std::vector<HCKEYBOARD_CONTROL_FUNC> funcs;

	funcs.push_back({ HCKEYBOARD_KEY_TYPE::W, HCKEYBOARD_KEY_STATE::HELD,  charMoveUp });
	funcs.push_back( { HCKEYBOARD_KEY_TYPE::S, HCKEYBOARD_KEY_STATE::HELD,  charMoveDown });
	funcs.push_back( { HCKEYBOARD_KEY_TYPE::A, HCKEYBOARD_KEY_STATE::HELD,  charMoveLeft });
	funcs.push_back( { HCKEYBOARD_KEY_TYPE::D, HCKEYBOARD_KEY_STATE::HELD,  charMoveRight });

	funcs.push_back( { HCKEYBOARD_KEY_TYPE::W, HCKEYBOARD_KEY_STATE::RELEASED,  charIdle });
	funcs.push_back( { HCKEYBOARD_KEY_TYPE::S, HCKEYBOARD_KEY_STATE::RELEASED,  charIdle });
	funcs.push_back( { HCKEYBOARD_KEY_TYPE::A, HCKEYBOARD_KEY_STATE::RELEASED,  charIdle });
	funcs.push_back( { HCKEYBOARD_KEY_TYPE::D, HCKEYBOARD_KEY_STATE::RELEASED,  charIdle });

	keyboard->SetObject(&m_character, &funcs);
}

void HCCharacterControlTest::Update(float deltaTime)
{
	m_character.Update(deltaTime);
}

void HCCharacterControlTest::Render()
{
	m_character.Render();
}
