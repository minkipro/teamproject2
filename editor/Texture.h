#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Color.h"
#include <assimp/material.h>
#include <unordered_map>
#include <string>

enum class TextureStorageType
{
	Invalid,
	None,
	EmbeddedIndexCompressed,
	EmbeddedIndexNonCompressed,
	EmbeddedCompressed,
	EmbeddedNonCompressed,
	Disk
};

class Texture
{
public:
	Texture(ID3D11Device* device, const Color& color, aiTextureType type);
	Texture(ID3D11Device* device, const Color* colorData, UINT width, UINT height, aiTextureType type); //Generate texture of specific color data
	Texture(ID3D11Device* device, const std::string& filePath, aiTextureType type);
	Texture(ID3D11Device* device, const uint8_t* pData, size_t size, aiTextureType type);

	aiTextureType GetType();
	ID3D11ShaderResourceView* GetTextureResourceView();
	ID3D11ShaderResourceView** GetTextureResourceViewAddress();

private:
	void Initialize1x1ColorTexture(ID3D11Device* device, const Color& colorData, aiTextureType type);
	void InitializeColorTexture(ID3D11Device* device, const Color* colorData, UINT width, UINT height, aiTextureType type);

private:
	Microsoft::WRL::ComPtr<ID3D11Resource>				_texture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	_textureView = nullptr;
	aiTextureType										_type = aiTextureType::aiTextureType_UNKNOWN;
	std::string _filePath;
	struct TextureData
	{
		Microsoft::WRL::ComPtr<ID3D11Resource> texture = nullptr;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView = nullptr;
	};
	static std::unordered_map<std::string, TextureData> _textureMap;
};