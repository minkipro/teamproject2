#include "stdafx.h"
#include "HCGraphic.h"


void HCGraphic::ReserveRender(const std::string& pipeLineName, const std::string& textureBufferName,const GameObject* object)
{
	size_t index = 0;
	for (auto& it : m_PipeLines)
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
	m_PipeLines[pipeLineSlot]->RenderReserveObject(textureBufferName, object);
}

void HCGraphic::Render()
{
	RenderBegin();

	for (auto& it : m_PipeLines)
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
	if (m_PipeLines.size() <= slot)
	{
		m_PipeLines.resize(slot);
	}

	m_PipeLines[slot] = pipeLine;
}