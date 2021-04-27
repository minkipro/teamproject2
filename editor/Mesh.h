#pragma once
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Texture.h"

struct RenderVertexSkeleton : public HC::InputDataSample
{
	virtual std::vector<HCInputLayoutElement>	GetInputData() const override
	{
		return { {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT},
				 {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT},
				 {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT},
				 {"BONEIDA",0,DXGI_FORMAT_R32G32B32A32_FLOAT},
				 {"BONEIDB",0,DXGI_FORMAT_R32G32B32A32_FLOAT },
				 {"BONEWEIGHTA",0,DXGI_FORMAT_R32G32B32A32_FLOAT},
				 {"BONEWEIGHTB",0,DXGI_FORMAT_R32G32B32A32_FLOAT },
				 {"TEXINDEX",0,DXGI_FORMAT_R32_SINT} };
	}

	virtual const char* GetInputName() const override { return typeid(RenderVertexSkeleton).name(); }
	virtual unsigned int						GetDataSize() const override { return sizeof(RenderPoint); }
	virtual int									GetTextureIndex() const override { return TextureIndex; }

	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 BoneIdA;
	DirectX::XMFLOAT4 BoneIdB;
	DirectX::XMFLOAT4 BoneWeightA;
	DirectX::XMFLOAT4 BoneWeightB;

	int TextureIndex = -1;
};

struct CB_VS_vertexshader_skeleton 
{
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX boneTransform[100];
};

struct Mesh
{
	Mesh(std::vector<RenderVertexSkeleton>& vertices, std::vector<DWORD>& indices, std::vector<Texture>& textures, const DirectX::XMMATRIX& transformMatrix, bool isTri);
	Mesh(const Mesh& mesh);

	bool m_isTri;
	std::vector<RenderVertexSkeleton>	m_vertices;
	std::vector<DWORD>					m_indices;
	std::vector<Texture>				m_textures;
	DirectX::XMMATRIX					m_transformMatrix;
};