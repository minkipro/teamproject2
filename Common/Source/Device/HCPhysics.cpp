#include "stdafx.h"
#include "HCPhysics.h"

IHCCollider::~IHCCollider()
{
	HCDEVICE(HCPhysics)->DeletedCollider(this);
}
