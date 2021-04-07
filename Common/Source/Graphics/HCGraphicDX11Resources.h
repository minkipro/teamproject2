#pragma once
#include <d3d11.h>
#include <wrl/client.h>

#include "HCGraphic.h"

using Microsoft::WRL::ComPtr;

class HCDX11Shader : public IHCShader
{
public:
	HCDX11Shader()
	{
	}

	virtual ~HCDX11Shader() = default;

	virtual void* GetShaderData() override;
};

//class HCDX11Texture : public IHCTexture, ID3D11Texture2D
//{
//public:
//	HCDX11Texture( ) {}
//
//	virtual ~HCDX11Texture() = default;
//
//	virtual void* GetTextureData();
//	virtual POINT GetTextureSize();
//};
//
//class IHCDX11ConstBuffer: public IHCCBuffer, ID3D11Buffer
//{
//public:
//	IHCDX11ConstBuffer() {}
//	virtual ~IHCDX11ConstBuffer() = default;
//
//	virtual void CopyData(size_t stride, size_t num, const void* data) override;
//
//protected:
//
//};

//class IHCTextureBuffer
//{
//public:
//	IHCTextureBuffer() {}
//	virtual ~IHCTextureBuffer() = default;
//
//	virtual void SetTexture(size_t slot, IHCTexture* texture) = 0;
//	IHCTexture* GetTexture(size_t slot) { return m_TextureSlots[slot]; }
//
//protected:
//	std::vector<IHCTexture*> m_TextureSlots;
//};