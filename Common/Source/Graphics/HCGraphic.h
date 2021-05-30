#pragma once
#include "HCDevice.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <dxgiformat.h>
#include "GameObject.h"
#include "HCInputDataSamples.h"

namespace HC
{
	enum SHADER_TYPE
	{
		HCSHADER_VS,
		HCSHADER_GS,
		HCSHADER_HS,
		HCSHADER_DS,
		HCSHADER_PS,
		HCSHADER_COUNT
	};

	enum class GRAPHIC_RESOURCE_TYPE
	{
		GRAPHIC_RESOURCE_BUFFER,
		GRAPHIC_RESOURCE_CONSTANT_BUFFER,
		GRAPHIC_RESOURCE_TEXTURE1D,
		GRAPHIC_RESOURCE_TEXTURE2D,
		GRAPHIC_RESOURCE_TEXTURE3D,
	};

	enum class GRAPHIC_RESOURCE_USAGE_TYPE
	{
		GRAPHIC_RESOURCE_USAGE_DEFAULT,
		GRAPHIC_RESOURCE_USAGE_IMMUTABLE,
		GRAPHIC_RESOURCE_USAGE_DYNAMIC,
		GRAPHIC_RESOURCE_USAGE_STAGING,
	};

	enum GRAPHIC_RESOURCE_BIND_FLAGS
	{
		GRAPHIC_RESOURCE_BIND_NONE = 0,
		GRAPHIC_RESOURCE_BIND_VERTEX_BUFFER = 1,
		GRAPHIC_RESOURCE_BIND_INDEX_BUFFER = 1 << 1,
		GRAPHIC_RESOURCE_BIND_CONSTANT_BUFFER = 1 << 2,
		GRAPHIC_RESOURCE_BIND_SHADERRESOURCE_VS = 1 << 3,
		GRAPHIC_RESOURCE_BIND_SHADERRESOURCE_GS = 1 << 4,
		GRAPHIC_RESOURCE_BIND_SHADERRESOURCE_HS = 1 << 5,
		GRAPHIC_RESOURCE_BIND_SHADERRESOURCE_DS = 1 << 6,
		GRAPHIC_RESOURCE_BIND_SHADERRESOURCE_PS = 1 << 7,
		GRAPHIC_RESOURCE_BIND_STREAM_OUTPUT = 1 << 8,
		GRAPHIC_RESOURCE_BIND_RENDER_TARGET = 1 << 9,
		GRAPHIC_RESOURCE_BIND_DEPTH_STENCIL = 1 << 10,
		GRAPHIC_RESOURCE_BIND_UNORDERED_ACCESS = 1 << 11,

		GRAPHIC_RESOURCE_BIND_SHADERRESOURCE_ALL = GRAPHIC_RESOURCE_BIND_SHADERRESOURCE_VS | GRAPHIC_RESOURCE_BIND_SHADERRESOURCE_GS | GRAPHIC_RESOURCE_BIND_SHADERRESOURCE_HS | GRAPHIC_RESOURCE_BIND_SHADERRESOURCE_DS | GRAPHIC_RESOURCE_BIND_SHADERRESOURCE_PS,
	};

	enum GRAPHIC_RESOURCE_FLAGS
	{
		GRAPHIC_RESOURCE_FLAG_NONE = 0,
		GRAPHIC_RESOURCE_FLAG_STRUCTURED_BUFFER = 1,
	};

	struct GRAPHIC_RESOURCE_BUFFER
	{
		UINT					StrideNum = 0;
	};

	struct GRAPHIC_RESOURCE_TEXTURE
	{
		DirectX::XMUINT3		Size = { 0,0,0 };
		UINT					ArrayNum = 0;
		DXGI_FORMAT				Format;
	};

	struct GRAPHIC_RESOURCE_DESC
	{
		GRAPHIC_RESOURCE_DESC()
		{
			ZeroMemory(this, sizeof(GRAPHIC_RESOURCE_DESC));
		}
		
		GRAPHIC_RESOURCE_TYPE		Type;
		GRAPHIC_RESOURCE_FLAGS		Flags;
		GRAPHIC_RESOURCE_USAGE_TYPE	UsageType;
		GRAPHIC_RESOURCE_BIND_FLAGS BindFlags;
		UINT						Stride = 0;

		const void*					DefaultData = nullptr;

		union
		{
			GRAPHIC_RESOURCE_TEXTURE Texture;
			GRAPHIC_RESOURCE_BUFFER	 Buffer;
		};
	};

	enum class PRIMITIVE_TOPOLOGY
	{
		UNDEFINED,
		POINT,
		LINELIST,
		LINESTRIP,
		TRIANGLELIST,
		TRIANGLESTRIP
	};

	struct MainPass
	{
		DirectX::XMFLOAT4X4 ViewMatrix;
		DirectX::XMFLOAT4X4 ProjMatrix;
		DirectX::XMFLOAT4X4 ViewProjMatrix;
		DirectX::XMFLOAT4X4 OrthoMatrix;
		DirectX::XMFLOAT2	MousePos;
		DirectX::XMUINT2	RenderTargetSize;
	};
}

class IHCShader
{
public:
	IHCShader()
	{
	}

	virtual ~IHCShader() = default;

	virtual void* GetShaderData() = 0;

protected:
};

class IHCResource
{
public:
	IHCResource()
	{
	}
	virtual ~IHCResource() = default;

	const HC::GRAPHIC_RESOURCE_DESC&	GetDesc() { return m_desc; }
	virtual void*						GetResource() = 0;
	virtual void*						GetResourceView() = 0;

