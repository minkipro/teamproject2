#pragma once
#include "DirectXMath.h"
#include <functional>
#include "HCCharacterController.h"
namespace HC
{
	class CharacterControllerByKeyboard : public HC::CharacterController
	{
	public:
		enum class CharacterState : unsigned char// ĳ���� ����(�����Ӱ� ���ݵ� �ִϸ��̼ǿ� ���õ� ����)
		{
			UP = 0,
			DOWN,
			LEFT,
			RIGHT, //������� ��ġ�� �����ϸ� �ȵ�
			IDLE,
			A,
			HCSHADER_COUNT
		};

		enum class CharacterMoveState : unsigned char// ĳ���� ������ ����
		{
			UP = 0,
			DOWN = 1,
			LEFT = 2,
			RIGHT = 3,
			HCSHADER_COUNT
		};

		enum class BehaviorState : unsigned char// �Է¿� ���� �ൿ�� ����
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

		struct Keys // Ű ���� ����
		{
			bool isKeyboard = true;//false�� mouse
			unsigned char data;
		};

		enum class KeyState : unsigned char// Ű���� // input�� �ʿ�� ���̱� ������ �����־ ����
		{
			ONCE = 0,
			PRESSED,
			RELEASED,
			HCSHADER_COUNT
		};
		
	public:
		CharacterControllerByKeyboard(DirectX::XMFLOAT3* position, int* textureIndex, int spriteNum, 
			std::vector<int> animationIndex[] /*CharacterState::COUNTũ���� �迭�� ������ �䱸��*/);
		virtual void Update() override;

	private:
		Keys m_keyMapping[(unsigned char)BehaviorState::HCSHADER_COUNT];
		std::vector<std::function<void()>> m_behaviorStateFunction[(unsigned char)BehaviorState::HCSHADER_COUNT][(unsigned char)KeyState::HCSHADER_COUNT];

		//����
		CharacterState m_state;
		CharacterState m_prevState;
		bool m_isThereInput;//��ǲ�� �ִ��� ������
		bool m_continuos;//���� ���¿� ��ġ�ϸ� true �ƴϸ� false
		bool m_characterMoveState[(unsigned char)CharacterMoveState::HCSHADER_COUNT];

		//animation���� ����
		DirectX::XMFLOAT3* m_position;
		int* m_textureIndex;
		int m_originTextureIndex;
		std::vector<int> m_animationIndex[(unsigned char)CharacterState::HCSHADER_COUNT];
		float m_deltaTime;//�ִϸ��̼��� �ٲٱ����� �����Ǵ� �ð�
		float m_animationIndexChangeTime;
		int m_currentAnimationIndex;

		// debug��
		std::shared_ptr<IHCTextData> m_textRender; 
		bool m_buttonState[2];
	};
}