#pragma once
#include <map>
#include <string>

class HCDevice
{
public:
	HCDevice();
	virtual ~HCDevice() = default;

public:
	virtual void		Init() = 0;
	virtual void		Update() = 0;
	static size_t		GetDeviceIndex(const std::string& deviceName);

protected:
	virtual std::string GetDeviceName() const = 0;

private:
	static std::map<std::string, size_t> m_DeviceIndices;
};