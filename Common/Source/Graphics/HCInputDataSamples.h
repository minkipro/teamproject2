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

struct RenderPoint
{
	static std::vector<HCInputLayoutElement> InputLayout;

	DirectX::XMFLOAT3	Position;
	DirectX::XMFLOAT2	Size;
	DirectX::XMFLOAT4	Color;
	int					TextureIndex = -1;
};

std::vector<HCInputLayoutElement> RenderPoint::InputLayout = {
				{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT},
				{"RENDERSIZE",0,DXGI_FORMAT_R32G32_FLOAT},
				{"TEXCOORD",0,DXGI_FORMAT_R32G32B32A32_FLOAT},
				{"TEXINDEX",0,DXGI_FORMAT_R32_SINT} };