#include "HCDevice.h"
#include "COMException.h"

std::map<std::string, size_t> HCDevice::m_DeviceIndices;

HCDevice::HCDevice()
{
	auto iter = m_DeviceIndices.find(GetDeviceName());

	if (iter == m_DeviceIndices.end())
	{
		m_DeviceIndices.insert({ GetDeviceName(), m_DeviceIndices.size() });
	}
	else
	{
		COM_THROW_IF_FAILED(false, "Device per type is created only one");
	}
}

size_t HCDevice::GetDeviceIndex(const std::string& deviceName)
{
	return size_t();
}

size_t HCDevice::GetDeviceIndex(const std::string& deviceName)
{
	auto iter = m_DeviceIndices.find(deviceName);

	COM_THROW_IF_FAILED( iter!=m_DeviceIndices.end() , "Device per type is created only one");

	if (iter != m_DeviceIndices.end())
	{
		m_DeviceIndices.insert({ deviceName, m_DeviceIndices.size() });
	}
	else
	{
		COM_THROW_IF_FAILED(false, "Device per type is created only one");
	}
}
