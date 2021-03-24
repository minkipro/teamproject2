#pragma once
#include "Shaders.h"

class Graphics
{
public:
	bool Initialize(HWND hwnd, int width, int height);
	void ClearFrame();
	void Present();
	void SetTopologyAndShader(bool tri=true, bool isColor=false);

	ID3D11Device* GetDevice() { return device.Get(); }
	ID3D11DeviceContext* GetDeviceContext() { return deviceContext.Get(); }

private:
	bool InitializeDirectX(HWND hwnd, int width, int height);
	bool InitializeShaders();

private:
	float winWidth = 0.0f;
	float winHeight = 0.0f;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

	VertexShader vertexshader;
	VertexShader color_vertexshader;
	PixelShader pixelshader;
	PixelShader color_pixelshader;


	
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
};

