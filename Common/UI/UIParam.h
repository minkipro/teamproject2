#pragma once
#include <DirectXMath.h>
#include <string>
#include <memory>
#include "UIObject.h"

class UIPanel;

struct ENUM_ELEMENT
{
	int Value = 0;
	std::wstring ElementName;
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
		: m_type(type)
		, m_controlType(UICONTROLTYPE::ORIGIN_DATA)
		, m_paramPtr(nullptr)
		, m_dataType(HC::DATA_TYPE::TYPE_INT)
		, m_enumElementInfo(nullptr)
		, m_strings(nullptr)
		, m_dirtyFlag(false)
	{
	}
	virtual			~UIParam() = default;

	virtual void	Init() override;
	virtual void	Update() override;
	virtual void	Render() override;
	virtual void	UIOn(bool Value) override;

	template<typename T> void	SetTargetParam(const std::wstring& paramName, T* data);
	void						SetEnumParam(const std::wstring& paramName, const std::vector<ENUM_ELEMENT>* elementInfo, int* data);
	void						SetStringParam(const std::wstring& paramName, const std::vector<std::string>* strings, std::string* data);
	void						SetTextHeight(int height);

	bool								IsDirty() { return m_dirtyFlag; }
	std::wstring						GetDataString();
	template<typename T> std::wstring	GetStringFromValue();

private:
	const UIPARAMTYPE				m_type;
	UICONTROLTYPE					m_controlType;
	std::wstring					m_paramName;
	void*							m_paramPtr;
	HC::DATA_TYPE					m_dataType;
	bool							m_dirtyFlag;
	std::shared_ptr<IHCTextData>	m_textRenderer;

	const std::vector<ENUM_ELEMENT>*	m_enumElementInfo;
	const std::vector<std::string>*		m_strings;
};

template<typename T>
inline void UIParam::SetTargetParam(const std::wstring& paramName, T* data)
{
	if (typeid(T) == typeid(int))
	{
		m_dataType = HC::DATA_TYPE::TYPE_INT;
	}
	else if (typeid(T) == typeid(float))
	{
		m_dataType = HC::DATA_TYPE::TYPE_FLOAT;
	}
	else if (typeid(T) == typeid(bool))
	{
		m_dataType = HC::DATA_TYPE::TYPE_BOOL;
	}
	else if (typeid(T) == typeid(unsigned int))
	{
		m_dataType = HC::DATA_TYPE::TYPE_UINT;
	}
	else
	{
		assert(false);
		return;
	}

	m_controlType = UICONTROLTYPE::ORIGIN_DATA;
	m_paramName = paramName;
	m_paramPtr = reinterpret_cast<void*>(data);
	m_enumElementInfo = nullptr;
}

template<typename T>
inline std::wstring UIParam::GetStringFromValue()
{
	std::wstring result;

	result = std::to_wstring(*reinterpret_cast<T*>(m_paramPtr));
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

