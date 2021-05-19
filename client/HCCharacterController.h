#pragma once
#include "DirectXMath.h"
#include <functional>
namespace HC
{
	class CharacterController
	{
	public:
		struct Keys // 키 매핑 상태
		{
			bool isKeyboard = true;//false면 mouse
			unsigned char data;
		};
		enum class BehaviorState : unsigned char// 키입력에 따른 행동만 정리
		{
			UP = 0,
			DOWN = 1,
			LEFT = 2,
			RIGHT = 3,
			A,
			B,
			C,
			D,
			COUNT
		};

		enum class KeyState : unsigned char// 키상태 // input에 필요안 것이기 떄문에 여기있어도 괜찮
		{
			ONCE = 0,
			PRESSED,
			RELEASED,
			COUNT
		};


		enum class CharacterState : unsigned char// 캐릭터 상태(움직임과 공격등 애니메이션에 관련된 상태)
		{
			UP = 0,
			DOWN,
			LEFT,
			RIGHT, //여기까지 위치는 변경하면 안됨
			IDLE,
			A,
			COUNT
		};

		enum class CharacterMoveState : unsigned char// 캐릭터 움직임 상태
		{
			UP = 0,
			DOWN = 1,
			LEFT = 2,
			RIGHT = 3,
			COUNT
		};
	public:
		CharacterController(DirectX::XMFLOAT3* position, DirectX::XMFLOAT4* uv, int indexSizeX, int indexSizeY, 
			std::vector<DirectX::XMUINT2> animationIndex[] /*CharacterState::COUNT크기의 배열을 무조건 요구함*/);
		void Update();
	private:
		DirectX::XMFLOAT3* m_position;
		DirectX::XMFLOAT4* m_uv;
		Keys m_keyMapping[(unsigned char)BehaviorState::COUNT];
		CharacterState m_state;
		CharacterState m_prevState;
		bool m_isThereInput;//인풋이 있는지 없는지
		bool m_continuos;//기존 상태와 일치하면 true 아니면 false
		std::vector<DirectX::XMFLOAT4> m_animationIndex[(unsigned char)CharacterState::COUNT];
		float m_deltaTime;
		float m_animationIndexChangeTime;
		int m_currentAnimationIndex;
		std::vector<std::function<void()>> m_behaviorStateFunction[(unsigned char)BehaviorState::COUNT][(unsigned char)KeyState::COUNT];
		bool m_characterMoveState[(unsigned char)CharacterMoveState::COUNT];
	private:
		size_t m_textIndex; // debug용
		bool m_buttonState[2];
	};
}