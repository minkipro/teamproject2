#pragma once
#include "HCDevice.h"
#include <functional>

enum class HCCOLLIDER_TYPE
{
	HCCOLLIDERTYPE_RECTLT,
	HCCOLLIDERTYPE_RECTCENTER,
	HCCOLLIDERTYPE_CIRCLE,
};

enum class HCCOL_FUNC_TYPE
{
	HCCOLFUNC_CONTACT,
	HCCOLFUNC_CONTACTING,
	HCCOLFUNC_UNTACT,
	HCCOLFUNC_PICKING_LEFT_PRESE,
	HCCOLFUNC_PICKING_LEFT_RELEASE,
	HCCOLFUNC_PICKING_LEFT_HELD,
	HCCOLFUNC_PICKING_RIGHT_PRESE,
	HCCOLFUNC_PICKING_RIGHT_RELEASE,
	HCCOLFUNC_PICKING_RIGHT_HELD,
};

struct HCColFunc
{
	HCCOL_FUNC_TYPE			Type;
	std::function<void()>	ColFunc;
};

struct IHCCollider
{
	IHCCollider() = default;
	virtual ~IHCCollider();

private:
	HCCOLLIDER_TYPE			Type = HCCOLLIDER_TYPE::HCCOLLIDERTYPE_RECTLT;
	DirectX::XMFLOAT3*		Pos = nullptr;
	std::vector<HCColFunc>	ColFuncs;

	const int				ColIdFromDevice = -1;
};

class HCPhysics :public IHCDevice
{
public:
	HCPhysics() {}
	virtual ~HCPhysics() = default;

public:
	virtual void		Init() = 0;
	virtual void		Update() = 0;

	virtual void		CreateCollider(HCCOLLIDER_TYPE type, IHCCollider** out) = 0;
	virtual void		SetCollider(IHCCollider* data, const std::wstring& sceneName) = 0;
	virtual void		TakeOutCollider(IHCCollider* data, const std::wstring& sceneName) = 0;
	virtual void		DeletedCollider(IHCCollider* data) = 0;

private:
	virtual std::string GetDeviceName() const override { return typeid(HCPhysics).name(); }
};