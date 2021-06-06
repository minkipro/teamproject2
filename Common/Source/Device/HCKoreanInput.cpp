#include "stdafx.h"
#include "HCKoreanInput.h"
#include <tchar.h>

HCKoreanInput::HCKoreanInput()
{
}

void HCKoreanInput::Init()
{
}

void HCKoreanInput::Update()
{
}



LRESULT HCKoreanInput::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HIMC m_hIMC = ImmGetContext(hwnd);
	int len;
	switch (uMsg)
	{
	case WM_IME_COMPOSITION:

		if (lParam & GCS_RESULTSTR)//전부 조합됐으면
		{
			if ((len = ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, NULL, 0)) > 0)
			{
				ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, m_cstr, len);
				m_cstr[len] = 0;
				size_t len = wcslen(m_text);
				wcscpy_s(m_text + len, 255- len, m_cstr);
				memset(m_cstr, 0, 10);
			}
		}
		else if (lParam & GCS_COMPSTR)//조합중
		{
			len = ImmGetCompositionString(m_hIMC, GCS_COMPSTR, NULL, 0);
			ImmGetCompositionString(m_hIMC, GCS_COMPSTR, m_cstr, len);
			m_cstr[len] = 0;
		}
		ImmReleaseContext(hwnd, m_hIMC);
		return 0;
	case WM_CHAR:
		return 0;
	case WM_IME_NOTIFY:
		return 0;
	case WM_KEYDOWN:
		return 0;
	}

	return 0;
}