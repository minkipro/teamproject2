#include "stdafx.h"
#include "Shaders.h"
#include "COMException.h"

bool VertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements)
{
	COM_HRESULT_IF_FAILED(D3DReadFileToBlob(
		shaderpath.c_str(), this->shader_buffer.GetAddressOf()),
		L"Failed to load shader: " + shaderpath);
	
	COM_HRESULT_IF_FAILED(device->CreateVertexShader(
		this->shader_buffer->GetBufferPointer(), 
		this->shader_buffer->GetBufferSize(), NULL, 
		this->shader.GetAddressOf()), 
		L"Failed to create vertex shader: " + shaderpath);

	COM_HRESULT_IF_FAILED(device->CreateInputLayout(
		layoutDesc, numElements,
		this->shader_buffer->GetBufferPointer(), 
		this->shader_buffer->GetBufferSize(), 
		this->inputLayout.GetAddressOf()), L"Failed to create input layout");

	return true;
}

ID3D11VertexShader* VertexShader::GetShader()
{
	return this->shader.Get();
}

ID3D10Blob* VertexShader::GetBuffer()
{
	return this->shader_buffer.Get();
}

ID3D11InputLayout* VertexShader::GetInputLayout()
{
	return this->inputLayout.Get();
}

bool PixelShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath)
{
	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->shader_buffer.GetAddressOf());
	
	COM_HRESULT_IF_FAILED(hr, L"Failed to load shaer: " + shaderpath);

	hr = device->CreatePixelShader(this->shader_buffer.Get()->GetBufferPointer(), this->shader_buffer.Get()->GetBufferSize(), NULL, this->shader.GetAddressOf());
	
	COM_HRESULT_IF_FAILED(hr, L"Failed to create pixel shader: " + shaderpath);

	return true;
}

ID3D11PixelShader* PixelShader::GetShader()
{
	return this->shader.Get();
}

ID3D10Blob* PixelShader::GetBuffer()
{
	return this->shader_buffer.Get();
}

bool GeometryShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath)
{
	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->shader_buffer.GetAddressOf());

	COM_HRESULT_IF_FAILED(hr, L"Failed to load shaer: " + shaderpath);

	hr = device->CreateGeometryShader(this->shader_buffer.Get()->GetBufferPointer(), this->shader_buffer.Get()->GetBufferSize(), NULL, this->shader.GetAddressOf());

	COM_HRESULT_IF_FAILED(hr, L"Failed to create geometry shader: " + shaderpath);

	return true;
}

ID3D11GeometryShader* GeometryShader::GetShader()
{
	return this->shader.Get();
}

ID3D10Blob* GeometryShader::GetBuffer()
{
	return this->shader_buffer.Get();
}
