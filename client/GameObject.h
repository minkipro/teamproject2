#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Graphics/Vertex.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/ConstantBuffer.h"

using namespace DirectX;
class GameObject
{
private:
	ConstantBuffer<CB_VS_vertexshader> constantBuffer;
	VertexBuffer<Vertex> vertexBuffer;
	IndexBuffer indicesBuffer;
	XMMATRIX wvp;
	XMFLOAT3 pos;
public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* dc);
	void Update(DirectX::XMMATRIX camera);
	void Draw(ID3D11DeviceContext* dc);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
};

