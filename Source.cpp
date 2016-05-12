#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "wininet")

#include <windows.h>
#include <wininet.h>
#include <stdio.h>

TCHAR szClassName[] = TEXT("Window");

// 1601/1/1ÇÊÇËëOÇÃì˙ïtÇÃèÍçáÇÕé∏îsÇ∑ÇÈ
BOOL GetUnixTime(WORD nYear, WORD nMonth, WORD nDay, PINT64 pUnixTime)
{
	const SYSTEMTIME systemtime = { nYear, nMonth, 0, nDay, 0,0,0,0 };
	FILETIME filetime;
	if (SystemTimeToFileTime(&systemtime, &filetime))
	{
		*pUnixTime = filetime.dwHighDateTime;
		*pUnixTime <<= 32;
		*pUnixTime += filetime.dwLowDateTime;
		*pUnixTime -= 116444736000000000L;
		*pUnixTime /= 10000000L;
		return TRUE;
	}
	else
	{
		*pUnixTime = 0;
		return FALSE;
	}
}

SYSTEMTIME StringToSystemTime(LPCTSTR lpszString)
{
	SYSTEMTIME systemtime = { 0 };
	swscanf_s(lpszString, TEXT("%hd/%hd/%hd"),
		&systemtime.wYear,
		&systemtime.wMonth,
		&systemtime.wDay);
	return systemtime;
}

LPTSTR SystemTimeToString(const LPSYSTEMTIME lpSystemTime)
{
	LPTSTR lpszString = (LPTSTR)GlobalAlloc(0, sizeof(TCHAR)*(4 + 1 + 2 + 1 + 2 + 1));
	wsprintf(lpszString, TEXT("%04d/%02d/%02d"), lpSystemTime->wYear, lpSystemTime->wMonth, lpSystemTime->wDay);
	return lpszString;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit1;
	static HWND hEdit2;
	static HWND hButton;
	switch (msg)
	{
	case WM_CREATE:
		{
			SYSTEMTIME systemtime;
			GetSystemTime(&systemtime);
			LPTSTR p = SystemTimeToString(&systemtime);
			hEdit1 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), p, WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
			GlobalFree(p);
		}
		hEdit2 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL | ES_READONLY, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("éÊìæ"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		PostMessage(hWnd, WM_COMMAND, IDOK, 0);
		break;
	case WM_SIZE:
		MoveWindow(hEdit1, 10, 10, 256, 32, TRUE);
		MoveWindow(hEdit2, 10, 50, 256, 32, TRUE);
		MoveWindow(hButton, 10, 90, 256, 32, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			TCHAR szText[256];
			SYSTEMTIME systemtime;
			GetWindowText(hEdit1, szText, 256);
			systemtime = StringToSystemTime(szText);
			INT64 nUnixTime;
			if (GetUnixTime(systemtime.wYear, systemtime.wMonth, systemtime.wDay, &nUnixTime))
			{
				wsprintf(szText, TEXT("%I64d"), nUnixTime);
			}
			else
			{
				lstrcpy(szText, TEXT("ïœä∑Ç…é∏îsÇµÇ‹ÇµÇΩ"));
			}
			SetWindowText(hEdit2, szText);
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefDlgProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		DLGWINDOWEXTRA,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		0,
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("ì¸óÕÇ≥ÇÍÇΩì˙ïtÇUnixéûä‘Ç…ïœä∑Ç∑ÇÈ"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		if (!IsDialogMessage(hWnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}
