#pragma once
#include "HCGraphic.h"
#include <SpriteFont.h>
class HCFont : public IHCFont
{
public:

	HCFont() = default;
	virtual ~HCFont() = default;
	virtual void Init(void* device, void* dc) override;
	virtual void SetText(const IHCFont::TextData& textData) override;
	virtual void Render() override;
private:
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;
	std::vector<IHCFont::TextData> m_texts;
};