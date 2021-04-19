#pragma once
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Texture.h"

class Mesh
{
public:
	Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<>& vertices, std::vector<DWORD>& indices, std::vector<Texture>& textures, const DirectX::XMMATRIX& transformMatrix, bool isTri);
	Mesh(const Mesh& mesh);
	void Update();
	const DirectX::XMMATRIX& GetTransformMatirx();
private:
	bool _isTri;
	//VertexBuffer<Vertex3D_Skeleton>	_vertexbuffer;
	//IndexBuffer				_indexbuffer;
	ID3D11DeviceContext* _deviceContext;
	std::vector<Texture>	_textures;
	DirectX::XMMATRIX		_transformMatrix;
};