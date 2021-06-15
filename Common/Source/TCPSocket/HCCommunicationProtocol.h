#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <mutex>

#define	MAX_BUFFER		8192 //보통 8k , 4k, 12k 가능
#define SERVER_PORT		24642
#define SERVER_IP		"121.128.250.128"

enum class HCTypeEnum : long
{
	HCchar = 0,
	HCint = 1,
	HCfloat = 2,
	HCdouble = 3,
	HCSTRUCT1 = 4
};

struct HCStruct1
{
	int testData;
};

template<typename T>
void HCDataToBuffer(char dataBuffer[], unsigned int& bufferOffset, T& out)
{
	memcpy_s(&dataBuffer[bufferOffset], MAX_BUFFER - bufferOffset, &out, sizeof(T));
	bufferOffset += sizeof(T);
}

template<typename T>
void HCDataToBuffer(char dataBuffer[], unsigned int& bufferOffset, T out[], unsigned int arrNum, unsigned int& dataIndexOffset)
{
	memcpy_s(&dataBuffer[bufferOffset], MAX_BUFFER - bufferOffset, &out[dataIndexOffset], sizeof(T)* (arrNum-dataIndexOffset));
	bufferOffset += sizeof(T)*arrNum;
	dataIndexOffset += arrNum;
}

template<typename T>
void HCBufferToData(char outdataBuffer[], unsigned int& bufferOffset, T& in)
{
		
}

template<typename T>
void HCBufferToData(char outdataBuffer[], unsigned int& bufferOffset, T in[], unsigned int arrNum, unsigned int& dataIndexOffset)
{

}

template<typename T>
void HCDataArangeForSend(T dataArr[], size_t arrNum, char out[])
{
	HCTypeEnum dataType = HCTypeEnum::HCchar;
	if (typeid(T) == typeid(char))
	{
		dataType = HCTypeEnum::HCchar;
	}
	else if (typeid(T) == typeid(int))
	{
		dataType = HCTypeEnum::HCint;
	}
	else if (typeid(T) == typeid(float))
	{
		dataType = HCTypeEnum::HCfloat;
	}
	else if (typeid(T) == typeid(double))
	{
		dataType = HCTypeEnum::HCdouble;
	}
	else if (typeid(T) == typeid(HCStruct1))
	{
		dataType = HCTypeEnum::HCSTRUCT1;
	}
	unsigned int bufferOffset = 0;
	unsigned int dataIndexOffset = 0;
	HCDataToBuffer(out, bufferOffset, dataType);
	HCDataToBuffer(out, bufferOffset, dataArr, arrNum, dataIndexOffset);
}

template<typename T>
void HCGetDataArr(char buffer[], void* outDataArr, std::mutex* ourMutex)
{
	char curBuffer[MAX_BUFFER];
	ourMutex->lock();
	memcpy_s(curBuffer, MAX_BUFFER, buffer, MAX_BUFFER);
	ourMutex->unlock();

	unsigned int bufferOffset = 0;

	HCTypeEnum dataType = HCTypeEnum::HCchar;
	HCDataToBuffer(curBuffer, bufferOffset, dataType);

	unsigned int arrNum = 0;
	HCDataToBuffer(curBuffer, bufferOffset, arrNum);

	outData.resize(arrNum);
	HCDataToBuffer()
}