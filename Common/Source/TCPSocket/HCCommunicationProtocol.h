#pragma once
#define	MAX_BUFFER		1024
#define SERVER_PORT		24642
#define SERVER_IP		"121.128.250.128"

enum class HCTypeEnum : long
{
	HCchar = 0,
	HCint = 1,
	HCfloat = 2,
	HCdouble = 3
};
enum class HCDataFormat : long
{
	IP = 0,
	COUNT
};