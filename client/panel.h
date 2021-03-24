#pragma once
#include <d3d11.h>
#include "Graphics/Vertex.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/ConstantBuffer.h"
using namespace DirectX;
class Panel
{
private:
	ConstantBuffer<CB_VS_vertexshader> constantBuffer;
	VertexBuffer<ColorVertex> vertexBuffer;
	IndexBuffer indicesBuffer;
	XMMATRIX wvp;
	XMFLOAT2 pos;
	XMFLOAT2 size;

public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* dc, float x, float y, float width, float height, XMFLOAT4 color);
	void Update(DirectX::XMMATRIX& ortho);
	void Draw(ID3D11DeviceContext* dc);
};

