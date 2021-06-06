#pragma once
#include <vector>

struct HCInputLayoutElement
{
	HCInputLayoutElement(const char* semanticName, UINT semanticIndex, DXGI_FORMAT format)
	{
		SemanticName = semanticName;
		SemanticIndex = semanticIndex;
		Format = format;
	}

	std::string SemanticName;
	UINT		SemanticIndex;
	DXGI_FORMAT Format;
};

struct HCOnePointExtToRect
{
	static std::vector<HCInputLayoutElement> InputLayout;
	DirectX::XMUINT4 noneUsed = {};
};

struct HCPointRenderInfo
{
	DirectX::XMFLOAT3	Position = { 0,0,0 };
	DirectX::XMFLOAT2	Size = { 0,0 };
	DirectX::XMFLOAT4	Color = { 0,0,0,1 };
	int					SpriteInfoIndex = -1;
	int					PixelColId = -1;
	int					Pad1 = 0;
};