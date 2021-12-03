#include "stdafx.h"
#include "HCCharacterControlTest.h"
#include "Graphics/HXmlAnimationManager.h"

void HCCharacterControlTest::Init(const wchar_t* textureName)
{
	enum
	{
		ANI_READY,
		ANI_ANGRY,
		ANI_CRY,
		ANI_WALK,
		ANI_SAD,
		ANI_RUN,
		ANI_JUMP,
		ANI_SEAT,
		ANI_STAND,
		ANI_IDLE,
		ANI_STRECTCHING,
		ANI_COUNT,
	};

	m_animations.resize(ANI_COUNT);
	HCTextureData textureData = HCDEVICE(HCGraphic)->GetTextureIndex(textureName);
	for (auto& it : m_animations)
	{
		it.SetTexture(textureData);
	}

	const float baseAniTime = 0.3f;

	auto& curActionMap = HXmlAnimationManager::Get()->m_actionCharacterMap["sp_cat.png"];
	
	int offset = 0;
	for (auto& it : curActionMap)
	{
		m_animations[offset++].SetSpriteIndices(it.second.indices, it.second.time, it.second.isRepeat);
	}
	//m_animations[ANI_IDLE].SetSpriteIndices({ 1 }, baseAniTime, true);
	//m_animations[ANI_UP].SetSpriteIndices({ 9, 10, 11 }, baseAniTime, true);
	//m_animations[ANI_LEFT].SetSpriteIndices({ 3, 4, 5 }, baseAniTime, true);
	//m_animations[ANI_DOWN].SetSpriteIndices({ 0, 1, 2 }, baseAniTime, true);
	//m_animations[ANI_RIGHT].SetSpriteIndices({ 6, 7, 8 }, baseAniTime, true);

	m_character.Init();
	m_character.SetZ(0.1f);
	m_character.SetSize({ 100.0f, 100.0f });
	m_character.SetAnimation(&m_animations[ANI_IDLE]);

	auto charIdle = [this](void* character, float deltaTime)->void
	{
		static_cast<HC2DCharacter*>(character)->SetAnimation(&m_animations[ANI_IDLE]);
	};

	auto charMoveLeft = [this](void* character, float deltaTime)->void
	{
		DirectX::XMFLOAT2 pos = { -200.0f * deltaTime, 0.0f };
		static_cast<HC2DCharacter*>(character)->SetAnimation(&m_animations[ANI_RUN]);
		static_cast<HC2DCharacter*>(character)->AddPos(pos);
	};

	auto charMoveRight = [this](void* character, float deltaTime)->void
	{
		DirectX::XMFLOAT2 pos = { 200.0f * deltaTime, 0.0f };
		static_cast<HC2DCharacter*>(character)->SetAnimation(&m_animations[ANI_RUN]);
		static_cast<HC2DCharacter*>(character)->AddPos(pos);
	};

	auto keyboard = HCDEVICE(HCKeyboard);
	std::vector<HCKEYBOARD_CONTROL_FUNC> funcs;
;
	funcs.push_back( { HCKEYBOARD_KEY_TYPE::A, HCKEYBOARD_KEY_STATE::PRESS_HELD,  charMoveLeft });
	funcs.push_back( { HCKEYBOARD_KEY_TYPE::D, HCKEYBOARD_KEY_STATE::PRESS_HELD,  charMoveRight });

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
