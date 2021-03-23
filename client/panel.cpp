#include "panel.h"
#include "Graphics/Vertex.h"
#include "ErrorLogger.h"

bool Panel::Initialize(ID3D11Device* device, ID3D11DeviceContext* dc, float x, float y, float width, float height, XMFLOAT4 color)
{
	pos = { x, y };
	size = { width, height };

	//12
	//03
	ColorVertex v[] =
	{
		ColorVertex(-50.0f,		 50.0f),
		ColorVertex(-50.0f,		-50.0f),
		ColorVertex(50.0f,		-50.0f),
		ColorVertex(50.0f,		 50.0f),
	};
	HRESULT hr = this->vertexBuffer.Initialize(device, v, ARRAYSIZE(v));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer.");
		return false;
	}

	DWORD indices[8] = { 0,1,1,2,2,3,3,0 };
	
	hr = this->indicesBuffer.Initialize(device, indices, 8);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create indices buffer.");
		return false;
	}

	hr = this->constantBuffer.Initialize(device, dc);
	COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");
	return true;
}

void Panel::Update(DirectX::XMMATRIX& ortho)
{
	XMMATRIX scale = XMMatrixScaling(size.x / 50.0f, size.y / 50.0f, 1.0f);
	XMMATRIX trans = XMMatrixTranslation(pos.x+size.x*0.5f, pos.y+size.y*0.5f, 0.0f);
	
	wvp = scale*trans*ortho;
}

void Panel::Draw(ID3D11DeviceContext* dc)
{
	constantBuffer.data.mat = wvp;
	constantBuffer.ApplyChanges();

	dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	dc->IASetIndexBuffer(indicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	dc->DrawIndexed(indicesBuffer.BufferSize(), 0, 0);
}
