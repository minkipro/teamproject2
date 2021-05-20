#include "UIParam.h"
#include "UIPanel.h"
#include "UIButton.h"
#include "GraphicDO.h"
#include "PhysicsDO.h"
#include "d3dApp.h"

UIParam::ParamController UIParam::s_ParamController;

void UIParam::Delete()
{
	s_ParamController.WorkClear();
	GameObject::Delete();
}

void UIParam::AddUIParam(UIParam* param)
{
	if (param)
	{
		if (param->m_Parent->Is<UIPanel>())
		{
			reinterpret_cast<UIPanel*>(param->m_Parent)->PopUICom(param);
		}

		param->SetParent(this);
		param->m_Font->m_FontHeight = m_Font->m_FontHeight;
	}
}

void UIParam::SetPos(const physx::PxVec3& pos)
{
	m_Trans->SetPosX(pos.x - m_Size.x * m_BenchUV.x);
	m_Trans->SetPosY(pos.y - m_Size.y * m_BenchUV.y);
	m_Trans->SetPosZ(pos.z);
}

void UIParam::SetEnumParam(const std::wstring& paramName, const std::vector<ENUM_ELEMENT>* elementInfo, int* data)
{
	m_ControlType = UICONTROLTYPE::ENUM_DATA;
	m_ParamName = paramName;
	m_ParamPtr = reinterpret_cast<void*>(data);
	m_EnumElementInfo = elementInfo;
	m_DirtyCall = nullptr;
}

void UIParam::SetStringParam(const std::wstring& paramName, const std::vector<std::string>* strings, std::string* data)
{
	m_ControlType = UICONTROLTYPE::STRING_DATA;
	m_ParamName = paramName;
	m_ParamPtr = reinterpret_cast<void*>(data);
	m_Strings = strings;
	m_DirtyCall = nullptr;
}

void UIParam::SetTextHeight(int height)
{
	m_Font->m_FontHeight = height;
}

void UIParam::SetDirtyCall(std::function<void()> dirtyCall)
{
	m_DirtyCall = dirtyCall;
}

void UIParam::Init()
{
	m_Trans = CreateComponenet<DOTransform>();
	m_Font = CreateComponenet<DOFont>();
	m_Font->SetFont(RenderFont::fontNames.front());

	auto Trans = GetComponent<DOTransform>();

	if (m_Type == UIPARAMTYPE::MODIFIER)
	{
		m_Font->AddPixelFunc(std::bind(&ParamController::SetUIParam, &s_ParamController, this),
			DirectX::Mouse::ButtonStateTracker::RELEASED, DirectX::MOUSEBUTTONINDEX::LEFTBUTTON);
	}
}

void UIParam::Update(float delta)
{
	physx::PxVec3 pos = m_Trans->GetTransform().p;
	float topY = pos.y;

	if (m_ParamPtr)
	{
		m_Font->m_Pos.x = pos.x;
		m_Font->m_Pos.y = pos.y;
		m_Font->m_Pos.z = pos.z;

		std::wstring text = m_ParamName + L" : ";

		switch (m_Type)
		{
		case UIParam::UIPARAMTYPE::VIEWER:
		{
			text += GetDataString();
		}
		break;
		case UIParam::UIPARAMTYPE::MODIFIER:
		{
			m_Size = m_Font->m_DrawSize;

			if (!s_ParamController.IsSelected(this))
			{
				text += GetDataString();
				m_Font->m_Color = { 1,1,1,1 };
			}
			else
			{
				text += m_Font->m_Text;
				m_Font->m_Color = { 1,0,0,1 };
			}
		}
		break;
		default:
			break;
		}

		m_Font->m_Text = text;
	}

	auto childParam = GetComponents<UIParam>();
	pos.x += 25;
	pos.y += m_Font->m_FontHeight + m_ParamInterval;

	float mustX = 0;
	for (auto& it : childParam)
	{
		it->SetPos(pos);
		pos.y += m_Font->m_FontHeight + m_ParamInterval;

		if (mustX < it->m_Font->m_DrawSize.x)
		{
			mustX = it->m_Font->m_DrawSize.x;
		}
	}

	m_Size.x = 25 + mustX < m_Font->m_DrawSize.x ? m_Font->m_DrawSize.x : 25 + mustX;
	m_Size.y = pos.y - topY + m_Font->m_FontHeight;
}

int UIParam::GetPixelFuncID()
{
	return m_Font->GetFontRenderID();
}

