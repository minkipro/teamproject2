#pragma once
#include "HCGraphic.h"
#include <SpriteFont.h>
class HCFont : public IHCFont
{
public:

	HCFont() = default;
	virtual ~HCFont() = default;
	virtual void Init(void* device, void* dc) override;
	virtual size_t GetFontNum() override;
	virtual void GetFontNames(std::vector<std::wstring>& out) override;
	virtual void SetFont(unsigned int index) override;
	virtual void SetFont(std::wstring fileName) override;
	virtual size_t AddText(const IHCFont::TextData& textData) override;//삭제 필요
	virtual size_t AddText() override;
	virtual void SetText(int index, const wchar_t* text) override;
	virtual void Render() override;
private:
	std::unordered_map<std::wstring, std::unique_ptr<DirectX::SpriteFont>> m_spriteFonts;
	DirectX::SpriteFont* m_currentFont = nullptr;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	std::vector<IHCFont::TextData> m_texts;//삭제 필요
};