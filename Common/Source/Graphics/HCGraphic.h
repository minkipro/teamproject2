#pragma once
#include "HCDevice.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <dxgiformat.h>
#include <functional>
#include "GameObject.h"

struct HCInputLayoutElement
{
	HCInputLayoutElement(const char* semanticName, UINT semanticIndex, DXGI_FORMAT format, UINT alignedByteOffset)
	{
		SemanticName = semanticName;
		SemanticIndex = semanticIndex;
		Format = format;
		AlignedByteOffset = alignedByteOffset;
	}

	std::string SemanticName;
	UINT		SemanticIndex;
	DXGI_FORMAT Format;
	UINT		AlignedByteOffset;
};

namespace HC
{
	enum class SHADERTYPE
	{
		VS,
		GS,
		HS,
		DS,
		PS,
		COUNT
	};

	enum class TEXTURETYPE
	{
		D2TEXTURE,
		DEPTH_STENCIL
	};

	enum class PRIMITIVE_TOPOLOGY
	{
		POINT,
		LINELIST,
		LINESTRIP,
		TRIANGLELIST,
		TRIANGLESTRIP
	};

	struct InputDataSample
	{
		virtual std::vector<HCInputLayoutElement>	GetInputData() const = 0;
		virtual const char* GetInputName() const = 0;
		virtual unsigned int						GetDataSize() const = 0;
	};

	struct MainPass
	{
		DirectX::XMFLOAT4X4 gView;
		DirectX::XMFLOAT4X4 gProj;
		DirectX::XMFLOAT4X4 gViewProj;
		DirectX::XMFLOAT4X4 gOrthoMatrix;
		DirectX::XMFLOAT2	gMousePos;
		DirectX::XMUINT2	gRenderTargetSize;
	};
}

struct RenderPoint :public HC::InputDataSample
{
	virtual std::vector<HCInputLayoutElement>	GetInputData() const override
	{
		return { {"VTABLE",0,DXGI_FORMAT_R32G32_UINT, 0},
				 {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT, 8},
				 {"RENDERSIZE",0,DXGI_FORMAT_R32G32_FLOAT, 20},
				 {"TEXCOORD",0,DXGI_FORMAT_R32G32B32A32_FLOAT, 28},
				 {"TEXINDEX",0,DXGI_FORMAT_R32_SINT, 44} };
	}

	virtual const char* GetInputName() const override { return typeid(RenderPoint).name(); }
	virtual unsigned int						GetDataSize() const override { return sizeof(RenderPoint); }

	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 Size;
	DirectX::XMFLOAT4 Color;

	int TextureIndex = -1;
};

class IHCInputLayout
{
public:
	IHCInputLayout() {}
	virtual ~IHCInputLayout() = default;

	virtual void* GetInputLayoutData() = 0;

private:

};

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

class IHCTexture
{
public:
	IHCTexture() {}

	virtual ~IHCTexture() = default;

	virtual void* GetTextureData() = 0;
	virtual POINT	GetTextureSize() = 0;
protected:

};

class IHCCBuffer
{
public:
	IHCCBuffer() {}
	virtual ~IHCCBuffer() = default;

	virtual void		CopyData(const void* data) = 0;
	virtual void* GetBuffer() = 0;

protected:

};

class HCTextureBuffer
{
public:
	HCTextureBuffer() {}
	virtual ~HCTextureBuffer() = default;

	virtual void	SetTexture(size_t slot, IHCTexture* texture) = 0;
	IHCTexture* GetTexture(size_t slot) { return m_TextureSlots[slot]; }

protected:
	std::vector<IHCTexture*> m_TextureSlots;
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

class IHCFont
{
public:
	struct TextData
	{
		std::wstring Text;
		DirectX::XMFLOAT2 Position;
		DirectX::XMFLOAT3 Color;
		DirectX::XMFLOAT2 Scale;
	};
	IHCFont() {}
	virtual ~IHCFont() = default;
	virtual void Init(void* device, void* dc) = 0;
	virtual void SetText(const IHCFont::TextData& textData) = 0;
	virtual void Render() = 0;
protected:
	
};

class HCGraphicPipeLine
{
public:
	HCGraphicPipeLine() = delete;
	HCGraphicPipeLine(const HCGraphicPipeLine& rhs) = delete;
	HCGraphicPipeLine(const std::string& name, void* vertexBuffer, std::function<void(void*)> vertexBufferReleaseFunc)
		: m_PipeLineName(name)
		, m_VertexBuffer(vertexBuffer)
		, m_VertexBufferRelease(vertexBufferReleaseFunc)
	{
	}
	virtual ~HCGraphicPipeLine()
	{
		m_VertexBufferRelease(m_VertexBuffer);
	}
	const HC::InputDataSample* GetCurrInputSample() const { return m_InputSample.get(); }
	const std::string& GetPipeLineName() const { return m_PipeLineName; }
	const auto& GetReservedObjects() const { return m_RenderReservedObjectsByTexture; }
	void* GetVertexBuffer() const { return m_VertexBuffer; }

