#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<Vertex3D_Skeleton>& vertices, std::vector<DWORD>& indices, std::vector<Texture>& textures, const DirectX::XMMATRIX& transformMatrix, bool isTri)
{
	_deviceContext = deviceContext;
	_textures = textures;
	_transformMatrix = transformMatrix;

	auto graphic = HCDEVICE(HCGraphic);
	
	COM_HRESULT_IF_FAILED(_vertexbuffer.Initialize(device, vertices.data(), vertices.size()), "Failed to initialize vertex buffer for mesh.");

	COM_HRESULT_IF_FAILED(_indexbuffer.Initialize(device, indices.data(), indices.size()), "Failed to initialize index buffer for mesh.");
	_isTri = isTri;
}

Mesh::Mesh(const Mesh& mesh)
{
	_deviceContext = mesh._deviceContext;
	_indexbuffer = mesh._indexbuffer;
	_vertexbuffer = mesh._vertexbuffer;
	_textures = mesh._textures;
	_transformMatrix = mesh._transformMatrix;
	_isTri = mesh._isTri;

	

	graphic->
	graphic->CreateGraphicPipeLine
}

void Mesh::Update()
{


	UINT offset = 0;

	for (int i = 0; i < _textures.size(); i++)
	{
		if (_textures[i].GetType() == aiTextureType::aiTextureType_DIFFUSE)
		{
			_deviceContext->PSSetShaderResources(0, 1, _textures[i].GetTextureResourceViewAddress());
			break;
		}
	}
	if (_isTri)
	{
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	else
	{
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	}
	_deviceContext->IASetVertexBuffers(0, 1, _vertexbuffer.GetAddressOf(), _vertexbuffer.StridePtr(), &offset);
	_deviceContext->IASetIndexBuffer(_indexbuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	_deviceContext->DrawIndexed(_indexbuffer.IndexCount(), 0, 0);
}

const DirectX::XMMATRIX& Mesh::GetTransformMatirx()
{
	return _transformMatrix;
}
