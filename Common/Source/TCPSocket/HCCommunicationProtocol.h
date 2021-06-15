#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <mutex>

#define	MAX_BUFFER		8192 //���� 8k , 4k, 12k ����
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
	assert("��� Ÿ���� ��������� �մϴ�.");
	return HCTypeEnum::HCchar;
}

struct HCStruct1
{
	int testData;
};

template<typename T>
void HCDataToBuffer(char destBuffer[], unsigned int& bufferOffset, T& source)
{
	memcpy_s(&destBuffer[bufferOffset], MAX_BUFFER - bufferOffset, &source, sizeof(T));
	bufferOffset += sizeof(T);
}

template<typename T>
void HCDataToBuffer(char destBuffer[], unsigned int& bufferOffset, T source[], unsigned int arrNum, unsigned int& sourceOffset)
{
	memcpy_s(&destBuffer[bufferOffset], MAX_BUFFER - bufferOffset, &source[sourceOffset], sizeof(T)* (arrNum-sourceOffset));
	bufferOffset += sizeof(T)*arrNum;
	sourceOffset += arrNum;
}

template<typename T>
void HCDataArangeForSend(T dataArr[], size_t arrNum, char out[], unsigned int& dataIndexOffset)
{
	if (arrNum == 0)
	{
		return;
	}
	HCTypeEnum dataType = GetTypeEnum(dataArr[0]);
	unsigned int bufferOffset = 0;
	HCDataToBuffer(out, bufferOffset, dataType);
	HCDataToBuffer(out, bufferOffset, arrNum);
	HCDataToBuffer(out, bufferOffset, dataArr, arrNum, dataIndexOffset);
}

template<typename T>
void HCDataArangeForSend(T data, char out[])
{
	size_t arrNum =1;
	HCTypeEnum dataType = GetTypeEnum(data);
	unsigned int bufferOffset = 0;
	HCDataToBuffer(out, bufferOffset, dataType);
	HCDataToBuffer(out, bufferOffset, arrNum);
	HCDataToBuffer(out, bufferOffset, dataArr);
}