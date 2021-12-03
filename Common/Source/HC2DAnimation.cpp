#include "stdafx.h"
#include "HC2DAnimation.h"

int HC2DAnimation::Update(float deltaTime)
{
	int result = -1;

	if (m_spriteIndices.size())
	{
		UINT currIndex = static_cast<UINT>(m_stockTime / (m_animationTime / m_spriteIndices.size()));
		
		if (m_loopAnimation)
		{
			if (currIndex >= m_spriteIndices.size())
			{
				currIndex = 0;
				m_stockTime = 0;
			}
		}
		else
		{
			if (currIndex >= m_spriteIndices.size())
			{
				currIndex = SizeTTransUINT(m_spriteIndices.size() - 1);
			}
		}

		result = m_spriteIndices[currIndex] + m_textureData.spriteStartIndex;
	}

	m_stockTime += deltaTime;

	return result;
}

void HC2DAnimation::SetSpriteIndices(const std::vector<UINT>& sprites,const char* animName, float animationTime, bool loop)
{
	for (auto& it : sprites)
	{
		//COM_THROW_IF_FAILED(it < m_textureData.spriteNum, "Current animation's texture spriteNum is less than parameter sprite");
	}

	m_animName = animName;
	m_spriteIndices = sprites;
	m_animationTime = animationTime;
	m_stockTime = 0;
	m_loopAnimation = loop;
}

void HC2DAnimation::SetAnimationTime(float animationTime)
{
	m_animationTime = animationTime;
}

void HC2DAnimation::PushBackSpriteIndex(UINT sprite)
{
	COM_THROW_IF_FAILED(sprite < m_textureData.spriteNum, "Current animation's texture spriteNum is less than parameter sprite");

	m_spriteIndices.push_back(sprite);
}

void HC2DAnimation::Start()
{
	m_stockTime = 0;
}

bool HC2DAnimation::IsEnd()
{
	return m_loopAnimation ? false : (m_stockTime > m_animationTime);
}