	virtual void						Map() = 0;
	virtual void						UnMap() = 0;
	virtual void						CpuDataCopyToGpu(void* data) = 0;
	virtual void						CpuDataCopyToGpu(void* data, size_t byteSize, size_t byteOffset) = 0;
	virtual void						CpuDataCopyToGpu(void* data, size_t offsetSrtide) = 0;
	virtual void						GpuDataCopyToCpu(const RECT& rect, std::vector<std::vector<BYTE>>& out) = 0;

protected:
	HC::GRAPHIC_RESOURCE_DESC m_desc;
};

class IHCRasterizer
{
public:
	IHCRasterizer() {}
	virtual ~IHCRasterizer() = default;

protected:
};

class IHCBlendState
{
public:
	IHCBlendState() {}
	virtual ~IHCBlendState() = default;

protected:
};

class IHCDepthStencilState
{
public:
	IHCDepthStencilState() {}
	virtual ~IHCDepthStencilState() = default;

protected:
};

class IHCTextData
{
public:
	IHCTextData() = default;
	virtual ~IHCTextData() = default;

	virtual void SetFont(const std::wstring& fontName) = 0;
	virtual void GetFontNames(std::vector<std::wstring>& out) = 0;

public:
	std::wstring		m_text;
	DirectX::XMFLOAT3	m_position = { 0,0,0 };
	DirectX::XMFLOAT4	m_color = { 0,0,0,1 };
	DirectX::XMFLOAT2	m_scale = { 1,1 };
};

struct HCTextureData
{
	int textureIndex = -1;
	int spriteNum = -1;
};

struct HCMesh
{
	HC::PRIMITIVE_TOPOLOGY			Primitive = HC::PRIMITIVE_TOPOLOGY::POINT;
	UINT							VertexCount = 0;
	UINT							IndexCount = 0;
	UINT							VertexOffset = 0;
	UINT							IndexOffset = 0;
	std::shared_ptr<IHCResource>	VertexBuffer;
	std::shared_ptr<IHCResource>	IndexBuffer;
};

class HCGraphicPipeLine
{
public:
	HCGraphicPipeLine() = default;
	virtual ~HCGraphicPipeLine() = default;

	void										SetVertexType(size_t typeHashCode, size_t size, const std::vector<HCInputLayoutElement>* inputLayout);
	const std::vector<HCInputLayoutElement>*	GetVertexInputLayout() const { return m_vertexLayout; }
	size_t										GetVertexTypeHash() const { return m_vertexTypeHash; }
	UINT										GetVertexSize() const { return m_vertexSize; }

public:
	std::shared_ptr<IHCShader>					m_shaders[HC::SHADER_TYPE::HCSHADER_COUNT] = {};

	std::shared_ptr<IHCRasterizer>				m_rasterizer;
	std::shared_ptr<IHCDepthStencilState>		m_depthStencilState;
	std::shared_ptr<IHCBlendState>				m_blendState;

	std::vector<std::shared_ptr<IHCResource>>	m_renderTargets;
	std::shared_ptr<IHCResource>				m_depthStencil;																																																																																																						
	HC::PRIMITIVE_TOPOLOGY						m_primitive = HC::PRIMITIVE_TOPOLOGY::POINT;

private:
	const std::vector<HCInputLayoutElement>*	m_vertexLayout;
	size_t										m_vertexTypeHash;
	UINT										m_vertexSize;
};

class HCGraphic : public IHCDevice
{
public: //pure virtual method
	HCGraphic(HWND windowHandle)
		: m_windowHandle(windowHandle)
	{

	}
	virtual ~HCGraphic() = default;

	virtual void			Init() = 0;
	virtual void			Update() = 0;

	virtual void			CreateResource(const HC::GRAPHIC_RESOURCE_DESC& desc, std::shared_ptr<IHCResource>& out) = 0;
	virtual void			CreateShader(HC::SHADER_TYPE type, const std::wstring& filePath, const std::string& entryPoint, std::shared_ptr<IHCShader>& out) = 0;
	virtual void			CreateTextData(std::shared_ptr<IHCTextData>& out) = 0;

	virtual void			CopyResource(std::shared_ptr<IHCResource> dest, std::shared_ptr<IHCResource> src) = 0;

	virtual void			RenderBegin() = 0;
	virtual void			RenderEnd() = 0;

	virtual void			SetPipeLineObject(const HCGraphicPipeLine* pipeLine) = 0;
	virtual void			SetTexture(UINT textureIndex, UINT shaderResourceSlot) = 0;
	virtual void			SetShaderResource(std::shared_ptr<IHCResource> resource, UINT shaderResourceSlot) = 0;
	virtual void			SetShaderResources(const std::vector<std::shared_ptr<IHCResource>>& resources, UINT shaderResourceStartSlot) = 0;
	virtual void			SetConstantBuffer(std::shared_ptr<IHCResource> buffer, UINT constantBufferSlot) = 0;
	virtual void			SetConstantBuffers(const std::vector<std::shared_ptr<IHCResource>>& buffers, UINT constantBufferStartSlot) = 0;

	virtual void			DrawInsatance(const HCMesh* mesh, UINT numInstance, UINT InstanceOffset = 0) = 0;
	virtual void			DrawIndexedInsatance(const HCMesh* mesh, UINT numInstance, UINT InstanceOffset = 0) = 0;
	virtual void			DrawIndexed(const HCMesh* mesh) = 0;
	virtual void			Draw(const HCMesh* mesh) = 0;
	virtual void			DrawFont() = 0;

	virtual HCTextureData	GetTextureIndex(const std::wstring& textureName) const = 0;

public: //Optional virtual function
	virtual LRESULT			WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return LRESULT(0); }

private:
	virtual std::string		GetDeviceName() const override { return typeid(HCGraphic).name(); }

protected:
	HWND m_windowHandle;
};