std::wstring UIParam::GetDataString()
{
	std::wstring result;

	switch (m_ControlType)
	{
	case UIParam::UICONTROLTYPE::ORIGIN_DATA:
	{
		switch (m_DataType)
		{
		case CGH::DATA_TYPE::TYPE_BOOL:
			result = *(reinterpret_cast<bool*>(m_ParamPtr)) ? L"true" : L"false";
			break;
		case CGH::DATA_TYPE::TYPE_FLOAT:
			result = GetStringFromValue<float>();
			break;
		case CGH::DATA_TYPE::TYPE_INT:
			result = GetStringFromValue<int>();
			break;
		case CGH::DATA_TYPE::TYPE_UINT:
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

void UIParam::ParamController::Update(float delta)
{
	if (m_NextTimeClose)
	{
		WorkClear();
		WorkEnd();
	}
	else if (m_CurrParam)
	{
		m_NextTimeClose = GETAPP->IsMouseButtonClickedAndNotThisObject(DirectX::MOUSEBUTTONINDEX::LEFTBUTTON, 
			m_CurrParam->GetPixelFuncID());

		auto& keyboard = GETKEY;

		if (keyboard.IsKeyPressed(KEYState::Enter))
		{
			Excute();
			return;
		}
		else if (keyboard.IsKeyPressed(KEYState::Back))
		{
			if (m_InputData.size())
			{
				m_InputData.pop_back();
			}
		}
		else if (keyboard.IsKeyPressed(KEYState::OemMinus))
		{
			if (m_InputData.size() == 0)
			{
				m_InputData += '-';
			}
		}
		else if (keyboard.IsKeyPressed(KEYState::OemPeriod))
		{
			m_InputData += '.';
		}
		else
		{
			for (auto key = KEYState::D0; key <= KEYState::D9; key = static_cast<KEYState>(key + 1))
			{
				if (keyboard.IsKeyPressed(key))
				{
					m_InputData += ('0' + key - KEYState::D0);
					break;
				}
			}
		}

		m_CurrParam->m_Font->m_Text.clear();
		m_CurrParam->m_Font->m_Text.insert(m_CurrParam->m_Font->m_Text.end(), m_InputData.begin(), m_InputData.end());
	}
}

void UIParam::ParamController::WorkClear()
{
	m_InputData.clear();
	m_NextTimeClose = false;

	if (m_CurrParam)
	{
		m_CurrParam = nullptr;
	}

	if (m_EnumSelectPanel)
	{
		m_EnumSelectPanel->UIOff();
	}
}

void UIParam::ParamController::Excute()
{
	if (m_InputData.size())
	{
		switch (m_CurrParam->m_DataType)
		{
		case CGH::DATA_TYPE::TYPE_BOOL:
			*reinterpret_cast<bool*>(m_CurrParam->m_ParamPtr) = atoi(m_InputData.c_str());
			break;
		case CGH::DATA_TYPE::TYPE_FLOAT:
			*reinterpret_cast<float*>(m_CurrParam->m_ParamPtr) = static_cast<float>(atof(m_InputData.c_str()));
			break;
		case CGH::DATA_TYPE::TYPE_INT:
			*reinterpret_cast<int*>(m_CurrParam->m_ParamPtr) = atoi(m_InputData.c_str());
			break;
		case CGH::DATA_TYPE::TYPE_UINT:
			*reinterpret_cast<UINT*>(m_CurrParam->m_ParamPtr) = atoi(m_InputData.c_str());
			break;
		default:
			break;
		}

		if (m_CurrParam->m_DirtyCall)
		{
			m_CurrParam->m_DirtyCall();
		}
	}

	WorkClear();
	WorkEnd();
}

void UIParam::ParamController::SetUIParam(UIParam* uiParam)
{
	WorkStart();
	WorkClear();

	m_CurrParam = uiParam;

	if (m_CurrParam->m_ParamPtr)
	{
		switch (m_CurrParam->m_ControlType)
		{
		case UIParam::UICONTROLTYPE::ENUM_DATA:
		case UIParam::UICONTROLTYPE::STRING_DATA:
			CreateSubPanel(m_CurrParam);
			break;
		}
	}
	else
	{
		m_CurrParam = nullptr;
	}
}


void UIParam::ParamController::CreateSubPanel(UIParam* param)
{
	if (m_EnumSelectPanel == nullptr)
	{
		m_EnumSelectPanel = param->GetScene().CreateGameObject<UIPanel>();
	}

	m_EnumSelectPanel->DeleteAllComs();
	m_EnumSelectPanel->UIOn();
	m_EnumSelectPanel->SetPos(GETAPP->GetMousePos());
	m_EnumSelectPanel->GetComponent<DOTransform>()->SetPosZ(0.1f);
	m_EnumSelectPanel->SetSize({});

	switch (param->m_ControlType)
	{
	case UIParam::UICONTROLTYPE::ENUM_DATA:
	{
		for (auto& it : *param->m_EnumElementInfo)
		{
			auto button = m_EnumSelectPanel->CreateComponenet<UIButton>(true);
			button->SetText(it.elementName);
			button->SetTextHeight(10);
			button->OnlyFontMode();
			button->AddFunc(std::bind(&UIParam::ParamController::SetEnumData, this, it.value),
				DirectX::Mouse::ButtonStateTracker::ButtonState::RELEASED);

			if (param->m_DirtyCall)
			{
				button->AddFunc(param->m_DirtyCall, DirectX::Mouse::ButtonStateTracker::ButtonState::RELEASED);
			}

			m_EnumSelectPanel->AddUICom(button);
		}
	}
	break;
	case UIParam::UICONTROLTYPE::STRING_DATA:
	{
		for (auto& it : *param->m_Strings)
		{
			std::wstring temp;
			temp.insert(temp.end(), it.begin(), it.end());
			auto button = m_EnumSelectPanel->CreateComponenet<UIButton>(true);
			button->SetText(temp);
			button->SetTextHeight(15);
			button->OnlyFontMode();
			button->AddFunc(std::bind(&UIParam::ParamController::SetStringData, this, it),
				DirectX::Mouse::ButtonStateTracker::ButtonState::RELEASED);

			if (param->m_DirtyCall)
			{
				button->AddFunc(param->m_DirtyCall, DirectX::Mouse::ButtonStateTracker::ButtonState::RELEASED);
			}

			m_EnumSelectPanel->AddUICom(button);
		}
	}
	break;
	}
}

void UIParam::ParamController::SetEnumData(int value)
{
	*reinterpret_cast<int*>(m_CurrParam->m_ParamPtr) = value;
	WorkClear();
}

void UIParam::ParamController::SetStringData(const std::string& str)
{
	*reinterpret_cast<std::string*>(m_CurrParam->m_ParamPtr) = str;
	WorkClear();
}
