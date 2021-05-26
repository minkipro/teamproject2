#include "stdafx.h"
#include "HCPhysics.h"

HCColliderData::~HCColliderData()
{
	HCDEVICE(HCPhysics)->DeletedCollider(this);
}
