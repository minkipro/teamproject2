#include "stdafx.h"
#include "HCKoreanInput.h"

HCKoreanInput::HCKoreanInput()
{
}

char Text[255];
char Cstr[10];

LRESULT HCKoreanInput::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	
	switch (uMsg)
	{
	case WM_IME_COMPOSITION:
		int len;
		HIMC m_hIMC = ImmGetContext(hwnd);
		if (lParam & GCS_RESULTSTR)
		{
			if ((len = ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, NULL, 0)) > 0)
			{
				ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, Cstr, len);
				Cstr[len] = 0;
				strcpy(Text + strlen(Text), Cstr);
				memset(Cstr, 0, 10);
				{
					char szTemp[256] = "";
					sprintf(szTemp, "완성된 글자 : %s\r\n", Text);
				}
			}
		}
		else if (lParam & GCS_COMPSTR)
		{
			len = ImmGetCompositionString(m_hIMC, GCS_COMPSTR, NULL, 0);
			ImmGetCompositionString(m_hIMC, GCS_COMPSTR, Cstr, len);
			Cstr[len] = 0;

			{
				char szTemp[256] = "";
				sprintf(szTemp, "조합중인 글자 : %s\r\n", Cstr);
			}
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
	return 1;
}