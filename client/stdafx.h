#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <DirectXMath.h>
#include "COMException.h"
#include "Engine.h"

using namespace std;
using namespace DirectX;
inline unsigned int SizeTTransUINT(size_t size)
{
	assert(UINT_MAX > size);
	return static_cast<unsigned int>(size);
}

inline int SizeTTransINT(size_t size)
{
	assert(INT_MAX > size);
	return static_cast<int>(size);
}