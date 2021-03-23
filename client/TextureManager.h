#pragma once
#include <string>
#include <map>
#include <wrl/client.h>
#include <d3d11.h>
class ID3D11ShaderResourceView;
class TextureManager
{
private:
	std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureMap;
public:
	
	ID3D11ShaderResourceView** SetTexture(ID3D11Device* device, std::wstring filePath);
};

