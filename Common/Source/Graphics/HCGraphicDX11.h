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
	virtual ~HCGraphicDX11();
	virtual void		Init();
	virtual void		Update();

	virtual void		CreateGraphicPipeLine(const std::string& pipeLineName, HCGraphicPipeLine** out) override;
	virtual void		CreateTextureBuffer(const std::string& bufferName, HCTextureBuffer** out) override;
	virtual void		CreateTexture(const std::wstring& filePath, IHCTexture** out) override;
	virtual void		CreateShaderResource(const std::string& resourceName, size_t stride, const POINT& size, IHCTexture** out) override;
	virtual void		CreateCB(const std::string& bufferName, size_t stride, size_t num, IHCCBuffer** out) override;
	virtual void		CreateShader(const std::string& shaderName, HC::SHADERTYPE type, const std::wstring& filePath, const std::string& entryPoint, IHCShader** out) override;

	virtual void		GetGraphicPipeLine(const std::string& pipeLineName, HCGraphicPipeLine** out) override;
	virtual void		GetTextureBuffer(const std::string& bufferName, HCTextureBuffer** out) override;
	virtual void		GetTexture(const std::string& textureName, IHCTexture** out) override;
	virtual void		GetShaderResource(const std::string& resourceName, IHCTexture** out) override;
	virtual void		GetCB(const std::string& bufferName, IHCCBuffer** out) override;
	virtual void		GetShader(const std::string& shaderName, IHCShader** out) override;

	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
private:
	virtual void		RenderBegin() override;
	virtual void		RenderEnd() override;
	virtual void		SetPipeLineObject(const HCGraphicPipeLine* pipeLine) override;
	virtual void		RenderObjects(const std::string & textureBufferName, const std::vector<const HC::InputDataSample*> objects, size_t offset) override;

private:
	void				CreateBaseSamplers();
	void				CreateInputLayout(const HC::InputDataSample* sample, HCDX11Shader* vs);

private:
	std::unique_ptr<HCSwapchainDX11>										m_swapchain;
	ComPtr<ID3D11Device>													m_device;
	ComPtr<ID3D11DeviceContext>												m_deviceContext;

	std::unique_ptr<IHCDX11ConstBuffer<HC::MainPass>>						m_mainPassCB;
	ComPtr<ID3D11RasterizerState>											m_baseRasterizer;
	ComPtr<ID3D11DepthStencilState>											m_baseDepthStencilState;
	ComPtr<ID3D11BlendState>												m_baseBlendState;
	std::vector<ComPtr<ID3D11SamplerState>>									m_samplers;

	std::unordered_map<std::string, ComPtr<ID3D11InputLayout>>				m_inputLayout;
	std::unordered_map<std::string, std::unique_ptr<IHCShader>>				m_shaders;
	std::unordered_map<std::wstring, std::unique_ptr<HCDX11Texture>>		m_textures;
	std::unordered_map<std::string, std::unique_ptr<HCDX11TextureBuffer>>	m_textureBuffers;

	bool																	m_resizing = false;
	bool																	m_minimized = false;
	bool																	m_maximized = false;

	const std::wstring														m_textureFolderPath = L"./../Common/Texture/";
	std::unique_ptr<IHCFont>												m_font;
};

