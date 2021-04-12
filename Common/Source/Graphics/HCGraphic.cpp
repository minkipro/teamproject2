#include "stdafx.h"
#include "HCGraphic.h"


void HCGraphic::ReserveRender(const std::string& pipeLineName, const std::string& textureBufferName,const HC::InputDataSample* object)
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

void HCGraphic::ReserveRender(size_t pipeLineSlot, const std::string& textureBufferName,const HC::InputDataSample* object)
{
	m_PipeLineSlots[pipeLineSlot]->RenderReserveObject(textureBufferName, object);
}

void HCGraphic::Render()
{
	RenderBegin();

	for (auto& it : m_PipeLineSlots)
	{
		if (it)
		{
			SetPipeLineObject(it);

			size_t offsetAccumulate = 0;
			for (auto& it2 : it->GetReservedObjects())
			{
				RenderObjects(it2.first, it2.second, offsetAccumulate);
				offsetAccumulate += it2.second.size();
			}
		}
	}

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