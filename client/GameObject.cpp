#include "GameObject.h"
#include "Graphics/Vertex.h"
#include <WICTextureLoader.h>
#include "ErrorLogger.h"
bool GameObject::Initialize(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	pos = { 0.0f, 0.0f, 0.0f };
	//Load Texture
	HRESULT hr = DirectX::CreateWICTextureFromFile(device, L"Data\\Textures\\character.gif", nullptr, texture.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create wic texture from file.");
		return false;
	}
	Vertex v[] =
	{
		Vertex(0.0f,  100.0f, 0.0f, 1.0f), //Bottom Left   - [0]
		Vertex(0.0f,   0.0f, 0.0f, 0.0f), //Top Left      - [1]
		Vertex(100.0f,   0.0f, 1.0f, 0.0f), //Top Right     - [2]
		Vertex(100.0f,  100.0f, 1.0f, 1.0f), //Bottom Right   - [3]
	};
	hr = this->vertexBuffer.Initialize(device, v, ARRAYSIZE(v));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}

	DWORD indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};
	hr = this->indicesBuffer.Initialize(device, indices, ARRAYSIZE(indices));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create indices buffer.");
		return false;
	}

	hr = this->constantBuffer.Initialize(device, dc);
	COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");
	return true;
}

void GameObject::Update(DirectX::XMMATRIX camera)
{
	wvp = XMMatrixTranslation(pos.x, pos.y, pos.z) * camera;
}

void GameObject::Draw(ID3D11DeviceContext* dc)
{
	constantBuffer.data.mat = wvp;
	constantBuffer.ApplyChanges();

	dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	dc->PSSetShaderResources(0, 1, texture.GetAddressOf());

	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	dc->IASetIndexBuffer(indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	dc->DrawIndexed(indicesBuffer.BufferSize(), 0, 0);
}
