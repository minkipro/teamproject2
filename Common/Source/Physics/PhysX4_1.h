#pragma once
#include "Device\HCPhysics.h"

class PhysX4_1 :public HCPhysics
{
public:
	PhysX4_1() {}
	virtual ~PhysX4_1() = default;

public:
	virtual void		Init() override;
	virtual void		Update() override;

	virtual void		SetCollider(IHCCollider* data, const std::wstring& sceneName) override;
	virtual void		TakeOutCollider(IHCCollider* data, const std::wstring& sceneName) override;
	virtual void		DeletedCollider(IHCCollider* data) override;

protected:
	std::vector<IHCCollider*>		m_colliders;
	std::vector<UINT>				m_remainingIndices;
};