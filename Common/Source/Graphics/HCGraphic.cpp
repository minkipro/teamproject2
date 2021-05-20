#include "stdafx.h"
#include "HCGraphic.h"

std::vector<HCInputLayoutElement> RenderPoint::InputLayout = {
				{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT},
				{"RENDERSIZE",0,DXGI_FORMAT_R32G32_FLOAT},
				{"TEXCOORD",0,DXGI_FORMAT_R32G32B32A32_FLOAT},
				{"TEXINDEX",0,DXGI_FORMAT_R32_SINT} };
std::vector<HCInputLayoutElement> RenderPointUV::InputLayout = {
				{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT},
				{"RENDERSIZE",0,DXGI_FORMAT_R32G32_FLOAT},
				{"TEXCOORD",0,DXGI_FORMAT_R32G32B32A32_FLOAT},
				{"UV",0,DXGI_FORMAT_R32G32B32A32_FLOAT},
				{"TEXINDEX",0,DXGI_FORMAT_R32_SINT} };

void HCGraphic::ReserveRender(const std::string& pipeLineName, const void* object, int textureIndex)
{
	size_t index = 0;
	for (auto& it : m_PipeLineSlots)
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
	m_PipeLineSlots[pipeLineSlot]->RenderReserveObject(object, textureIndex);
}

void HCGraphic::Render()
{
	RenderBegin();

	for (auto& it : m_PipeLineSlots)
	{
		if (it)
		{
			SetPipeLineObject(it);
			RenderObjects(it);
			it->ClearReservedObjects();
		}
	}

	RenderFont();
	RenderEnd();
}

void HCGraphic::NumberingGraphicPipeLineSlot(size_t slot, HCGraphicPipeLine* pipeLine)
{
	if (m_PipeLineSlots.size() <= slot)
	{
		m_PipeLineSlots.resize(slot + 1);
	}
	m_PipeLineSlots[slot] = pipeLine;
}

void HCGraphic::NumberingGraphicPipeLineSlot(size_t slot, const std::string& pipelineName)
{
	auto iter = m_PipeLines.find(pipelineName);

	COM_THROW_IF_FAILED(iter != m_PipeLines.end(), pipelineName + " pipeline is not created.");

	if (m_PipeLineSlots.size() <= slot)
	{
		m_PipeLineSlots.resize(slot + 1);
	}

	m_PipeLineSlots[slot] = iter->second.get();
}

void HCGraphicPipeLine::RenderReserveObject(const void* inputData, int textureIndex)
{
	size_t textureBufferIndex = (textureIndex > -1) ? (textureIndex>>16) : 0;

	if (textureBufferIndex >= m_renderReservedObjectsByTexture.size())
	{
		m_renderReservedObjectsByTexture.insert(m_renderReservedObjectsByTexture.end(), 
												(textureBufferIndex + 1)- m_renderReservedObjectsByTexture.size(), {});
	}

	auto& currBuffer = m_renderReservedObjectsByTexture[textureBufferIndex];
	size_t currSize = currBuffer.size();

	currBuffer.insert(currBuffer.end(), m_InputDataSize, 0);
	memcpy(currBuffer.data() + currSize, inputData, m_InputDataSize);
}

void HCGraphicPipeLine::ClearReservedObjects()
{
	for (auto& it : m_renderReservedObjectsByTexture)
	{
		it.clear();
	}
}
