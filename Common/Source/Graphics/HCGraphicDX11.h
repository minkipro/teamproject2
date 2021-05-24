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
	struct TextureArrayInTextureData
	{
		DirectX::XMFLOAT2 StartUV = {};
		DirectX::XMFLOAT2 EndUV = {};
		UINT Index = 0;
		int	 NumSprite = -1;
		UINT Pad1 = 0;
		UINT Pad2 = 0;
	};

	struct SpriteData
	{
		DirectX::XMFLOAT2 StartUV;
		DirectX::XMFLOAT2 EndUV;
	};

	struct Texture2DArrayData
	{
		ComPtr<ID3D11ShaderResourceView>						TextureView;
		ComPtr<ID3D11ShaderResourceView>						TextureInfoView;
		std::vector<TextureArrayInTextureData>					TextureDatas;
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
	virtual void		CreateResource(const std::string& resourceName, const HC::GRAPHIC_RESOURCE_DESC& desc, IHCResource** out) override;
	virtual void		CreateCB(const std::string& bufferName, size_t stride, size_t num, std::unique_ptr<IHCCBuffer>& out) override;
	virtual void		CreateShader(const std::string& shaderName, HC::SHADER_TYPE type, const std::wstring& filePath, const std::string& entryPoint, IHCShader** out) override;

	virtual void		GetGraphicPipeLine(const std::string& pipeLineName, HCGraphicPipeLine** out) override;
	virtual void		GetShaderResource(const std::string& resourceName, IHCResource** out) override;
	virtual void		GetCB(const std::string& bufferName, IHCCBuffer** out) override;
	virtual void		GetShader(const std::string& shaderName, IHCShader** out) override;

	virtual TextureData	GetTextureIndex(const std::wstring& textureName) const override;

	virtual LRESULT		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
private:
	virtual void		RenderBegin() override;
	virtual void		RenderEnd() override;
	virtual void		SetPipeLineObject(const HCGraphicPipeLine* pipeLine) override;
	virtual void		RenderObjects(HCGraphicPipeLine* pipeLine) override;
	virtual void		RenderFont() override;
	virtual IHCFont*	GetFont() override;

private:
	void				CreateBaseSamplers();
void					CreateTextures();
	void				CreateGraphicPipeLineBaseSettings();
	void				CreateInputLayout(size_t inputLayoutHash, const std::vector<HCInputLayoutElement>* inputLayoutEle, HCDX11Shader* vs);

	void				GetSpriteData(const std::wstring& texturePath, std::vector<SpriteData>* out);

private:
	std::unique_ptr<HCSwapchainDX11>										m_swapchain;
	ComPtr<ID3D11Device>													m_device;
	ComPtr<ID3D11DeviceContext>												m_deviceContext;

	std::unique_ptr<IHCCBuffer>												m_mainPassCB;

	ComPtr<ID3D11RasterizerState>											m_baseRasterizer;
	ComPtr<ID3D11DepthStencilState>											m_baseDepthStencilState;
	ComPtr<ID3D11BlendState>												m_baseBlendState;
	std::vector<ComPtr<ID3D11SamplerState>>									m_samplers;

	std::unordered_map<size_t, ComPtr<ID3D11InputLayout>>					m_inputLayout;
	std::unordered_map<std::string, std::unique_ptr<IHCShader>>				m_shaders;
	std::vector<Texture2DArrayData>											m_textures;
	std::unordered_map<std::wstring, UINT>									m_textureBufferIndex;


	bool																	m_resizing = false;
	bool																	m_minimized = false;
	bool																	m_maximized = false;

public:
	std::unique_ptr<IHCFont>												m_font;
};

