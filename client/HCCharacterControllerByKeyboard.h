#pragma once
#include "DirectXMath.h"
#include <functional>
#include "HCCharacterController.h"
namespace HC
{
	class CharacterControllerByKeyboard : public HC::CharacterController
	{
	public:
		enum class CharacterState : unsigned char// 캐릭터 상태(움직임과 공격등 애니메이션에 관련된 상태)
		{
			UP = 0,
			DOWN,
			LEFT,
			RIGHT, //여기까지 위치는 변경하면 안됨
			IDLE,
			A,
			HCSHADER_COUNT
		};

		enum class CharacterMoveState : unsigned char// 캐릭터 움직임 상태
		{
			UP = 0,
			DOWN = 1,
			LEFT = 2,
			RIGHT = 3,
			HCSHADER_COUNT
		};

		enum class BehaviorState : unsigned char// 입력에 따른 행동만 정리
		{
			UP = 0,
			DOWN = 1,
			LEFT = 2,
			RIGHT = 3,
			A,
			B,
			C,
			D,
			HCSHADER_COUNT
		};

		struct Keys // 키 매핑 상태
		{
			bool isKeyboard = true;//false면 mouse
			unsigned char data;
		};

		enum class KeyState : unsigned char// 키상태 // input에 필요안 것이기 떄문에 여기있어도 괜찮
		{
			ONCE = 0,
			PRESSED,
			RELEASED,
			HCSHADER_COUNT
		};
		
	public:
		CharacterControllerByKeyboard(DirectX::XMFLOAT3* position, int* textureIndex, int spriteNum, 
			std::vector<int> animationIndex[] /*CharacterState::COUNT크기의 배열을 무조건 요구함*/);
		virtual void Update() override;

	private:
		Keys m_keyMapping[(unsigned char)BehaviorState::HCSHADER_COUNT];
		std::vector<std::function<void()>> m_behaviorStateFunction[(unsigned char)BehaviorState::HCSHADER_COUNT][(unsigned char)KeyState::HCSHADER_COUNT];

		//상태
		CharacterState m_state;
		CharacterState m_prevState;
		bool m_isThereInput;//인풋이 있는지 없는지
		bool m_continuos;//기존 상태와 일치하면 true 아니면 false
		bool m_characterMoveState[(unsigned char)CharacterMoveState::HCSHADER_COUNT];

		//animation관련 변수
		DirectX::XMFLOAT3* m_position;
		int* m_textureIndex;
		int m_originTextureIndex;
		std::vector<int> m_animationIndex[(unsigned char)CharacterState::HCSHADER_COUNT];
		float m_deltaTime;//애니메이션을 바꾸기위해 누적되는 시간
		float m_animationIndexChangeTime;
		int m_currentAnimationIndex;

		// debug용
		std::shared_ptr<IHCTextData> m_textRender; 
		bool m_buttonState[2];
	};
}