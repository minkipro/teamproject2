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
	enum class SHADERTYPE
	{
		VS = 0,
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

	virtual void	CopyData() = 0;
	virtual void*	GetBuffer() = 0;
	virtual void	SetData(size_t stride, void* data) = 0;

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
	virtual size_t GetFontNum() = 0;
	virtual void GetFontNames(std::vector<std::wstring>& out) = 0;
	virtual void SetFont(unsigned int index) = 0;
	virtual void SetFont(std::wstring fileName) = 0;
	virtual void SetText(const IHCFont::TextData& textData) = 0;//삭제 필요
	virtual std::vector<IHCFont::TextData>* GetText() = 0;
	virtual void Render() = 0;
protected:

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
	{
	}
	virtual ~HCGraphicPipeLine()
	{
		m_vertexBufferRelease(m_vertexBuffer);
	}
	const HC::InputDataSample*	GetCurrInputSample() const { return m_inputSample.get(); }
	const std::string&			GetPipeLineName() const { return m_pipeLineName; }
	const auto&					GetReservedObjects() const { return m_renderReservedObjectsByTexture; }
	void*						GetVertexBuffer() const { return m_vertexBuffer; }

	void						SetShader(HC::SHADERTYPE type, IHCShader* shader) { m_shaders[static_cast<unsigned int>(type)] = shader; }
	void						RenderReserveObject(const HC::InputDataSample* object);
	void						ClearReservedObjects();

	template<typename T> void	SelectInputSample() { m_inputSample = std::make_unique<T>(); }

public:
	IHCShader*												m_shaders[static_cast<unsigned int>(HC::SHADERTYPE::COUNT)] = {};
	std::vector<IHCCBuffer*>								m_CBuffers;

	HC::PRIMITIVE_TOPOLOGY									m_primitive = HC::PRIMITIVE_TOPOLOGY::POINT;

	IHCRasterizer*											m_rasterizer = nullptr;
	IHCDepthStencilState*									m_depthStencilState = nullptr;
	IHCBlendState*											m_blendState = nullptr;

private:
	std::string												m_pipeLineName;
	void*													m_vertexBuffer;
	std::function<void(void*)>								m_vertexBufferRelease;
	std::unique_ptr<HC::InputDataSample>					m_inputSample = nullptr;
	std::vector<std::vector<const HC::InputDataSample*>>	m_renderReservedObjectsByTexture;
};
class HCFont;
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
	virtual void		CreateShaderResource(const std::string& resourceName, size_t stride, const POINT& size, IHCTexture** out) = 0;
	virtual void		CreateCB(const char* bufferName, size_t stride, size_t num, void* data, IHCCBuffer** out) = 0;
	virtual void		CreateShader(const std::string& shaderName, HC::SHADERTYPE type, const std::wstring& filePath, const std::string& entryPoint, IHCShader** out) = 0;

	virtual void		GetGraphicPipeLine(const std::string& pipeLineName, HCGraphicPipeLine** out) = 0;
	virtual void		GetShaderResource(const std::string& resourceName, IHCTexture** out) = 0;
	virtual void		GetCB(const std::string& bufferName, IHCCBuffer** out) = 0;
	virtual void		GetShader(const std::string& shaderName, IHCShader** out) = 0;

	virtual int			GetTextureIndex(const std::wstring& textureName) const = 0;

public: //Optional virtual function
	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return LRESULT(0); }
	virtual void		NumberingGraphicPipeLineSlot(size_t slot, HCGraphicPipeLine* pipeLine);
	virtual void		NumberingGraphicPipeLineSlot(size_t slot, const std::string& pipelineName);

public:
	void				ReserveRender(const std::string& pipeLineName, const HC::InputDataSample* object);
	void				ReserveRender(size_t pipeLineSlot, const HC::InputDataSample* object);
	void				Render();
	virtual void		RenderFont() {};
	virtual IHCFont*	GetFont() { return nullptr; };
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

