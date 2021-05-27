#include "stdafx.h"
#include "UIParam.h"
#include "Graphics\HCGraphic.h"


void UIParam::SetEnumParam(const std::wstring& paramName, const std::vector<ENUM_ELEMENT>* elementInfo, int* data)
{
	m_ControlType = UICONTROLTYPE::ENUM_DATA;
	m_ParamName = paramName;
	m_ParamPtr = reinterpret_cast<void*>(data);
	m_EnumElementInfo = elementInfo;
}

void UIParam::SetStringParam(const std::wstring& paramName, const std::vector<std::string>* strings, std::string* data)
{
	m_ControlType = UICONTROLTYPE::STRING_DATA;
	m_ParamName = paramName;
	m_ParamPtr = reinterpret_cast<void*>(data);
	m_Strings = strings;
}

void UIParam::SetTextHeight(int height)
{

}

void UIParam::Init()
{

}

void UIParam::Update()
{
	if (m_isOn)
	{
		UIObject::Update();

		if (m_ParamPtr)
		{
			/*m_Font->m_Pos.x = pos.x;
			m_Font->m_Pos.y = pos.y;
			m_Font->m_Pos.z = pos.z;*/

			//std::wstring text = m_ParamName + L" : ";

			switch (m_Type)
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
	}
}

std::wstring UIParam::GetDataString()
{
	std::wstring result;

	switch (m_ControlType)
	{
	case UIParam::UICONTROLTYPE::ORIGIN_DATA:
	{
	/*	switch (m_DataType)
		{
		case HC::DATA_TYPE::TYPE_BOOL:
			result = *(reinterpret_cast<bool*>(m_ParamPtr)) ? L"true" : L"false";
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
		}*/
	}
	break;
	case UIParam::UICONTROLTYPE::ENUM_DATA:
	{
		int paramValue = *reinterpret_cast<int*>(m_ParamPtr);
		bool isNotValidValue = true;

		for (auto& it : *m_EnumElementInfo)
		{
			if (it.value == paramValue)
			{
				result = it.elementName + L"(" + std::to_wstring(paramValue) + L")";
				isNotValidValue = false;
				break;
			}
		}

		if (isNotValidValue)
		{
			auto& element = m_EnumElementInfo->front();
			*reinterpret_cast<int*>(m_ParamPtr) = element.value;
			result = element.elementName + L"(" + std::to_wstring(element.value) + L")";
		}
	}
	break;
	case UIParam::UICONTROLTYPE::STRING_DATA:
	{
		std::string* targetString = reinterpret_cast<std::string*>(m_ParamPtr);
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