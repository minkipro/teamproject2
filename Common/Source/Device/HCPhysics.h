#pragma once
#include "HCDevice.h"
#include <functional>

enum class HCCOLLIDERTYEP
{
	HCCOLLIDERTYPE_RECTLT,
	HCCOLLIDERTYPE_RECTCENTER,
	HCCOLLIDERTYPE_CIRCLE,
	HCCOLLIDERTYPE_PIXEL,
};

enum class HCCOLFUNCTYPE
{
	HCCOLFUNC_CONTACT,
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
	HCCOLFUNCTYPE			Type;
	std::function<void()>	ColFunc;
};

struct HCColliderData
{
	HCColliderData() = default;
	~HCColliderData();

	HCCOLLIDERTYEP						Type;
	DirectX::XMFLOAT2*					Pos = nullptr;
	std::vector<HCColFunc>				ColFuncs;

	union
	{
		DirectX::XMFLOAT2	Rect;
		float				CircleRad;
		int					RenderID;
	}Col;
};

class HCPhysics :public IHCDevice
{
public:
	HCPhysics() {}
	virtual ~HCPhysics() = default;

public:
	virtual void		Init() = 0;
	virtual void		Update() = 0;
	virtual std::string GetDeviceName() const = 0;

	virtual void		SetCollider(HCColliderData* data, const std::wstring& sceneName) = 0;
	virtual void		TakeOutCollider(HCColliderData* data, const std::wstring& sceneName) = 0;
	virtual void		DeletedCollider(HCColliderData* data) = 0;

protected:

};