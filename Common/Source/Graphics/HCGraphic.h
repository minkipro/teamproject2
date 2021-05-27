#pragma once
#include "HCDevice.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <dxgiformat.h>
#include <functional>
#include "GameObject.h"
#include "HCInputDataSamples.h"

namespace HC
{
	enum class SHADER_TYPE
	{
		VS,
		GS,
		HS,
		DS,
		PS,
		COUNT
	};

	enum class GRAPHIC_RESOURCE_TYPE
	{
		GRAPHIC_RESOURCE_BUFFER,
		GRAPHIC_RESOURCE_UPLOAD_BUFFER,
		GRAPHIC_RESOURCE_TEXTURE1D,
		GRAPHIC_RESOURCE_TEXTURE2D,
		GRAPHIC_RESOURCE_TEXTURE3D,
	};

	enum GRAPHIC_RESOURCE_FLAGS
	{
		GRAPHIC_RESOURCE_FLAG_NONE = 0,
		GRAPHIC_RESOURCE_FLAG_RENDERTARGET = 1,
		GRAPHIC_RESOURCE_FLAG_SHADERRESOURCE_VS = 1 << 1,
		GRAPHIC_RESOURCE_FLAG_SHADERRESOURCE_HS = 1 << 2,
		GRAPHIC_RESOURCE_FLAG_SHADERRESOURCE_DS = 1 << 3,
		GRAPHIC_RESOURCE_FLAG_SHADERRESOURCE_PS = 1 << 4,
		GRAPHIC_RESOURCE_FLAG_ONLY_READBACK = 1 << 5,
	};

	struct GRAPHIC_RESOURCE_DESC
	{
		GRAPHIC_RESOURCE_TYPE	Type;
		GRAPHIC_RESOURCE_FLAGS	Flags;
		DXGI_FORMAT				Format;
		POINT					Size;
		UINT					Stride = 0;
	};

	enum class PRIMITIVE_TOPOLOGY
	{
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

struct RenderPoint
{
	static std::vector<HCInputLayoutElement> InputLayout;

	DirectX::XMFLOAT3	Position = { 0,0,0 };
	DirectX::XMFLOAT2	Size = { 0,0 };
	DirectX::XMFLOAT4	Color = { 0,0,0,1 };
	int					TextureIndex = -1;
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

class IHCResource
{
public:
	IHCResource() {}

	virtual ~IHCResource() = default;

	virtual void*	GetResourceData() = 0;
	virtual POINT	GetResourceSize() = 0;
protected:

};

class IHCCBuffer
{
public:
	IHCCBuffer() {}
	virtual ~IHCCBuffer() = default;

	virtual void	CopyData(const void* data) = 0;
	virtual void*	GetBuffer() = 0;

protected:

};

class IHCVertexBuffer
{
public:
	IHCVertexBuffer() {}
	virtual ~IHCVertexBuffer() = default;

	virtual void	CopyData(const void* data) = 0;
	virtual void*	GetBuffer() = 0;

protected:

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

	virtual void Release() = 0;
	virtual void SetFont(const std::wstring& fontName) = 0;
	virtual void GetFontNames(std::vector<std::wstring>& out) = 0;

public:
	std::wstring		m_text;
	DirectX::XMFLOAT3	m_position = { 0,0,0 };
	DirectX::XMFLOAT4	m_color = { 0,0,0,1 };
	DirectX::XMFLOAT2	m_scale = { 1,1 };
};

struct TextureData
{
	int textureIndex = -1;
	int spriteNum = -1;
};

class HCGraphicPipeLine
{
public:
	HCGraphicPipeLine() = delete;
	HCGraphicPipeLine(const HCGraphicPipeLine& rhs) = delete;
	HCGraphicPipeLine(const std::string& name, void* vertexBuffer, std::function<void(void*)> vertexBufferReleaseFunc)
		: m_pipeLineName(name)
		, m_vertexBuffer(vertexBuffer)
		, m_vertexBufferRelease(vertexBufferReleaseFunc)
		, m_InputLayout(nullptr)
		, m_InputHash(0)
		, m_InputDataSize(0)
	{
	}
	virtual ~HCGraphicPipeLine()
	{
		m_vertexBufferRelease(m_vertexBuffer);
	}

