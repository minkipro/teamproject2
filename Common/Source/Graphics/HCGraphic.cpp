#include "stdafx.h"
#include "HCGraphic.h"

void HCGraphicPipeLine::SetVertexType(size_t typeHashCode, size_t size, const std::vector<HCInputLayoutElement>* inputLayout)
{
	m_vertexTypeHash = typeHashCode;
	m_vertexSize = size;
	m_vertexLayout = inputLayout;
}
