#pragma once
#include "UIObject.h"
#include <DirectXMath.h>
#include <string>
#include <memory>

class UIPanel;

struct ENUM_ELEMENT
{
	int value = 0;
	std::wstring elementName;
};

class UIParam :public UIObject
{
public:
	enum class UIPARAMTYPE
	{
		VIEWER,
		MODIFIER,
	};

	enum class UICONTROLTYPE
	{
		ORIGIN_DATA,
		ENUM_DATA,
		STRING_DATA
	};

public:
	UIParam(UIPARAMTYPE type)
		: m_Type(type)
		, m_ControlType(UICONTROLTYPE::ORIGIN_DATA)
		, m_ParamPtr(nullptr)
		, m_DataType(HC::DATA_TYPE::TYPE_INT)
		, m_EnumElementInfo(nullptr)
		, m_Strings(nullptr)
		, m_DirtyFlag(false)
	{
	}
	virtual			~UIParam() = default;

	virtual void	Init() override;
	virtual void	Update() override;

	template<typename T> void	SetTargetParam(const std::wstring& paramName, T* data);
	void						SetEnumParam(const std::wstring& paramName, const std::vector<ENUM_ELEMENT>* elementInfo, int* data);
	void						SetStringParam(const std::wstring& paramName, const std::vector<std::string>* strings, std::string* data);
	void						SetTextHeight(int height);

	bool								IsDirty() { return m_DirtyFlag; }
	std::wstring						GetDataString();
	template<typename T> std::wstring	GetStringFromValue();

private:
	const UIPARAMTYPE	m_Type;
	UICONTROLTYPE		m_ControlType;
	std::wstring		m_ParamName;
	void*				m_ParamPtr;
	HC::DATA_TYPE		m_DataType;
	bool				m_DirtyFlag;

	const std::vector<ENUM_ELEMENT>*	m_EnumElementInfo;
	const std::vector<std::string>*		m_Strings;
};

template<typename T>
inline void UIParam::SetTargetParam(const std::wstring& paramName, T* data)
{
	if (typeid(T) == typeid(int))
	{
		m_DataType = CGH::DATA_TYPE::TYPE_INT;
	}
	else if (typeid(T) == typeid(float))
	{
		m_DataType = CGH::DATA_TYPE::TYPE_FLOAT;
	}
	else if (typeid(T) == typeid(bool))
	{
		m_DataType = CGH::DATA_TYPE::TYPE_BOOL;
	}
	else if (typeid(T) == typeid(unsigned int))
	{
		m_DataType = CGH::DATA_TYPE::TYPE_UINT;
	}
	else
	{
		assert(false);
		return;
	}

	m_ControlType = UICONTROLTYPE::ORIGIN_DATA;
	m_ParamName = paramName;
	m_ParamPtr = reinterpret_cast<void*>(data);
	m_EnumElementInfo = nullptr;
	m_DirtyCall = nullptr;
}

template<typename T>
inline std::wstring UIParam::GetStringFromValue()
{
	std::wstring result;

	result = std::to_wstring(*reinterpret_cast<T*>(m_ParamPtr));
	size_t period = result.find(L'.');

	if (period < result.size())
	{
		while (result.size())
		{
			if (result.back() == L'0')
			{
				if (period == result.size() - 2)
				{
					break;
				}

				result.pop_back();
			}
			else
			{

				break;
			}
		}
	}

	return result;
}