	void						SetShader(HC::SHADERTYPE type, IHCShader* shader) { m_shaders[static_cast<unsigned int>(type)] = shader; }
	void						RenderReserveObject(const std::string& textureBufferName, const HC::InputDataSample* object) { m_RenderReservedObjectsByTexture[textureBufferName].push_back(object); }
	void						ClearReservedObjects() { m_RenderReservedObjectsByTexture.clear(); }

	template<typename T> void	SelectInputSample() { m_InputSample = std::make_unique<T>(); }

public:
	IHCShader* m_shaders[static_cast<unsigned int>(HC::SHADERTYPE::COUNT)] = {};
	std::vector<IHCCBuffer*>													m_CBuffers;

	HC::PRIMITIVE_TOPOLOGY														m_Primitive = HC::PRIMITIVE_TOPOLOGY::POINT;

	IHCRasterizer* m_Rasterizer = nullptr;
	IHCDepthStencilState* m_DepthStencilState = nullptr;
	IHCBlendState* m_BlendState = nullptr;

private:
	std::string																	m_PipeLineName;
	void*																		m_VertexBuffer;
	std::function<void(void*)>													m_VertexBufferRelease;
	std::unique_ptr<HC::InputDataSample>										m_InputSample = nullptr;
	std::unordered_map<std::string, std::vector<const HC::InputDataSample*>>	m_RenderReservedObjectsByTexture;
};

class HCGraphic : public IHCDevice
{
public: //pure virtual method
	HCGraphic(HWND windowHandle)
		: m_windowHandle(windowHandle)
	{

	}
	virtual ~HCGraphic() = default;

	virtual void		Init() = 0;
	virtual void		Update() = 0;

	virtual void		CreateGraphicPipeLine(const std::string& pipeLineName, HCGraphicPipeLine** out) = 0;
	virtual void		CreateTextureBuffer(const std::string& bufferName, HCTextureBuffer** out) = 0;
	virtual void		CreateTexture(const std::wstring& filePath, IHCTexture** out) = 0;
	virtual void		CreateShaderResource(const std::string& resourceName, size_t stride, const POINT& size, IHCTexture** out) = 0;
	virtual void		CreateCB(const std::string& bufferName, size_t stride, size_t num, IHCCBuffer** out) = 0;
	virtual void		CreateShader(const std::string& shaderName, HC::SHADERTYPE type, const std::wstring& filePath, const std::string& entryPoint, IHCShader** out) = 0;

	virtual void		GetGraphicPipeLine(const std::string& pipeLineName, HCGraphicPipeLine** out) = 0;
	virtual void		GetTextureBuffer(const std::string& bufferName, HCTextureBuffer** out) = 0;
	virtual void		GetTexture(const std::string& textureName, IHCTexture** out) = 0;
	virtual void		GetShaderResource(const std::string& resourceName, IHCTexture** out) = 0;
	virtual void		GetCB(const std::string& bufferName, IHCCBuffer** out) = 0;
	virtual void		GetShader(const std::string& shaderName, IHCShader** out) = 0;

public: //Optional virtual function
	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return LRESULT(0); }
	virtual void		NumberingGraphicPipeLineSlot(size_t slot, HCGraphicPipeLine* pipeLine);
	virtual void		NumberingGraphicPipeLineSlot(size_t slot, const std::string& pipelineName);

public:
	void				ReserveRender(const std::string& pipeLineName, const std::string& textureBufferName, const HC::InputDataSample* object);
	void				ReserveRender(size_t pipeLineSlot, const std::string& textureBufferName, const HC::InputDataSample* object);
	void				Render();

protected: //pure virtual method
	virtual void		RenderBegin() = 0;
	virtual void		RenderEnd() = 0;
	virtual void		SetPipeLineObject(const HCGraphicPipeLine* pipeLine) = 0;
	virtual void		RenderObjects(const std::string& textureBufferName, const std::vector<const HC::InputDataSample*> objects, size_t offset) = 0;

private:
	virtual std::string GetDeviceName() const override { return typeid(HCGraphic).name(); }

protected:
	HWND																m_windowHandle;
	std::unordered_map<std::string, std::unique_ptr<HCGraphicPipeLine>>	m_PipeLines;
	std::vector<HCGraphicPipeLine*>										m_PipeLineSlots;
};

