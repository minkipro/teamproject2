#pragma once
#include "HCGraphic.h"

class HCMeshManager final
{
public:
	static HCMeshManager* Get() { return &s_instance; }

public:
	void			SetMesh(const std::string& name, const HCMesh& src);

	const HCMesh*	GetMesh(const std::string& name);
	void			GetMeshNames(std::vector<std::string>& out);

private:
	HCMeshManager() = default;
	~HCMeshManager() = default;

private:
	static HCMeshManager s_instance;

	std::unordered_map<std::string, std::unique_ptr<HCMesh>> m_meshs;
};