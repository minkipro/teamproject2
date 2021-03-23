#include "GameObject.h"
#include "Graphics/Vertex.h"

#include "ErrorLogger.h"
#include "TextureManager.h"

bool GameObject::Initialize(ID3D11Device* device, ID3D11DeviceContext* dc, TextureManager* textureManager, std::wstring filePath)
{
	pos = { 0.0f, 0.0f, 0.0f };

	texture = textureManager->SetTexture(device, filePath);
	
	Vertex v[] =
	{
		Vertex(-50.0f,  50.0f, 0.0f, 1.0f), //Bottom Left   - [0]
		Vertex(-50.0f,   -50.0f, 0.0f, 0.0f), //Top Left      - [1]
		Vertex(50.0f,   -50.0f, 1.0f, 0.0f), //Top Right     - [2]
		Vertex(50.0f,  50.0f, 1.0f, 1.0f), //Bottom Right   - [3]
	};
	HRESULT hr = this->vertexBuffer.Initialize(device, v, ARRAYSIZE(v));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}

	DWORD indices[6];
	IndexBuffer::GetIndexArray(indices, 4);
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

void GameObject::Update(DirectX::XMMATRIX& camera)
{
	wvp = XMMatrixTranslation(pos.x, pos.y, pos.z) * camera;
}

void GameObject::Draw(ID3D11DeviceContext* dc)
{
	constantBuffer.data.mat = wvp;
	constantBuffer.ApplyChanges();

	dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	dc->PSSetShaderResources(0, 1, texture);

	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	dc->IASetIndexBuffer(indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	dc->DrawIndexed(indicesBuffer.BufferSize(), 0, 0);
}
