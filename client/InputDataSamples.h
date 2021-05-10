#pragma once
#include <vector>

struct HCInputLayoutElement
{
	HCInputLayoutElement(const char* semanticName, UINT semanticIndex, DXGI_FORMAT format)
	{
		SemanticName = semanticName;
		SemanticIndex = semanticIndex;
		Format = format;
	}

	std::string SemanticName;
	UINT		SemanticIndex;
	DXGI_FORMAT Format;
};

namespace HC
{
	struct InputDataSample
	{
		virtual std::vector<HCInputLayoutElement>	GetInputData() const = 0;
		virtual const char* GetInputName() const = 0;
		virtual unsigned int						GetDataSize() const = 0;
		virtual int									GetTextureIndex() const { return -1; }
		const void* GetData() const
		{
			return reinterpret_cast<const BYTE*>(this) + sizeof(void*);
		}
	};
}

struct RenderPoint :public HC::InputDataSample
{
	virtual std::vector<HCInputLayoutElement>	GetInputData() const override
	{
		return { {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT},
				 {"RENDERSIZE",0,DXGI_FORMAT_R32G32_FLOAT},
				 {"TEXCOORD",0,DXGI_FORMAT_R32G32B32A32_FLOAT},
				 {"TEXINDEX",0,DXGI_FORMAT_R32_SINT} };
	}

	virtual const char* GetInputName() const override { return typeid(RenderPoint).name(); }
	virtual unsigned int						GetDataSize() const override { return sizeof(RenderPoint) - sizeof(void*); }
	virtual int									GetTextureIndex() const override { return TextureIndex; }

	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 Size;
	DirectX::XMFLOAT4 Color;

	int TextureIndex = -1;
};

struct RenderPointUV :public HC::InputDataSample
{
	virtual std::vector<HCInputLayoutElement>	GetInputData() const override
	{
		return { {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT},
				 {"RENDERSIZE",0,DXGI_FORMAT_R32G32_FLOAT},
				 {"TEXCOORD",0,DXGI_FORMAT_R32G32B32A32_FLOAT},
				 {"UV",0,DXGI_FORMAT_R32G32B32A32_FLOAT},
				 {"TEXINDEX",0,DXGI_FORMAT_R32_SINT} };
	}

	virtual const char* GetInputName() const override { return typeid(RenderPointUV).name(); }
	virtual unsigned int						GetDataSize() const override { return sizeof(RenderPointUV) - sizeof(void*); }
	virtual int									GetTextureIndex() const override { return TextureIndex; }

	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 Size;
	DirectX::XMFLOAT4 Color;
	DirectX::XMFLOAT4 Uv; // startX, startY, endX, endY

	int TextureIndex = -1;
};