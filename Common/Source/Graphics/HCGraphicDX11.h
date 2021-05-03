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
	struct TextureInTextureData
	{
		DirectX::XMFLOAT2 StartUV = {};
		DirectX::XMFLOAT2 EndUV = {};
		UINT Index = 0;
		UINT Pad0 = 0;
		UINT Pad1 = 0;
		UINT Pad2 = 0;
	};

	struct Texture2DArrayData
	{
		ComPtr<ID3D11ShaderResourceView>						TextureView;
		ComPtr<ID3D11ShaderResourceView>						TextureInfoView;
		std::vector<TextureInTextureData>						TextureDatas;
		std::unordered_map<std::wstring, UINT>					TextureIndex;
	};

public:
	HCGraphicDX11(HWND windowHandle)
		:HCGraphic(windowHandle)
	{

	}
	virtual ~HCGraphicDX11();
	virtual void		Init();
	virtual void		Update();

	virtual void		CreateGraphicPipeLine(const std::string& pipeLineName, HCGraphicPipeLine** out) override;
	virtual void		CreateShaderResource(const std::string& resourceName, size_t stride, const POINT& size, IHCTexture** out) override;
	virtual void		CreateCB(const char* bufferName, size_t stride, size_t num, void* data, IHCCBuffer** out) override;
	virtual void		CreateShader(const std::string& shaderName, HC::SHADERTYPE type, const std::wstring& filePath, const std::string& entryPoint, IHCShader** out) override;

	virtual void		GetGraphicPipeLine(const std::string& pipeLineName, HCGraphicPipeLine** out) override;
	virtual void		GetShaderResource(const std::string& resourceName, IHCTexture** out) override;
	virtual void		GetCB(const std::string& bufferName, IHCCBuffer** out) override;
	virtual void		GetShader(const std::string& shaderName, IHCShader** out) override;

	virtual int			GetTextureIndex(const std::wstring& textureName) const override;

	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
private:
	virtual void		RenderBegin() override;
	virtual void		RenderEnd() override;
	virtual void		SetPipeLineObject(const HCGraphicPipeLine* pipeLine) override;
	virtual void		RenderObjects(HCGraphicPipeLine* pipeLine) override;
	virtual void		RenderFont() override;

private:
	void				CreateBaseSamplers();
	void				CreateTextures();
	void				CreateGraphicPipeLineBaseSettings();
	void				CreateInputLayout(const HC::InputDataSample* sample, HCDX11Shader* vs);

private:
	std::unique_ptr<HCSwapchainDX11>										m_swapchain;
	ComPtr<ID3D11Device>													m_device;
	ComPtr<ID3D11DeviceContext>												m_deviceContext;

	IHCCBuffer*																m_mainPassCB;
	HC::MainPass															m_mainPass;
	std::unordered_map<std::string, std::unique_ptr<IHCCBuffer>>			m_cbuffers;
	ComPtr<ID3D11RasterizerState>											m_baseRasterizer;
	ComPtr<ID3D11DepthStencilState>											m_baseDepthStencilState;
	ComPtr<ID3D11BlendState>												m_baseBlendState;
	std::vector<ComPtr<ID3D11SamplerState>>									m_samplers;

	std::unordered_map<std::string, ComPtr<ID3D11InputLayout>>				m_inputLayout;
	std::unordered_map<std::string, std::unique_ptr<IHCShader>>				m_shaders;
	std::vector<Texture2DArrayData>											m_textures;
	std::unordered_map<std::wstring, UINT>									m_textureBufferIndex;

	bool																	m_resizing = false;
	bool																	m_minimized = false;
	bool																	m_maximized = false;

	std::unique_ptr<IHCFont>												m_font;
};

