#include "stdafx.h"
#include "HCGraphic.h"


void HCGraphic::ReserveRender(const std::string& pipeLineName, const std::string& textureBufferName,const GameObject* object)
{
	size_t index = 0;
	for (auto& it : m_PipeLineSlots)
	{
		if (it != nullptr)
		{
			if (pipeLineName == it->GetPipeLineName())
			{
				ReserveRender(index, textureBufferName, object);
			}
		}

		index++;
	}
}

void HCGraphic::ReserveRender(size_t pipeLineSlot, const std::string& textureBufferName,const GameObject* object)
{
	m_PipeLineSlots[pipeLineSlot]->RenderReserveObject(textureBufferName, object);
}

void HCGraphic::Render()
{
	RenderBegin();

	ApplyBaseCB();

	for (auto& it : m_PipeLineSlots)
	{
		SetPipeLineObject(it);

		for (auto& it2 : it->GetReservedObjects())
		{
			RenderObjects(it2.first, it2.second);
		}
	}

	RenderEnd();
}

void HCGraphic::NumberingGraphicPipeLineSlot(size_t slot, HCGraphicPipeLine* pipeLine)
{
	if (m_PipeLineSlots.size() <= slot)
	{
		m_PipeLineSlots.resize(slot);
	}

	m_PipeLineSlots[slot] = pipeLine;
}

void HCGraphic::NumberingGraphicPipeLineSlot(size_t slot, const std::string& pipelineName)
{
	auto iter = m_PipeLines.find(pipelineName);

	COM_THROW_IF_FAILED(iter != m_PipeLines.end(), pipelineName + " pipeline is not created.");

	if (m_PipeLineSlots.size() <= slot)
	{
		m_PipeLineSlots.resize(slot);
	}

	m_PipeLineSlots[slot] = iter->second.get();
}