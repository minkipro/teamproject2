#include "stdafx.h"
#include "Physics2D.h"

HCPhysics* Physics2DCollider::s_physx2D = nullptr;

void Physics2DCollider::Update(float delta)
{
	if (m_isPicking)
	{
		static_cast<Physics2D*>(s_physx2D)->ReservePicking(this);
	}
}

void Physics2DCollider::SetColliderType(HCCOLLIDER_TYPE type)
{
	m_type = type;
}

void Physics2DCollider::ColliderOn(bool Value)
{
	m_isCol = Value;
}

void Physics2DCollider::PickingOn(bool Value)
{
	m_isPicking = Value;
}

void Physics2D::Init()
{
	Physics2DCollider::SetPhysics2D(this);
}

void Physics2D::Update()
{

}

void Physics2D::PickingUpdate()
{
	auto				mouse = HCDEVICE(HCMouse);
	DirectX::XMFLOAT2	mousePos = mouse->GetMousePos();
	float				currDeltaTime = HCDEVICE(HCTimer)->GetDeltatime();

	IHCCollider*		currPickingColiider = nullptr;
	float				currDepth = 1.0f;

	for (size_t i = 0; i < m_pickingReservedCols.size(); i++)
	{
		const DirectX::XMFLOAT3 pos = *m_pickingReservedCols[i]->m_pos;

		if (pos.z < currDepth)
		{
			const DirectX::XMFLOAT2 size = m_pickingReservedCols[i]->m_size;

			switch (m_pickingReservedCols[i]->GetType())
			{
			case HCCOLLIDER_TYPE::HCCOLLIDER_TYPE_2D_RECTLT:
			{
				if (mousePos.x > pos.x && mousePos.x < pos.x + size.x &&
					mousePos.y > pos.y && mousePos.y < pos.y + size.y)
				{
					currPickingColiider = m_pickingReservedCols[i];
					currDepth = pos.z;
				}
			}
			break;
			case HCCOLLIDER_TYPE::HCCOLLIDER_TYPE_2D_RECTCENTER:
			{
				const DirectX::XMFLOAT2 ltPos = { pos.x - (size.x / 2), pos.y - (size.y / 2) };

				if (mousePos.x > ltPos.x && mousePos.x < ltPos.x + size.x &&
					mousePos.y > ltPos.y && mousePos.y < ltPos.y + size.y)
				{
					currPickingColiider = m_pickingReservedCols[i];
					currDepth = pos.z;
				}
			}
			break;
			case HCCOLLIDER_TYPE::HCCOLLIDER_TYPE_2D_CIRCLE:
			{
				DirectX::XMFLOAT2 vec = { mousePos.x - pos.x, mousePos.y - pos.y };

				if ((vec.x * vec.x) + (vec.y * vec.y) < (size.x * size.x))
				{
					currPickingColiider = m_pickingReservedCols[i];
					currDepth = pos.z;
				}
			}
			break;
			}
		}
	}

	if (currPickingColiider)
	{
		auto& colFuncs = currPickingColiider->m_pickingFuncs;

		for (auto& it : colFuncs)
		{
			if (mouse->GetButtonState(it.PickingMouseButton) == it.PickingMouseButtonState)
			{
				it.ColFunc(currDeltaTime);
			}
		}
	}

	m_pickingReservedCols.clear();
}

void Physics2D::CreateCollider(HCCOLLIDER_TYPE type, std::shared_ptr<IHCCollider>& out)
{
	std::shared_ptr<IHCCollider> result = std::make_unique<Physics2DCollider>(m_colliders.size());

	if (m_remainingIndices.size())
	{
		m_colliders[m_remainingIndices.back()] = result.get();
		m_remainingIndices.pop_back();
	}
	else
	{
		m_colliders.push_back(result.get());
	}

	out = result;
}

void Physics2D::DeleteCollider(IHCCollider* data)
{
	m_colliders[data->GetID()] = nullptr;
	m_remainingIndices.push_back(data->GetID());
}

void Physics2D::ReservePicking(Physics2DCollider* collider)
{
	m_pickingReservedCols.push_back(collider);
}
