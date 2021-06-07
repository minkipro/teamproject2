#pragma once
#include "HCMouse.h"
#include <DirectXMath.h>
#include <functional>

enum class HCCOLLIDER_TYPE
{
	HCCOLLIDER_TYPE_2D_RECTLT,
	HCCOLLIDER_TYPE_2D_RECTCENTER,
	HCCOLLIDER_TYPE_2D_CIRCLE,
};

enum class HCCOL_FUNC_TYPE
{
	HCCOLFUNC_CONTACT,
	HCCOLFUNC_CONTACTING,
	HCCOLFUNC_UNTACT,
};

struct HCColFunc
{
	HCCOL_FUNC_TYPE				Type;
	HCMOUSE_BUTTON_TYPE			PickingMouseButton = HCMOUSE_BUTTON_TYPE::COUNT;
	HCMOUSE_BUTTON_STATE		PickingMouseButtonState;
	std::function<void(float)>	ColFunc;
};

struct HCPickingFunc
{
	HCMOUSE_BUTTON_TYPE			PickingMouseButton = HCMOUSE_BUTTON_TYPE::LBUTTON;
	HCMOUSE_BUTTON_STATE		PickingMouseButtonState = HCMOUSE_BUTTON_STATE::RELEASED;
	std::function<void(float)>	ColFunc;
};

class IHCCollider
{
public:
	IHCCollider(const UINT id)
		:m_colIdFromDevice(id)
	{
	}
	virtual ~IHCCollider() = default;

	virtual void			Update(float delta) = 0;

	virtual void			SetColliderType(HCCOLLIDER_TYPE type) = 0;
	virtual void			ColliderOn(bool Value) = 0;
	virtual void			PickingOn(bool Value) = 0;

	virtual HCCOLLIDER_TYPE GetType() = 0;
	UINT					GetID() { return m_colIdFromDevice; }

	DirectX::XMFLOAT3*			m_pos = nullptr;
	DirectX::XMFLOAT2			m_size = {};
	std::vector<HCColFunc>		m_colFuncs;
	std::vector<HCPickingFunc>	m_pickingFuncs;

protected:
	const UINT					m_colIdFromDevice;
};

class HCPhysics :public IHCDevice
{
public:
	HCPhysics() {}
	virtual ~HCPhysics() = default;

public:
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void PickingUpdate() = 0;
				 
	virtual void CreateCollider(HCCOLLIDER_TYPE type, std::shared_ptr<IHCCollider>& out) = 0;
	virtual void DeleteCollider(IHCCollider* collider) = 0;

private:
	virtual std::string GetDeviceName() const override { return typeid(HCPhysics).name(); }
};