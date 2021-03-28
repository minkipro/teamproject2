#pragma once
#include "HCDevice.h"
#include <map>
#include <vector>
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
}


class IHCSahder
{
public:


private:

};

class IHCGraphicOut
{
public:

private:

};


class IHCTexture
{
public:

private:

};

class IHCTextureBuffer
{
public:

private:

};

class HCGraphicPipeLine
{
public:
	HCGraphicPipeLine(const std::string& name)
		:m_PipeLineName(name)
	{
		for (unsigned int i = 0; i < static_cast<unsigned int>(HC::SHADERTYPE::COUNT); i++)
		{
			m_Shaders[i] = nullptr;
		}

	}
	virtual ~HCGraphicPipeLine() = default;

	const std::string&	GetPipeLineName() { return m_PipeLineName; }
	const auto&			GetReservedObjects() { return m_RenderReservedObjects; }
	void				RenderReserveObject(const std::string& textureBufferName, const GameObject* object) { m_RenderReservedObjects[textureBufferName].push_back(object); }
	void				ClearReservedObjects() { m_RenderReservedObjects.clear(); }

private:
	std::string												m_PipeLineName;
	IHCSahder*												m_Shaders[static_cast<unsigned int>(HC::SHADERTYPE::COUNT)];
	std::map<std::string, std::vector<const GameObject*>>	m_RenderReservedObjects;
};

class HCGraphic : public IHCDevice
{
public: //pure virtual method
	virtual void		Init(HWND windowHandle) = 0;
	virtual void		Update() = 0;
	virtual void		Resize() = 0;

	virtual HRESULT		CreateGraphicPipeLine(const std::string& pipeLineName, HCGraphicPipeLine** out) = 0;
	virtual HRESULT		CreateTextureBuffer(const std::string& bufferName, IHCTextureBuffer** out) = 0;
	virtual HRESULT		CreateGraphicOut(IHCGraphicOut** out) = 0;
	virtual HRESULT		CreateShader(HC::SHADERTYPE type, const std::wstring& filePath, const std::string& entryPoint, IHCSahder** out) = 0;

public: //Optional virtual function
	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return LRESULT(0); }
	virtual void		NumberingGraphicPipeLineSlot(size_t slot, HCGraphicPipeLine* pipeLine);

public:
	void				ReserveRender(const std::string& pipeLineName, const std::string& textureBufferName, const GameObject* object);
	void				ReserveRender(size_t pipeLineSlot, const std::string& textureBufferName, const GameObject* object);
	void				Render();

protected:
	virtual void		RenderBegin() = 0;
	virtual void		SetPipeLineObject(HCGraphicPipeLine* pipeLine) = 0;
	virtual void		RenderObjects(const std::string& textureBufferName, const std::vector<const GameObject*> objects) = 0;
	virtual void		RenderEnd() = 0;

private:
	virtual std::string GetDeviceName() const override { return typeid(HCGraphic).name(); }

protected:
	std::vector<HCGraphicPipeLine*>	m_PipeLines;
};

