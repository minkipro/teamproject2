#pragma once
#include <DirectXMath.h>

struct CB_VS_vertexshader
{
	DirectX::XMMATRIX mat;
};

struct CB_VS_color_vertexshader
{
	DirectX::XMMATRIX mat;
	DirectX::XMFLOAT4 color;
};

