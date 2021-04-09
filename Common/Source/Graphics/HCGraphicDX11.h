#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include "HCGraphic.h"
#include "HCSwapchainDX11.h"
#include "HCGraphicDX11Resources.h"

using Microsoft::WRL::ComPtr;

class HCGraphicDX11 final : public HCGraphic
{
public:
	HCGraphicDX11(HWND windowHandle)
		:HCGraphic(windowHandle)
	{

	}
	virtual ~HCGraphicDX11() = default;

	virtual void		Init();
	virtual void		Update();

	virtual HRESULT		CreateGraphicPipeLine(const std::string& pipeLineName, HCGraphicPipeLine** out);
	virtual HRESULT		CreateTextureBuffer(const std::string& bufferName, IHCTextureBuffer** out) override;
	virtual HRESULT		CreateTexture(const std::string& textureName, const std::wstring& filePath, IHCTexture** out) override;
	virtual HRESULT		CreateShaderResource(const std::string& resourceName, size_t stride, const POINT& size, IHCTexture** out) override;
	virtual HRESULT		CreateCB(const std::string& bufferName, size_t stride, size_t num, IHCCBuffer** out) override;
	virtual HRESULT		CreateShader(const std::string& shaderName, HC::SHADERTYPE type, const std::wstring& filePath, const std::string& entryPoint, IHCShader** out) override;
	virtual HRESULT		CreateInputLayout(const std::string& layoutName, unsigned int numElement, const HCVertexInputLayoutElement* elements, IHCInputLayout** out);

	virtual void		GetGraphicPipeLine(const std::string& pipeLineName, HCGraphicPipeLine** out) override;
	virtual void		GetTextureBuffer(const std::string& bufferName, IHCTextureBuffer** out) override;
	virtual void		GetTexture(const std::string& textureName, IHCTexture** out) override;
	virtual void		GetShaderResource(const std::string& resourceName, IHCTexture** out) override;
	virtual void		GetCB(const std::string& bufferName, IHCCBuffer** out) override;
	virtual void		GetShader(const std::string& shaderName, IHCShader** out) override;
	virtual void		GetInputLayout(const std::string& layoutName, IHCInputLayout** out);

	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
private:
	virtual void		RenderBegin() override;
	virtual void		RenderEnd() override;
	virtual void		ApplyBaseCB() override;
	virtual void		SetPipeLineObject(const HCGraphicPipeLine* pipeLine) override;
	virtual void		RenderObjects(const std::string & textureBufferName, const std::vector<const GameObject*> objects) override;

private:
	void				CreateBaseSamplers();

private:
	std::unique_ptr<HCSwapchainDX11>										m_Swapchain;
	ComPtr<ID3D11Device>													m_Device;
	ComPtr<ID3D11DeviceContext>												m_DeviceContext;

	ComPtr<ID3D11Buffer>													m_BaseCB;
	ComPtr<ID3D11RasterizerState>											m_BaseRasterizer;
	ComPtr<ID3D11DepthStencilState>											m_BaseDepthStencilState;
	ComPtr<ID3D11BlendState>												m_BaseBlendState;
	std::vector<ComPtr<ID3D11SamplerState>>									m_Samplers;

	std::unordered_map<std::string, std::unique_ptr<IHCInputLayout>>		m_InputLayout;
	std::unordered_map<std::string, std::unique_ptr<IHCShader>>				m_Shaders;
	std::unordered_map<std::string, std::unique_ptr<HCDX11Texture>>			m_Textures;
	std::unordered_map<std::string, std::unique_ptr<HCDX11TextureBuffer>>	m_TextureBuffers;

	bool																	m_Resizing = false;
	bool																	m_Minimized = false;
	bool																	m_Maximized = false;
};

