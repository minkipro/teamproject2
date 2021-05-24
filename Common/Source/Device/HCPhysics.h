#pragma once
#include "HCDevice.h"
#include "HCScene.h"

enum HCCOLLIDERTYEP
{
	HCCOLLIDERTYPE_RECTLT,
	HCCOLLIDERTYPE_RECTCENTER,
	HCCOLLIDERTYPE_CIRCLE,
};

struct HCColliderData
{
	HCCOLLIDERTYEP		type;
	DirectX::XMFLOAT2*	pos = nullptr;
	union
	{
		DirectX::XMFLOAT2	rect;
		float				circleRad;
	}col;
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

protected:

};