	const std::string&							GetPipeLineName() const { return m_pipeLineName; }
	const auto&									GetReservedObjectData() const { return m_renderReservedObjectsByTexture; }
	void*										GetVertexBuffer() const { return m_vertexBuffer; }

	const std::vector<HCInputLayoutElement>*	GetInputLayoutVector() const { return m_InputLayout; }
	size_t										GetInputLayoutHash() const { return m_InputHash; }
	UINT										GetInputDataSize() const { return m_InputDataSize; }

	void										SetShader(HC::SHADER_TYPE type, IHCShader* shader) { m_shaders[static_cast<unsigned int>(type)] = shader; }
	void										RenderReserveObject(const void* inputData, int textureIndex);
	void										ClearReservedObjects();

	template<typename T> void					SelectInputSample();
	void										SetVertexBuffer(void* vertexBuffer)
	{
		m_vertexBuffer = vertexBuffer;
	}
	

public:
	IHCShader*												m_shaders[static_cast<unsigned int>(HC::SHADER_TYPE::COUNT)] = {};
	std::vector<IHCCBuffer*>								m_CBuffers;

	HC::PRIMITIVE_TOPOLOGY									m_primitive = HC::PRIMITIVE_TOPOLOGY::POINT;

	IHCRasterizer*											m_rasterizer = nullptr;
	IHCDepthStencilState*									m_depthStencilState = nullptr;
	IHCBlendState*											m_blendState = nullptr;

private:
	std::string												m_pipeLineName;
	void*													m_vertexBuffer;
	std::function<void(void*)>								m_vertexBufferRelease;

	size_t													m_InputHash;
	const std::vector<HCInputLayoutElement>*				m_InputLayout;
	UINT													m_InputDataSize;

	std::vector<std::vector<BYTE>>							m_renderReservedObjectsByTexture;
};

template<typename T> 
inline void HCGraphicPipeLine::SelectInputSample()
{
	ClearReservedObjects();
	m_InputHash = typeid(T).hash_code();
	m_InputDataSize = sizeof(T);
	m_InputLayout = &T::InputLayout;
}

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
	virtual void		CreateResource(const std::string& resourceName, const HC::GRAPHIC_RESOURCE_DESC& desc, IHCResource** out) = 0;
	virtual void		CreateCB(const std::string& bufferName, size_t stride, size_t num, std::unique_ptr<IHCCBuffer>& out) = 0;
	virtual void		CreateShader(const std::string& shaderName, HC::SHADER_TYPE type, const std::wstring& filePath, const std::string& entryPoint, IHCShader** out) = 0;
	virtual void		CreateFontData(IHCTextData** out) = 0;

	virtual void		GetGraphicPipeLine(const std::string& pipeLineName, HCGraphicPipeLine** out) = 0;
	virtual void		GetShaderResource(const std::string& resourceName, IHCResource** out) = 0;
	virtual void		GetCB(const std::string& bufferName, IHCCBuffer** out) = 0;
	virtual void		GetShader(const std::string& shaderName, IHCShader** out) = 0;

	virtual TextureData	GetTextureIndex(const std::wstring& textureName) const = 0;

public: //Optional virtual function
	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return LRESULT(0); }
	virtual void		NumberingGraphicPipeLineSlot(size_t slot, HCGraphicPipeLine* pipeLine);
	virtual void		NumberingGraphicPipeLineSlot(size_t slot, const std::string& pipelineName);

public:
	void				ReserveRender(const std::string& pipeLineName, const void* object, int textureIndex);
	void				ReserveRender(size_t pipeLineSlot, const void* object, int textureIndex);
	void				Render();
	virtual void		RenderFont() {};

protected: //pure virtual method
	virtual void		RenderBegin() = 0;
	virtual void		RenderEnd() = 0;
	virtual void		SetPipeLineObject(const HCGraphicPipeLine* pipeLine) = 0;
	virtual void		RenderObjects(HCGraphicPipeLine* pipeLine) = 0;

private:
	virtual std::string GetDeviceName() const override { return typeid(HCGraphic).name(); }

protected:
	HWND																m_windowHandle;
	std::unordered_map<std::string, std::unique_ptr<HCGraphicPipeLine>>	m_PipeLines;
	std::vector<HCGraphicPipeLine*>										m_PipeLineSlots;
};

