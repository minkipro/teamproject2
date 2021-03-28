#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

class IndexBuffer
{
private:
	IndexBuffer(const IndexBuffer& rhs);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT bufferSize = 0;
public:
	IndexBuffer() {}
	static void GetIndexArray(DWORD* indexArray, int vertexNum)
	{
		if (vertexNum % 4 != 0)
			return;
		for (int i = 0; i < vertexNum / 4; i++)
		{
			indexArray[0 + i * 6] = 0 + i * 4;
			indexArray[1 + i * 6] = 1 + i * 4;
			indexArray[2 + i * 6] = 2 + i * 4;
			indexArray[3 + i * 6] = 0 + i * 4;
			indexArray[4 + i * 6] = 2 + i * 4;
			indexArray[5 + i * 6] = 3 + i * 4;
		}
	}
	ID3D11Buffer* Get()const
	{
		return buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf()const
	{
		return buffer.GetAddressOf();
	}

	UINT BufferSize() const
	{
		return this->bufferSize;
	}

	HRESULT Initialize(ID3D11Device* device, DWORD* data, UINT numIndices)
	{
		this->bufferSize = numIndices;
		//Load Index Data
		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * numIndices;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexBufferData;
		indexBufferData.pSysMem = data;
		HRESULT hr = device->CreateBuffer(&indexBufferDesc, &indexBufferData, buffer.GetAddressOf());
		return hr;
	}
};