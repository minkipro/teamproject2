#pragma once
#include "DirectXMath.h"
namespace HC
{
	class CharacterController
	{
	public:
		CharacterController(DirectX::XMFLOAT3* position);
		void Update();
	private:
		DirectX::XMFLOAT3* m_position;
	};
}