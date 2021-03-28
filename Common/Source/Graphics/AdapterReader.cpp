#include "AdapterReader.h"
#include "COMException.h"

std::vector<AdapterData> AdapterReader::adapters;

std::vector<AdapterData> AdapterReader::GetAdapters()
{
	if (adapters.size() > 0) //If already initialized
		return adapters;

	Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;

	// Create a DXGIFactory object.

	COM_HRESULT_IF_FAILED(CreateDXGIFactory(IID_PPV_ARGS(pFactory.GetAddressOf())),
		"Failed to create DXGIFactory for enumerating adapters.");

	IDXGIAdapter* pAdapter;
	UINT index = 0;
	while (SUCCEEDED(pFactory->EnumAdapters(index, &pAdapter)))
	{
		adapters.push_back(AdapterData(pAdapter));
		index += 1;
	}
	return adapters;
}

AdapterData::AdapterData(IDXGIAdapter* pAdapter)
{
	this->pAdapter = pAdapter;

	COM_HRESULT_IF_FAILED(pAdapter->GetDesc(&this->description), "Failed to Get Description for IDXGIAdapter.");
}
