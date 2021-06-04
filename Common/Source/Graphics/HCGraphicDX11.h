#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
#include "HCGraphic.h"
#include "HCSwapchainDX11.h"
#include "HCGraphicDX11Resources.h"
#include "HCFont.h"

using Microsoft::WRL::ComPtr;

class HCGraphicDX11 final : public HCGraphic
{
	struct TextureSpriteData
	{
		DirectX::XMFLOAT2 StartUV = {};
		DirectX::XMFLOAT2 EndUV = {};
	};

	struct TextureResourceData
	{
		ComPtr<ID3D11ShaderResourceView>						TextureView;
		D3D11_SHADER_RESOURCE_VIEW_DESC							TextureDesc;
		UINT													SpriteNum = 1;
		UINT													SpriteInfoStartIndex = 0;
	};

public:
	HCGraphicDX11(HWND windowHandle)
		:HCGraphic(windowHandle)
	{

	}
	virtual ~HCGraphicDX11();

	virtual void			Init();
	virtual void			Update();

	virtual void			CreateResource(const HC::GRAPHIC_RESOURCE_DESC& desc, std::shared_ptr<IHCResource>& out) override;
	virtual void			CreateShader(HC::SHADER_TYPE type, const std::wstring& filePath, const std::string& entryPoint, std::shared_ptr<IHCShader>& out) override;
	virtual void			CreateTextData(std::shared_ptr<IHCTextData>& out) override;

	virtual void			CopyResource(std::shared_ptr<IHCResource> dest, std::shared_ptr<IHCResource> src) override;

	virtual void			RenderBegin() override;
	virtual void			RenderEnd() override;

	virtual void			SetPipeLineObject(const HCGraphicPipeLine* pipeLine) override;
	virtual void			SetTexture(UINT textureIndex, UINT shaderResourceSlot) override;
	virtual void			SetTextures(const std::vector<UINT>& textureIndices, UINT shaderResourceStartSlot) override;
	virtual void			SetShaderResource(std::shared_ptr<IHCResource> resource, UINT shaderResourceSlot) override;
	virtual void			SetShaderResources(const std::vector<std::shared_ptr<IHCResource>>& resources, UINT shaderResourceStartSlot) override;
	virtual void			SetConstantBuffer(std::shared_ptr<IHCResource> buffer, UINT constantBufferSlot) override;
	virtual void			SetConstantBuffers(const std::vector<std::shared_ptr<IHCResource>>& buffers, UINT constantBufferStartSlot) override;

	virtual void			Draw(const HCMesh* mesh) override;
	virtual void			DrawInsatance(const HCMesh* mesh, UINT numInstance, UINT InstanceOffset = 0) override;
	virtual void			DrawIndexed(const HCMesh* mesh) override;
	virtual void			DrawIndexedInsatance(const HCMesh* mesh, UINT numInstance, UINT InstanceOffset = 0) override;
	virtual void			DrawFont() override;

	virtual HCTextureData	GetTextureIndex(const std::wstring& textureName) const override;

	virtual LRESULT			WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
	void						CreateBaseSamplers();
	void						CreateTextures();
	void						CreateGraphicPipeLineBaseSettings();
	void						CreateInputLayout(size_t inputLayoutHash, const std::vector<HCInputLayoutElement>* inputLayoutEle, HCDX11Shader* vs);

	D3D11_USAGE					GetResourceUsage(const HC::GRAPHIC_RESOURCE_DESC& desc);
	D3D11_CPU_ACCESS_FLAG		GetResourceCpuAcessFlags(const HC::GRAPHIC_RESOURCE_DESC& desc);
	D3D11_RESOURCE_MISC_FLAG	GetResourceMiscFlags(const HC::GRAPHIC_RESOURCE_DESC& desc);
	D3D11_BIND_FLAG				GetResourceBindFlags(const HC::GRAPHIC_RESOURCE_DESC& desc);
	void						GetSpriteData(const std::wstring& texturePath, std::vector<TextureSpriteData>* out);

private:
	std::unique_ptr<HCSwapchainDX11>										m_swapchain;
	ComPtr<ID3D11Device>													m_device;
	ComPtr<ID3D11DeviceContext>												m_deviceContext;

	ComPtr<ID3D11RasterizerState>											m_baseRasterizer;
	ComPtr<ID3D11DepthStencilState>											m_baseDepthStencilState;
	ComPtr<ID3D11BlendState>												m_baseBlendState;
	std::vector<ComPtr<ID3D11SamplerState>>									m_samplers;

	std::unordered_map<size_t, ComPtr<ID3D11InputLayout>>					m_vertexLayout;

	std::vector<HCMesh>														m_meshes;
	std::vector<TextureSpriteData>											m_allSpriteDatas;
	ComPtr<ID3D11ShaderResourceView>										m_textureInfoView;
	std::vector<TextureResourceData>										m_textures;
	std::unordered_map<std::wstring, UINT>									m_textureIndex;

	const HCGraphicPipeLine*												m_currPipeLine = nullptr;

	DX11FontMG																m_font;

	bool																	m_resizing = false;
	bool																	m_minimized = false;
	bool																	m_maximized = false;
};

