#pragma once

class HC2DAnimation
{
public:
	HC2DAnimation()
		: m_animationTime(0)
		, m_stockTime(0)
		, m_loopAnimation(false)
	{
	}
	virtual ~HC2DAnimation() = default;

	int  Update(float deltaTime); //return spriteInfoIndex
	
	void SetTexture(const HCTextureData& textureData) { m_textureData = textureData; }
	void SetSpriteIndices(const std::vector<UINT>& sprites, float animationTime, bool loop);
	void SetAnimationTime(float animationTime);
	void SetLoopAnimation(bool loop) { m_loopAnimation = loop; }
	void PushBackSpriteIndex(UINT sprite);
	
	int	 GetTextureIndex() const { return m_textureData.textureIndex; }

	void Start();
	bool IsEnd();

private:
	HCTextureData		m_textureData;
	std::vector<UINT>	m_spriteIndices;
	float				m_animationTime;
	float				m_stockTime;
	bool				m_loopAnimation;
};