#include "stdafx.h"
#include "HCMeshManager.h"

HCMeshManager HCMeshManager::s_instance;

void HCMeshManager::SetMesh(const std::string& name, const HCMesh& src)
{
	auto iter = m_meshs.find(name);

	COM_THROW_IF_FAILED(iter == m_meshs.end(), "this mesh name is already attached to MG");

	m_meshs[name] = std::make_unique<HCMesh>(src);
}

const HCMesh* HCMeshManager::GetMesh(const std::string& name)
{
	auto iter = m_meshs.find(name);

	COM_THROW_IF_FAILED(iter != m_meshs.end(), "this mesh name is not attached to MG");

	return iter->second.get();
}

void HCMeshManager::GetMeshNames(std::vector<std::string>& out)
{
	out.clear();

	for (auto& it : m_meshs)
	{
		out.push_back(it.first);
	}
}
