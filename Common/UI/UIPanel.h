#pragma once
#include "UIButton.h"
#include "UIParam.h"
#include <list>

class UIPanel :public UIObject
{
public:
	UIPanel()
	{
	}

	virtual			~UIPanel() = default;

	virtual void	Init() override;
	virtual void	Update() override;

	void			SetBackGroundColor(const DirectX::XMFLOAT4 color);
	void			SetName(const std::wstring& name);

protected:
};
