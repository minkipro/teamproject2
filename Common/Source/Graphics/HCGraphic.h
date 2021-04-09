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
	HCInputLayoutElement(LPCSTR semanticName, UINT semanticIndex, DXGI_FORMAT format, UINT alignedByteOffset)
	{
		SemanticName = semanticName;
		SemanticIndex = semanticIndex;
		Format = format;
		AlignedByteOffset = alignedByteOffset;
	}

	LPCSTR SemanticName;
	UINT SemanticIndex;
	DXGI_FORMAT Format;
	UINT AlignedByteOffset;
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
		virtual const char*							GetInputName() const = 0;
		virtual unsigned int						GetDataSize() const = 0;
	};

	struct PointVertex :public InputDataSample
	{
		virtual std::vector<HCInputLayoutElement>	GetInputData() const override
		{

		}

		virtual const char*							GetInputName() const override {	return typeid(PointVertex).name(); }
		virtual unsigned int						GetDataSize() const override { return sizeof(PointVertex); }

		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 Size;
		union
		{
			DirectX::XMFLOAT4 Color;
			DirectX::XMFLOAT2 UV;
		};

		int TextureIndex = -1;
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

	virtual void*	GetTextureData() = 0;
	virtual POINT	GetTextureSize() = 0;
protected:

};

class IHCCBuffer
{
public:
	IHCCBuffer() {}
	virtual ~IHCCBuffer() = default;

	virtual void		CopyData(const void* data) = 0;
	virtual void*		GetBuffer() = 0;

protected:

};

class IHCTextureBuffer
{
public:
	IHCTextureBuffer() {}
	virtual ~IHCTextureBuffer() = default;

	virtual void	SetTexture(size_t slot, IHCTexture* texture) = 0;
	IHCTexture*		GetTexture(size_t slot) { return m_TextureSlots[slot]; }

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

	const std::string&	GetPipeLineName() const { return m_PipeLineName; }
	const auto&			GetReservedObjects() { return m_RenderReservedObjectsByTexture; }
	void*				GetVertexBuffer() const { return m_VertexBuffer; }

	void				RenderReserveObject(const std::string& textureBufferName, const HC::InputDataSample* object) { m_RenderReservedObjectsByTexture[textureBufferName].push_back(object); }
	void				ClearReservedObjects() { m_RenderReservedObjectsByTexture.clear(); }

public:
	IHCShader*																	m_Shaders[static_cast<unsigned int>(HC::SHADERTYPE::COUNT)] = {};
	std::vector<IHCCBuffer*>													m_CBuffers;
	
	HC::PRIMITIVE_TOPOLOGY														m_Primitive = HC::PRIMITIVE_TOPOLOGY::POINT;
	std::unique_ptr<HC::InputDataSample>										m_InputSample = nullptr;
	
	IHCRasterizer*																m_Rasterizer = nullptr;
	IHCDepthStencilState*														m_DepthStencilState = nullptr;							
	IHCBlendState*																m_BlendState = nullptr;

private:
	std::string																	m_PipeLineName;
	void*																		m_VertexBuffer;
	std::function<void(void*)>													m_VertexBufferRelease;
	std::unordered_map<std::string, std::vector<const HC::InputDataSample*>>	m_RenderReservedObjectsByTexture;
};

class HCGraphic : public IHCDevice
{
public: //pure virtual method
	HCGraphic(HWND windowHandle)
		: m_WindowHandle(windowHandle)
	{

	}
	virtual ~HCGraphic() = default;

	virtual void		Init() = 0;
	virtual void		Update() = 0;

	virtual void		CreateGraphicPipeLine(const std::string& pipeLineName, HCGraphicPipeLine** out) = 0;
	virtual void		CreateTextureBuffer(const std::string& bufferName, IHCTextureBuffer** out) = 0;
	virtual void		CreateTexture(const std::string& textureName, const std::wstring& filePath, IHCTexture** out) = 0;
	virtual void		CreateShaderResource(const std::string& resourceName, size_t stride, const POINT& size, IHCTexture** out) = 0;
	virtual void		CreateCB(const std::string& bufferName, size_t stride, size_t num, IHCCBuffer** out) = 0;
	virtual void		CreateShader(const std::string& shaderName, HC::SHADERTYPE type, const std::wstring& filePath, const std::string& entryPoint, IHCShader** out) = 0;

	virtual void		GetGraphicPipeLine(const std::string& pipeLineName, HCGraphicPipeLine** out) = 0;
	virtual void		GetTextureBuffer(const std::string& bufferName, IHCTextureBuffer** out) = 0;
	virtual void		GetTexture(const std::string& textureName, IHCTexture** out) = 0;
	virtual void		GetShaderResource(const std::string& resourceName, IHCTexture** out) = 0;
	virtual void		GetCB(const std::string& bufferName, IHCCBuffer** out) = 0;
	virtual void		GetShader(const std::string& shaderName, IHCShader** out) = 0;

public: //Optional virtual function
	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return LRESULT(0); }
	virtual void		NumberingGraphicPipeLineSlot(size_t slot, HCGraphicPipeLine* pipeLine);
	virtual void		NumberingGraphicPipeLineSlot(size_t slot, const std::string& pipelineName);

public:
	void				ReserveRender(const std::string& pipeLineName, const std::string& textureBufferName, const GameObject* object);
	void				ReserveRender(size_t pipeLineSlot, const std::string& textureBufferName, const GameObject* object);
	void				Render();

protected: //pure virtual method
	virtual void		RenderBegin() = 0;
	virtual void		RenderEnd() = 0;
	virtual void		SetPipeLineObject(const HCGraphicPipeLine* pipeLine) = 0;
	virtual void		RenderObjects(const std::string& textureBufferName, const std::vector<const HC::InputDataSample*> objects) = 0;

private:
	virtual std::string GetDeviceName() const override { return typeid(HCGraphic).name(); }

protected:
	HWND																m_WindowHandle;
	std::unordered_map<std::string, std::unique_ptr<HCGraphicPipeLine>>	m_PipeLines;

	std::vector<HCGraphicPipeLine*>										m_PipeLineSlots;
};

