#pragma once
#include "Device\HCPhysics.h"

class Physics2DCollider : public IHCCollider
{
public:
	Physics2DCollider(const UINT id)
		: IHCCollider(id)
		, m_type(HCCOLLIDER_TYPE::HCCOLLIDER_TYPE_2D_RECTLT)
	{
	}
	virtual ~Physics2DCollider() = default;

	virtual void			Update(float delta) override;

	virtual void			SetColliderType(HCCOLLIDER_TYPE type) override;
	virtual void			ColliderOn(bool Value) override;
	virtual void			PickingOn(bool Value) override;

	virtual HCCOLLIDER_TYPE GetType() override { return m_type; }

	static void SetPhysics2D(HCPhysics* device)
	{
		s_physx2D = device;
	}

private:
	static HCPhysics*	s_physx2D;
	HCCOLLIDER_TYPE		m_type;
	bool				m_isPicking = false;
	bool				m_isCol = false;
};

class Physics2D :public HCPhysics
{
public:
	Physics2D() {}
	virtual ~Physics2D() = default;

public:
	virtual void Init() override;
	virtual void Update() override;
	virtual void PickingUpdate() override;
				 
	virtual void CreateCollider(HCCOLLIDER_TYPE type, std::shared_ptr<IHCCollider>& out) override;
	virtual void DeleteCollider(IHCCollider* collider) override;
	
	void ReservePicking(Physics2DCollider* collider);

protected:
	std::vector<IHCCollider*>		m_colliders;
	std::vector<IHCCollider*>		m_pickingReservedCols;
	std::vector<UINT>				m_remainingIndices;
};