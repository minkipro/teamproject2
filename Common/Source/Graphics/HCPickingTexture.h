#pragma once
#include "HCGraphic.h"

class HCPickingTexture
{
public:
	HCPickingTexture()
	{
		m_size = {};
	}
	~HCPickingTexture() = default;

	void Init(UINT sizeX, UINT sizeY);
	void Update();
	void CopyGpuDataToCpu();

	std::shared_ptr<IHCResource>	GetRenderTargetTexture() { return m_renderTargetTexture; }
	int								GetData(UINT indexX, UINT indexY);
	DirectX::XMUINT2				GetTextureSize() { return m_size; }

private:
	std::shared_ptr<IHCResource>	m_renderTargetTexture;
	std::shared_ptr<IHCResource>	m_stagingTexture;
	DirectX::XMUINT2				m_size;
	std::vector<int>				m_data;
};