#pragma once
#include "DirectXMath.h"
#include <functional>
namespace HC
{
	class CharacterController
	{
	public:
		struct Keys // Ű ���� ����
		{
			bool isKeyboard = true;//false�� mouse
			unsigned char data;
		};
		enum class BehaviorState : unsigned char// Ű�Է¿� ���� �ൿ�� ����
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

		enum class KeyState : unsigned char// Ű���� // input�� �ʿ�� ���̱� ������ �����־ ����
		{
			ONCE = 0,
			PRESSED,
			RELEASED,
			COUNT
		};


		enum class CharacterState : unsigned char// ĳ���� ����(�����Ӱ� ���ݵ� �ִϸ��̼ǿ� ���õ� ����)
		{
			UP = 0,
			DOWN,
			LEFT,
			RIGHT, //������� ��ġ�� �����ϸ� �ȵ�
			IDLE,
			A,
			COUNT
		};

		enum class CharacterMoveState : unsigned char// ĳ���� ������ ����
		{
			UP = 0,
			DOWN = 1,
			LEFT = 2,
			RIGHT = 3,
			COUNT
		};
	public:
		CharacterController(DirectX::XMFLOAT3* position, DirectX::XMFLOAT4* uv, int indexSizeX, int indexSizeY, 
			std::vector<DirectX::XMUINT2> animationIndex[] /*CharacterState::COUNTũ���� �迭�� ������ �䱸��*/);
		void Update();
	private:
		DirectX::XMFLOAT3* m_position;
		DirectX::XMFLOAT4* m_uv;
		Keys m_keyMapping[(unsigned char)BehaviorState::COUNT];
		CharacterState m_state;
		CharacterState m_prevState;
		bool m_isThereInput;//��ǲ�� �ִ��� ������
		bool m_continuos;//���� ���¿� ��ġ�ϸ� true �ƴϸ� false
		std::vector<DirectX::XMFLOAT4> m_animationIndex[(unsigned char)CharacterState::COUNT];
		float m_deltaTime;
		float m_animationIndexChangeTime;
		int m_currentAnimationIndex;
		std::vector<std::function<void()>> m_behaviorStateFunction[(unsigned char)BehaviorState::COUNT][(unsigned char)KeyState::COUNT];
		bool m_characterMoveState[(unsigned char)CharacterMoveState::COUNT];
	private:
		size_t m_textIndex; // debug��
		bool m_buttonState[2];
	};
}