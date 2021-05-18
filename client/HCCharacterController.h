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
		enum class BehaviorState // Ű�Է¿� ���� �ൿ�� ����
		{
			UP = 0,
			DOWN,
			LEFT,
			RIGHT,
			A,
			B,
			C,
			D,
			COUNT
		};

		enum class KeyState // Ű����
		{
			ONCE = 0,
			PRESSED,
			RELEASED,
			COUNT
		};

		enum class CharacterState // ĳ���� ����(�����Ӱ� ���ݵ� �ִϸ��̼ǿ� ���õ� ����)
		{
			UP = 0,
			DOWN,
			LEFT,
			RIGHT, //������� ��ġ�� �����ϸ� �ȵ�
			IDLE,
			COUNT
		};

		enum class CharacterMoveState // ĳ���� ������ ����
		{
			UP = 0,
			DOWN,
			LEFT,
			RIGHT,
			COUNT
		};
	public:
		CharacterController(DirectX::XMFLOAT3* position, DirectX::XMFLOAT4* uv, int indexSizeX, int indexSizeY, std::vector<DirectX::XMUINT2> animationIndex[]);
		void Update();
	private:
		DirectX::XMFLOAT3* m_position;
		DirectX::XMFLOAT4* m_uv;
		Keys m_keyMapping[(unsigned long long)BehaviorState::COUNT];
		CharacterState m_state;
		CharacterState m_prevState;
		bool m_isThereInput;//��ǲ�� �ִ��� ������
		bool m_continuos;//���� ���¿� ��ġ�ϸ� true �ƴϸ� false
		std::vector<DirectX::XMFLOAT4> m_animationIndex[(unsigned long long)CharacterState::COUNT];
		float m_deltaTime;
		float m_animationIndexChangeTime;
		int m_currentAnimationIndex;
		std::vector<std::function<void()>> m_behaviorStateFunction[(unsigned long long)BehaviorState::COUNT][(unsigned long long)KeyState::COUNT];
		bool m_characterMoveState[(unsigned long long)CharacterMoveState::COUNT];
	private:
		size_t m_textIndex; // debug��
	};
}