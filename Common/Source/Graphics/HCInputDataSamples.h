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

struct HCPositionVertex
{
	static std::vector<HCInputLayoutElement> InputLayout;
	DirectX::XMFLOAT3	Position = {};
	int					Pad = 0;
};

struct HCPointRenderInfo
{
	DirectX::XMFLOAT3	Position = { 0,0,0 };
	DirectX::XMFLOAT2	Size = { 0,0 };
	DirectX::XMFLOAT4	Color = { 0,0,0,1 };
	int					TextureIndex = -1;
	UINT				SpriteIndex = 0;
	UINT				Pad1 = 0;
};