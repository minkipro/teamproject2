#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include "HCGraphic.h"

class HCSwapchainDX11;

class HCGraphicDX11 : public HCGraphic
{
public:
	HCGraphicDX11(HWND windowHandle)
		:HCGraphic(windowHandle)
	{

	}
	virtual ~HCGraphicDX11() = default;

	virtual void		Init();
	virtual void		Update();

	virtual HRESULT		CreateGraphicPipeLine(const std::string & pipeLineName, HCGraphicPipeLine * *out) ;
	virtual HRESULT		CreateTextureBuffer(const std::string & bufferName, IHCTextureBuffer * *out);
	virtual HRESULT		CreateTexture(const std::wstring& filePath, IHCTexture** out);
	virtual HRESULT		CreateShaderResource(size_t stride, const POINT& size, IHCTexture** out);
	virtual HRESULT		CreateCB(size_t stride, size_t num, IHCCBuffer** out);
	virtual HRESULT		CreateShader(HC::SHADERTYPE type, const std::wstring & filePath, const std::string & entryPoint, IHCShader * *out);

	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	virtual void		RenderBegin();
	virtual void		RenderEnd();
	virtual void		ApplyBaseCB();
	virtual void		SetPipeLineObject(HCGraphicPipeLine * pipeLine);
	virtual void		RenderObjects(const std::string & textureBufferName, const std::vector<const GameObject*> objects);

private:
	std::unique_ptr<HCSwapchainDX11>			m_Swapchain;
	Microsoft::WRL::ComPtr<ID3D11Device>		m_Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_DeviceContext;

	bool										m_Resizing = false;
	bool										m_Minimized = false;
	bool										m_Maximized = false;
};

