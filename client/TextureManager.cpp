#include "TextureManager.h"
#include <WICTextureLoader.h>
#include "ErrorLogger.h"
ID3D11ShaderResourceView** TextureManager::SetTexture(ID3D11Device* device, std::wstring filePath)
{
	auto it = textureMap.find(filePath);
	ID3D11ShaderResourceView** ret;
	if (it == textureMap.end())//데이터가 없으면
	{
		ret = textureMap[filePath].GetAddressOf();
		//Load Texture
		HRESULT hr = DirectX::CreateWICTextureFromFile(device, filePath.c_str(), nullptr, ret);
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to create wic texture from file.");
			return nullptr;
		}
	}
	else
	{
		ret = it->second.GetAddressOf();
	}
    return ret;
}