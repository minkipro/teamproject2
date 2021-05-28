#include "stdafx.h"
#include "HCGraphic.h"

std::vector<HCInputLayoutElement> RenderPoint::InputLayout = {
				{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT},
				{"RENDERSIZE",0,DXGI_FORMAT_R32G32_FLOAT},
				{"TEXCOORD",0,DXGI_FORMAT_R32G32B32A32_FLOAT},
				{"TEXINDEX",0,DXGI_FORMAT_R32_SINT} };

void HCGraphic::NumberingGraphicPipeLineSlot(size_t slot, std::shared_ptr<HCGraphicPipeLine> pipeLine)
{
	if (m_pipeLineSlots.size() <= slot)
	{
		m_pipeLineSlots.resize(slot + 1);
	}
	m_pipeLineSlots[slot] = pipeLine;
}

void HCGraphic::ReserveRender(const std::string& pipeLineName, const void* object, int textureIndex)
{
	size_t index = 0;
	for (auto& it : m_pipeLineSlots)
	{
		if (it != nullptr)
		{
			if (pipeLineName == it->GetPipeLineName())
			{
				ReserveRender(index, object, textureIndex);
			}
		}
		index++;
	}
}

void HCGraphic::ReserveRender(size_t pipeLineSlot, const void* object, int textureIndex)
{
	m_pipeLineSlots[pipeLineSlot]->RenderReserveObject(object, textureIndex);
}

void HCGraphic::Render()
{
	RenderBegin();

	for (auto& it : m_pipeLineSlots)
	{
		if (it.get())
		{
			SetPipeLineObject(it.get());
			RenderObjects(it.get());
			it->ClearReservedObjects();
		}
	}

	RenderFont();
	RenderEnd();
}

void HCGraphicPipeLine::RenderReserveObject(const void* inputData, int textureIndex)
{
	size_t textureBufferIndex = (textureIndex > -1) ? (textureIndex >> 20) : 0;

	if (textureBufferIndex >= m_renderReservedObjectsByTexture.size())
	{
		m_renderReservedObjectsByTexture.insert(m_renderReservedObjectsByTexture.end(),
			(textureBufferIndex + 1) - m_renderReservedObjectsByTexture.size(), {});
	}

	auto& currBuffer = m_renderReservedObjectsByTexture[textureBufferIndex];
	size_t currSize = currBuffer.size();

	currBuffer.insert(currBuffer.end(), m_InputDataSize, 0);
	CopyMemory(currBuffer.data() + currSize, inputData, m_InputDataSize);
}

void HCGraphicPipeLine::ClearReservedObjects()
{
	for (auto& it : m_renderReservedObjectsByTexture)
	{
		it.clear();
	}
}
