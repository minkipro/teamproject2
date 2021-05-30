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
	UINT noneUsed = 0;
};