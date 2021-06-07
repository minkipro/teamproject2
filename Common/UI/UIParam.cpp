#include "stdafx.h"
#include "UIParam.h"
#include "Graphics\HCGraphic.h"


void UIParam::SetEnumParam(const std::wstring& paramName, const std::vector<ENUM_ELEMENT>* elementInfo, int* data)
{
	m_controlType = UICONTROLTYPE::ENUM_DATA;
	m_paramName = paramName;
	m_paramPtr = reinterpret_cast<void*>(data);
	m_enumElementInfo = elementInfo;
}

void UIParam::SetStringParam(const std::wstring& paramName, const std::vector<std::string>* strings, std::string* data)
{
	m_controlType = UICONTROLTYPE::STRING_DATA;
	m_paramName = paramName;
	m_paramPtr = reinterpret_cast<void*>(data);
	m_strings = strings;
}

void UIParam::SetTextHeight(int height)
{

}

void UIParam::Init()
{
	UIObject::Init();
	HCDEVICE(HCGraphic)->CreateTextData(m_textRenderer);
}

void UIParam::Update()
{
	if (m_isOn)
	{
		if (m_parent)
		{
			m_renderInfo.Position.x = reinterpret_cast<UIParam*>(m_parent)->m_renderInfo.Position.x + (m_pos.x - m_renderInfo.Size.x * m_benchUV.x);
			m_renderInfo.Position.y = reinterpret_cast<UIParam*>(m_parent)->m_renderInfo.Position.y + (m_pos.y - m_renderInfo.Size.y * m_benchUV.y);
			m_renderInfo.Position.z = reinterpret_cast<UIParam*>(m_parent)->m_renderInfo.Position.z + m_pos.z;
		}
		else
		{
			m_renderInfo.Position.x = (m_pos.x - m_renderInfo.Size.x * m_benchUV.x);
			m_renderInfo.Position.y = (m_pos.y - m_renderInfo.Size.y * m_benchUV.y);
			m_renderInfo.Position.z = m_pos.z;
		}

		m_textRenderer->m_position = m_renderInfo.Position;

		if (m_paramPtr)
		{
			/*m_Font->m_Pos.x = pos.x;
			m_Font->m_Pos.y = pos.y;
			m_Font->m_Pos.z = pos.z;*/

			//std::wstring text = m_ParamName + L" : ";

			switch (m_type)
			{
			case UIParam::UIPARAMTYPE::VIEWER:
			{
			}
			break;
			case UIParam::UIPARAMTYPE::MODIFIER:
			{

			}
			break;
			default:
				break;
			}
		}

		for (auto& it : m_childs)
		{
			it->Update();
		}
	}
}

void UIParam::Render()
{

}

void UIParam::UIOn(bool Value)
{
	UIObject::UIOn(Value);

	if (Value)
	{

	}
	else
	{

	}
}

std::wstring UIParam::GetDataString()
{
	std::wstring result;

	switch (m_controlType)
	{
	case UIParam::UICONTROLTYPE::ORIGIN_DATA:
	{
		switch (m_dataType)
		{
		case HC::DATA_TYPE::TYPE_BOOL:
			result = *(reinterpret_cast<bool*>(m_paramPtr)) ? L"true" : L"false";
			break;
		case HC::DATA_TYPE::TYPE_FLOAT:
			result = GetStringFromValue<float>();
			break;
		case HC::DATA_TYPE::TYPE_INT:
			result = GetStringFromValue<int>();
			break;
		case HC::DATA_TYPE::TYPE_UINT:
			result = GetStringFromValue<unsigned int>();
			break;
		default:
			assert(false);
			break;
		}
	}
	break;
	case UIParam::UICONTROLTYPE::ENUM_DATA:
	{
		int paramValue = *reinterpret_cast<int*>(m_paramPtr);
		bool isNotValidValue = true;

		for (auto& it : *m_enumElementInfo)
		{
			if (it.Value == paramValue)
			{
				result = it.ElementName + L"(" + std::to_wstring(paramValue) + L")";
				isNotValidValue = false;
				break;
			}
		}

		if (isNotValidValue)
		{
			auto& element = m_enumElementInfo->front();
			*reinterpret_cast<int*>(m_paramPtr) = element.Value;
			result = element.ElementName + L"(" + std::to_wstring(element.Value) + L")";
		}
	}
	break;
	case UIParam::UICONTROLTYPE::STRING_DATA:
	{
		std::string* targetString = reinterpret_cast<std::string*>(m_paramPtr);
		result.insert(result.end(), targetString->begin(), targetString->end());
	}
	break;
	default:
		break;
	}

	return result;
}

//void UIParam::ParamController::Excute()
//{
//	if (m_InputData.size())
//	{
//		switch (m_CurrParam->m_DataType)
//		{
//		case CGH::DATA_TYPE::TYPE_BOOL:
//			*reinterpret_cast<bool*>(m_CurrParam->m_ParamPtr) = atoi(m_InputData.c_str());
//			break;
//		case CGH::DATA_TYPE::TYPE_FLOAT:
//			*reinterpret_cast<float*>(m_CurrParam->m_ParamPtr) = static_cast<float>(atof(m_InputData.c_str()));
//			break;
//		case CGH::DATA_TYPE::TYPE_INT:
//			*reinterpret_cast<int*>(m_CurrParam->m_ParamPtr) = atoi(m_InputData.c_str());
//			break;
//		case CGH::DATA_TYPE::TYPE_UINT:
//			*reinterpret_cast<UINT*>(m_CurrParam->m_ParamPtr) = atoi(m_InputData.c_str());
//			break;
//		default:
//			break;
//		}
//
//		if (m_CurrParam->m_DirtyCall)
//		{
//			m_CurrParam->m_DirtyCall();
//		}
//	}
//
//	WorkClear();
//	WorkEnd();
//}