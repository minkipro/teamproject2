#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh(std::vector<RenderVertexSkeleton>& vertices, std::vector<DWORD>& indices, std::vector<Texture>& textures, const DirectX::XMMATRIX& transformMatrix, bool isTri)
{
	m_vertices = vertices;
	m_indices = indices;
	m_textures = textures;
	m_isTri = isTri;
}

Mesh::Mesh(const Mesh& mesh)
{
	m_vertices = mesh.m_vertices;
	m_indices = mesh.m_indices;
	m_textures = mesh.m_textures;
	m_isTri = mesh.m_isTri;
}