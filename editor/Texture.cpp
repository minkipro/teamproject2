#include "Texture.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
std::unordered_map<std::string, Texture::TextureData> Texture::_textureMap;
Texture::Texture(ID3D11Device* device, const Color& color, aiTextureType type)
{
	Initialize1x1ColorTexture(device, color, type);
}

Texture::Texture(ID3D11Device* device, const Color* colorData, UINT width, UINT height, aiTextureType type)
{
	InitializeColorTexture(device, colorData, width, height, type);
}

Texture::Texture(ID3D11Device* device, const std::string& filePath, aiTextureType type)
{
	_filePath = filePath;
	auto it = _textureMap.find(filePath);
	if (it != _textureMap.end())
	{
		return;
	}
	_type = type;
	TextureData data;
	if (StringHelper::GetFileExtension(filePath) == ".dds")
	{
		HRESULT hr = DirectX::CreateDDSTextureFromFile(device, StringHelper::StringToWide(filePath).c_str(), data.texture.GetAddressOf(), data.textureView.GetAddressOf());
		if (FAILED(hr))
		{
			assert(0);
			//this->Initialize1x1ColorTexture(device, Colors::UnloadedTextureColor, type);
		}
	}
	else
	{
		HRESULT hr = DirectX::CreateWICTextureFromFile(device, StringHelper::StringToWide(filePath).c_str(), data.texture.GetAddressOf(), data.textureView.GetAddressOf());
		if (FAILED(hr))
		{
			assert(0);
			//this->Initialize1x1ColorTexture(device, Colors::UnloadedTextureColor, type);
		}
	}
	_textureMap[filePath] = data;
}

Texture::Texture(ID3D11Device* device, const uint8_t* pData, size_t size, aiTextureType type)
{
	_type = type;
	HRESULT hr = DirectX::CreateWICTextureFromMemory(device, pData, size, _texture.GetAddressOf(), _textureView.GetAddressOf());
	COM_ERROR_IF_FAILED(hr, "Failed to create Texture form memory");
}

aiTextureType Texture::GetType()
{
	return _type;
}

ID3D11ShaderResourceView* Texture::GetTextureResourceView()
{
	if (_filePath.empty())
	{
		return _textureView.Get();
	}
	return _textureMap[_filePath].textureView.Get();
}

ID3D11ShaderResourceView** Texture::GetTextureResourceViewAddress()
{
	if (_filePath.empty())
	{
		return _textureView.GetAddressOf();
	}
	return _textureMap[_filePath].textureView.GetAddressOf();
}

void Texture::Initialize1x1ColorTexture(ID3D11Device* device, const Color& colorData, aiTextureType type)
{
	InitializeColorTexture(device, &colorData, 1, 1, type);
}

void Texture::InitializeColorTexture(ID3D11Device* device, const Color* colorData, UINT width, UINT height, aiTextureType type)
{
	_type = type;
	CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
	ID3D11Texture2D* p2DTexture = nullptr;
	D3D11_SUBRESOURCE_DATA initialData{};
	initialData.pSysMem = colorData;
	initialData.SysMemPitch = width * sizeof(Color);
	HRESULT hr = device->CreateTexture2D(&textureDesc, &initialData, &p2DTexture);
	COM_ERROR_IF_FAILED(hr, "Failed to initialize texture from color data.");
	_texture = static_cast<ID3D11Texture2D*>(p2DTexture);
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);
	hr = device->CreateShaderResourceView(_texture.Get(), &srvDesc, _textureView.GetAddressOf());
	COM_ERROR_IF_FAILED(hr, "Failed to create shader resource view from texture generated from color data.");
}