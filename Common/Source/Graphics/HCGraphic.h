#pragma once
#include "HCDevice.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include "GameObject.h"

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
}

class IHCShader
{
public:
	IHCShader()
	{
	}
	
	virtual ~IHCShader() = default;

	virtual void*	GetShaderData() = 0;

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

	virtual void CopyData(size_t stride, size_t num, const void* data) = 0;

protected:

};

class IHCTextureBuffer
{
public:
	IHCTextureBuffer() {}
	virtual ~IHCTextureBuffer() = default;

	virtual void SetTexture(size_t slot, IHCTexture* texture) = 0;
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

class HCGraphicPipeLine
{
public:
	HCGraphicPipeLine(const std::string& name)
		:m_PipeLineName(name)
	{
	}
	virtual ~HCGraphicPipeLine() = default;


	const std::string&	GetPipeLineName() { return m_PipeLineName; }
	const auto&			GetReservedObjects() { return m_RenderReservedObjectsByTexture; }
	void				RenderReserveObject(const std::string& textureBufferName, const GameObject* object) { m_RenderReservedObjectsByTexture[textureBufferName].push_back(object); }
	void				ClearReservedObjects() { m_RenderReservedObjectsByTexture.clear(); }

public:
	IHCShader*														m_Shaders[static_cast<unsigned int>(HC::SHADERTYPE::COUNT)] = {};
	IHCRasterizer*													m_Rasterizer=nullptr;
	IHCBlendState*													m_BlendState=nullptr;

private:
	std::string														m_PipeLineName;
	std::unordered_map<std::string, std::vector<const GameObject*>>	m_RenderReservedObjectsByTexture;
};

class HCGraphic : public IHCDevice
{
public: //pure virtual method
	HCGraphic(HWND windowHandle)
		: m_WindowHandle(windowHandle)
		, m_BaseCB(nullptr)
	{

	}
	virtual ~HCGraphic() = default;

	virtual void		Init() = 0;
	virtual void		Update() = 0;

	virtual HRESULT		CreateGraphicPipeLine(const std::string& pipeLineName, HCGraphicPipeLine** out) = 0;
	virtual HRESULT		CreateTextureBuffer(const std::string& bufferName, IHCTextureBuffer** out) = 0;
	virtual HRESULT		CreateTexture(const std::string& textureName, const std::wstring& filePath, IHCTexture** out) = 0;
	virtual HRESULT		CreateShaderResource(const std::string& resourceName, size_t stride, const POINT& size, IHCTexture** out) = 0;
	virtual HRESULT		CreateCB(const std::string& bufferName, size_t stride, size_t num, IHCCBuffer** out) = 0;
	virtual HRESULT		CreateShader(const std::string& shaderName, HC::SHADERTYPE type, const std::wstring& filePath, const std::string& entryPoint, IHCShader** out) = 0;

	virtual void		GetGraphicPipeLine(const std::string& pipeLineName, HCGraphicPipeLine** out) = 0;
	virtual void		GetTextureBuffer(const std::string& bufferName, IHCTextureBuffer** out) = 0;
	virtual void		GetTexture(const std::string& textureName, IHCTexture** out) = 0;
	virtual void		GetShaderResource(const std::string& resourceName, IHCTexture** out) = 0;
	virtual void		GetCB(const std::string& bufferName, IHCCBuffer** out) = 0;
	virtual void		GetShader(const std::string& shaderName, IHCShader** out) = 0;

protected: //pure virtual method
	virtual void		RenderBegin() = 0;
	virtual void		RenderEnd() = 0;
	virtual void		ApplyBaseCB() = 0;
	virtual void		SetPipeLineObject(const HCGraphicPipeLine* pipeLine) = 0;
	virtual void		RenderObjects(const std::string& textureBufferName, const std::vector<const GameObject*> objects) = 0;

public: //Optional virtual function
	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return LRESULT(0); }
	virtual void		NumberingGraphicPipeLineSlot(size_t slot, HCGraphicPipeLine* pipeLine);
	virtual void		NumberingGraphicPipeLineSlot(size_t slot, const std::string& pipelineName);

public:
	void				SetBaseCB(const IHCCBuffer* baseCB) { m_BaseCB = baseCB; }
	void				ReserveRender(const std::string& pipeLineName, const std::string& textureBufferName, const GameObject* object);
	void				ReserveRender(size_t pipeLineSlot, const std::string& textureBufferName, const GameObject* object);
	void				Render();

private:
	virtual std::string GetDeviceName() const override { return typeid(HCGraphic).name(); }

protected:
	HWND																m_WindowHandle;
	std::unordered_map<std::string, std::unique_ptr<HCGraphicPipeLine>>	m_PipeLines;

	std::vector<HCGraphicPipeLine*>										m_PipeLineSlots;
	const IHCCBuffer*													m_BaseCB;
};

