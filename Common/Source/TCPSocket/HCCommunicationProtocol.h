#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <mutex>
#include <assert.h>

#define	MAX_BUFFER		8192 //보통 8k , 4k, 12k 가능
#define SERVER_PORT		24642
#define SERVER_IP		"121.128.250.128"

enum class HCTypeEnum : long
{
	HCchar		= 0,
	HCint		= 1,
	HCfloat		= 2,
	HCdouble	= 3,
	HCSizeT		= 4,
	HCSTRUCT1	= 5
};

struct HCStruct1
{
	int testData;
};

template<typename T>
HCTypeEnum GetTypeEnum(T data)
{
	if (typeid(T) == typeid(char))
	{
		return HCTypeEnum::HCchar;
	}
	else if (typeid(T) == typeid(int))
	{
		return HCTypeEnum::HCint;
	}
	else if (typeid(T) == typeid(float))
	{
		return HCTypeEnum::HCfloat;
	}
	else if (typeid(T) == typeid(double))
	{
		return HCTypeEnum::HCdouble;
	}
	else if (typeid(T) == typeid(size_t))
	{
		return HCTypeEnum::HCSizeT;
	}
	else if (typeid(T) == typeid(HCStruct1))
	{
		return HCTypeEnum::HCSTRUCT1;
	}
	assert("모든 타입을 설정해줘야 합니다.");
	return HCTypeEnum::HCchar;
}



template<typename T>
void HCDataToBuffer(char destBuffer[], unsigned int& bufferOffset, T& source)
{
	memcpy_s(&destBuffer[bufferOffset], MAX_BUFFER - bufferOffset, &source, sizeof(T));
	bufferOffset += sizeof(T);
}

template<typename T>
void HCDataToBuffer(char destBuffer[], unsigned int& bufferOffset, T source[], unsigned int arrNum)
{
	memcpy_s(&destBuffer[bufferOffset], MAX_BUFFER - bufferOffset, &source[0], sizeof(T) * (arrNum));
	bufferOffset += sizeof(T) * arrNum;
}

template<typename T>
void HCBufferToData(char sourceBuffer[], unsigned int& bufferOffset, T& out)
{
	memcpy_s(&out, sizeof(T), sourceBuffer, sizeof(T));
	bufferOffset += sizeof(T);
}

template<typename T>
unsigned int HCDataArangeForSend(T dataArr[], size_t arrNum, char out[])
{
	unsigned int bufferOffset = 0;
	if (arrNum == 0)
	{
		return bufferOffset;
	}
	HCTypeEnum dataType = GetTypeEnum(dataArr[0]);
	HCDataToBuffer(out, bufferOffset, dataType);
	HCDataToBuffer(out, bufferOffset, arrNum);
	HCDataToBuffer(out, bufferOffset, dataArr, arrNum);
	return bufferOffset;
}

template<typename T>
unsigned int HCDataArangeForSend(T data, char out[])
{
	size_t arrNum = 1;
	HCTypeEnum dataType = GetTypeEnum(data);
	unsigned int bufferOffset = 0;
	HCDataToBuffer(out, bufferOffset, dataType);
	HCDataToBuffer(out, bufferOffset, arrNum);
	HCDataToBuffer(out, bufferOffset, data);
	return bufferOffset;
}