#pragma once
#include <map>
#include <string>
#include <wrl/client.h>
#include <d3d11.h>
class Texture
{
private:
	static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textures;
	static ID3D11Device* _device;
	static ID3D11DeviceContext* _dc;

	ID3D11ShaderResourceView* texture;
	
public:
	static void SetDevice(ID3D11Device* device, ID3D11DeviceContext* dc);
	virtual bool Initialize(std::wstring filePath);
	virtual void Draw();
};

