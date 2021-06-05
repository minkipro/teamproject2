#include "stdafx.h"
#include "HCPickingTexture.h"

void HCPickingTexture::Init(UINT sizeX, UINT sizeY)
{
	auto graphic = HCDEVICE(HCGraphic);

	m_size.x = sizeX;
	m_size.y = sizeY;
	m_data.resize(static_cast<size_t>(sizeX) * sizeY);

	HC::GRAPHIC_RESOURCE_DESC renderTargetTextureDesc;
	HC::GRAPHIC_RESOURCE_DESC stagingTextureDesc;

	renderTargetTextureDesc.Type = HC::GRAPHIC_RESOURCE_TYPE::GRAPHIC_RESOURCE_TEXTURE2D;
	renderTargetTextureDesc.UsageType = HC::GRAPHIC_RESOURCE_USAGE_TYPE::GRAPHIC_RESOURCE_USAGE_DEFAULT;
	renderTargetTextureDesc.BindFlags = HC::GRAPHIC_RESOURCE_BIND_RENDER_TARGET;
	renderTargetTextureDesc.Stride = sizeof(int);
	renderTargetTextureDesc.Texture.Size.x = sizeX;
	renderTargetTextureDesc.Texture.Size.y = sizeY;
	renderTargetTextureDesc.Texture.Size.z = 0;
	renderTargetTextureDesc.Texture.Format = DXGI_FORMAT_R32_SINT;
	renderTargetTextureDesc.Texture.ArrayNum = 1;

	stagingTextureDesc.Type = HC::GRAPHIC_RESOURCE_TYPE::GRAPHIC_RESOURCE_TEXTURE2D;
	stagingTextureDesc.UsageType = HC::GRAPHIC_RESOURCE_USAGE_TYPE::GRAPHIC_RESOURCE_USAGE_STAGING;
	stagingTextureDesc.BindFlags = HC::GRAPHIC_RESOURCE_BIND_NONE;
	stagingTextureDesc.Stride = sizeof(int);
	stagingTextureDesc.Texture.Size.x = sizeX;
	stagingTextureDesc.Texture.Size.y = sizeY;
	stagingTextureDesc.Texture.Size.z = 0;
	stagingTextureDesc.Texture.Format = DXGI_FORMAT_R32_SINT;
	stagingTextureDesc.Texture.ArrayNum = 1;

	graphic->CreateResource(renderTargetTextureDesc, m_renderTargetTexture);
	graphic->CreateResource(stagingTextureDesc, m_stagingTexture);
}

void HCPickingTexture::Update()
{
	auto graphic = HCDEVICE(HCGraphic);

	graphic->CopyResource(m_stagingTexture, m_renderTargetTexture);
}

void HCPickingTexture::CopyGpuDataToCpu()
{
	m_stagingTexture->Map();

	BYTE* mappedData = m_stagingTexture->GetMappedDataPtr();
	UINT gpuRowPitch = m_stagingTexture->GetMappedDataRowPitch();
	UINT dataWidth = m_size.x * sizeof(int);

	for (size_t i = 0; i < m_size.y; i++)
	{
		CopyMemory(&m_data[i * m_size.x], mappedData, dataWidth);
		mappedData += gpuRowPitch;
	}

	m_stagingTexture->UnMap();
}

int HCPickingTexture::GetData(UINT indexX, UINT indexY)
{
	return m_data[indexX + (static_cast<size_t>(indexY) * m_size.x)];
}

