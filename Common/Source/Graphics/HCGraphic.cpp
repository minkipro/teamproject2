#include "stdafx.h"
#include "HCGraphic.h"


void HCGraphic::ReserveRender(const std::string& pipeLineName, const HC::InputDataSample* object)
{
	size_t index = 0;
	for (auto& it : m_PipeLineSlots)
	{
		if (it != nullptr)
		{
			if (pipeLineName == it->GetPipeLineName())
			{
				ReserveRender(index, object);
			}
		}
		index++;
	}
}

void HCGraphic::ReserveRender(size_t pipeLineSlot, const HC::InputDataSample* object)
{
	m_PipeLineSlots[pipeLineSlot]->RenderReserveObject(object);
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

void HCGraphicPipeLine::RenderReserveObject(const HC::InputDataSample* object)
{
	int textureIndex = object->GetTextureIndex();
	size_t textureBufferIndex = (textureIndex > -1) ? (textureIndex>>16) : 0;

	if (textureBufferIndex >= m_renderReservedObjectsByTexture.size())
	{
		m_renderReservedObjectsByTexture.insert(m_renderReservedObjectsByTexture.end(), 
												(textureBufferIndex + 1)- m_renderReservedObjectsByTexture.size(), {});
	}

	m_renderReservedObjectsByTexture[textureBufferIndex].push_back(object);
}

void HCGraphicPipeLine::ClearReservedObjects()
{
	for (auto& it : m_renderReservedObjectsByTexture)
	{
		it.clear();
	}
}
