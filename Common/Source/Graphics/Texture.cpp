#include "Texture.h"
#include "COMException.h"
#include <WICTextureLoader.h>
ID3D11Device* Texture::_device = nullptr;
ID3D11DeviceContext* Texture::_dc = nullptr;
std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> Texture::textures;
bool Texture::Initialize(std::wstring filePath)					
{
	//Load Texture
	auto currentTexture = textures.find(filePath);
	if (currentTexture == textures.end())
	{
		textures[filePath];

		COM_HRESULT_IF_FAILED(DirectX::CreateWICTextureFromFile(
			_device, filePath.c_str(), nullptr, textures[filePath].GetAddressOf()),
			"Failed to create wic texture from file.");
		
		texture = textures[filePath].Get();
		/*D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		
		
		->GetDesc(&desc);
		desc.ViewDimension;*/
		int a = 1;
	}
	else
	{
		texture = currentTexture->second.Get();
	}
	return true;
}


void Texture::SetDevice(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	_device = device;
	_dc = dc;
}

void Texture::Draw()
{
	_dc->PSSetShaderResources(0, 1, &texture);
}
