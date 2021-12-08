// wmc.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "wmc.h"

#define MSG_WMC_MAINAPP	(WM_USER+100)
#define MYWM_NOTIFYICON		(WM_USER+101)


#pragma comment( lib, "psapi" )

#include <windows.h>
#include <Psapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <shellapi.h>

#define MAX_LOADSTRING 100
// Use to convert bytes to KB
#define DIV 1024

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND g_hWndWMC;
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

SIZE_T g_stTotal = 0;
BOOL g_bPriv = FALSE;

HANDLE g_hThread = NULL;
DWORD g_ThreadId = -1;
BOOL g_IsThreadRunning = FALSE;
BOOL g_IsMonitoring = FALSE;
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    WMCProc(HWND, UINT, WPARAM, LPARAM);
SIZE_T FreeMemoryRutine(HWND hwnd);
VOID UpdateMemoryInfo(HWND hwnd);

BOOL MysetAllPrivileges();
BOOL MySetTokenPrivileges(wchar_t *strValue);

DWORD WINAPI MemoryWatchProc(LPVOID lpParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WMC, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WMC));

    MSG msg;

	g_hWndWMC = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_WMC), 0, WMCProc, 0);
	ShowWindow(g_hWndWMC, nCmdShow);

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WMC));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WMC);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, SW_HIDE);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
	
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
		case IDOK:
		case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
		case IDC_STATIC_WEB:
			ShellExecute(0, 0, L"https://windowsmemorycleaner.com/", 0, 0, SW_SHOW);
			break;
		case IDC_BUTTON1:
			ShellExecute(0, 0, L"https://windowsmemorycleaner.com/change-log/", 0, 0, SW_SHOW);
			break;
		
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK WMCProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	SIZE_T stTmp = 0;
	SYSTEM_INFO si = { 0 };
	CHAR cActivate[MAX_PATH] = { 0 };
	char cCurrTime[MAX_PATH] = { 0 };
	SYSTEMTIME st;
	int err;

	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg, IDC_EDIT1, 80, TRUE);
		g_bPriv = MysetAllPrivileges();
		
		if(g_bPriv == FALSE)
		{
			MessageBox(NULL,TEXT("Privilage test failed, please run as admin!"), TEXT("Neede Privilage"),MB_OK);
			PostQuitMessage(0);
		}

		GetSystemInfo(&si);
		
		UpdateMemoryInfo(hDlg);
		g_IsThreadRunning = TRUE;
		g_hThread = CreateThread(NULL, 0, MemoryWatchProc, (LPVOID)hDlg, 0, &g_ThreadId);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			g_IsThreadRunning = FALSE;
			CloseHandle(g_hThread);
			g_ThreadId = -1;
			EndDialog(hDlg, LOWORD(wParam));
			PostQuitMessage(0);
			return (INT_PTR)TRUE;
		case IDC_BUTTON2: // About
		case ID_WMC_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hDlg, About);
			break;
		case ID_WMC_RESTORE:
			ShowWindow(hDlg, SW_SHOW);
			break;
		case IDC_BUTTON1: // clean mem
		case ID_WMC_CLEAN:
			stTmp = FreeMemoryRutine(hDlg) / 1000000;
			SetDlgItemInt(hDlg,IDC_EDIT9, stTmp,FALSE);
			
			g_stTotal = g_stTotal + stTmp;
			SetDlgItemInt(hDlg, IDC_EDIT10, g_stTotal, FALSE);
			
			GetSystemTime(&st);
			sprintf_s(cCurrTime, MAX_PATH, "%04d-%02d-%02d %02d:%02d:%02d ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			SetDlgItemTextA(hDlg, IDC_EDIT8, cCurrTime);
			UpdateMemoryInfo(hDlg);
			break;
		case IDC_BUTTON3: // Activate
			GetDlgItemTextA(hDlg, IDC_BUTTON3, cActivate, MAX_PATH);
			if (strcmp(cActivate, "Activate") == 0)
			{	
				g_IsMonitoring = TRUE;
				SetDlgItemTextA(hDlg, IDC_BUTTON3, "DeActivate");
			}
			else
			{
				g_IsMonitoring = FALSE;
				SetDlgItemTextA(hDlg, IDC_BUTTON3, "Activate");
			}
			break;
		case IDC_BUTTON4: // Open Task Manager
			ShellExecute(0, 0, L"taskmgr.exe", 0, 0, SW_SHOW);
			break;
			break;
		case IDC_BUTTON5: // Tray icon
			NOTIFYICONDATA tnid;

			tnid.cbSize = sizeof(NOTIFYICONDATA);
			tnid.hWnd = hDlg;
			tnid.uID = MSG_WMC_MAINAPP;
			tnid.uVersion = NOTIFYICON_VERSION_4;
			tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
			tnid.uCallbackMessage = MYWM_NOTIFYICON;
			//tnid.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));//wndclass.hIcon; 
			tnid.hIcon = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_WMC), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
			_stprintf_s(tnid.szTip,128, _T("Windows Memory Cleaner"));
			err = Shell_NotifyIcon(NIM_ADD, &tnid);
			err = GetLastError();
			ShowWindow(hDlg, SW_HIDE);
			UpdateWindow(hDlg);
			break;
		
		}
		break;
	case MYWM_NOTIFYICON:
	{
		switch (lParam)
		{
		case WM_RBUTTONDOWN:
		{

			HMENU menu = CreatePopupMenu();
			AppendMenu(menu, MF_DISABLED | MF_STRING, 0, L"WMC Menu");
			AppendMenu(menu, MF_SEPARATOR, 0, NULL);
			AppendMenu(menu, MF_STRING, ID_WMC_CLEAN, L"Clean");
			AppendMenu(menu, MF_STRING, ID_WMC_RESTORE, L"Restore");
			AppendMenu(menu, MF_STRING, ID_WMC_ABOUT, L"About");


			POINT p;
			GetCursorPos(&p);
			TrackPopupMenu(menu, TPM_CENTERALIGN | TPM_LEFTBUTTON, p.x, p.y, 0, hDlg, NULL);
			DestroyMenu(menu);
			break;
		}

		}
	}
	break;

	}
	return (INT_PTR)FALSE;
}

SIZE_T FreeMemoryRutine(HWND hwnd)
{
	SIZE_T empty = 0;
	SIZE_T ret = 0;

	for (DWORD pid = 0x0;pid < 0x4E1C;pid = pid + sizeof(DWORD))
	{
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		if (NULL == hProcess)
			hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_SET_QUOTA | PROCESS_VM_READ, FALSE, pid);
		
		if (NULL != hProcess)
		{
			
			PROCESS_MEMORY_COUNTERS pmc = { 0 };
			pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS) * sizeof(BYTE);
			
			GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc));
			empty = pmc.WorkingSetSize;
			if (EmptyWorkingSet(hProcess) == TRUE)
			{
				ZeroMemory(&pmc, sizeof(PROCESS_MEMORY_COUNTERS) * sizeof(BYTE));
				GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc));
				empty = empty - pmc.WorkingSetSize;
				ret = ret + empty;
			}
		}

	}

	return ret;
}

BOOL MySetTokenPrivileges(wchar_t *strValue)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	int err;

	if (0 == OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
	{
		err = GetLastError();
		//wprintf(L"OpenProcessToken error for %d -> %d\n", strValue, err);
		return FALSE;
	}



	if (0 == LookupPrivilegeValue(NULL, strValue, &tkp.Privileges[0].Luid))
	{
		err = GetLastError();
//		wprintf(L"LookupPrivilegeValue error for %d -> %d", strValue, err);
		return FALSE;
	}

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (0 == AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0))
	{
		err = GetLastError();
	//	wprintf(L"AdjustTokenPrivileges error for %d -> %d", strValue, err);

		return FALSE;
	}

	if (ERROR_SUCCESS == GetLastError())
		return TRUE;

	return FALSE;

}

BOOL MysetAllPrivileges()
{
	BOOL bRet = TRUE;

	//if (FALSE == MySetTokenPrivileges(SE_CREATE_TOKEN_NAME)) { printf("1");  bRet = FALSE; }
	//if (FALSE == MySetTokenPrivileges(SE_ASSIGNPRIMARYTOKEN_NAME)) { printf("2"); bRet = FALSE; }
	//if (FALSE == MySetTokenPrivileges(SE_LOCK_MEMORY_NAME)) { printf("3"); bRet = FALSE; }
	if (FALSE == MySetTokenPrivileges(SE_INCREASE_QUOTA_NAME)) { printf("4"); bRet = FALSE; }
	//if (FALSE == MySetTokenPrivileges(SE_MACHINE_ACCOUNT_NAME)) { printf("5"); bRet = FALSE; }
	//if (FALSE == MySetTokenPrivileges(SE_TCB_NAME)) { printf("6"); bRet = FALSE; }
	if (FALSE == MySetTokenPrivileges(SE_SECURITY_NAME)) { printf("7"); bRet = FALSE; }
	if (FALSE == MySetTokenPrivileges(SE_TAKE_OWNERSHIP_NAME)) { printf("8"); bRet = FALSE; }
	if (FALSE == MySetTokenPrivileges(SE_LOAD_DRIVER_NAME)) { printf("9"); bRet = FALSE; }
	if (FALSE == MySetTokenPrivileges(SE_SYSTEM_PROFILE_NAME)) { printf("10"); bRet = FALSE; }
	if (FALSE == MySetTokenPrivileges(SE_SYSTEMTIME_NAME)) { printf("11"); bRet = FALSE; }
	if (FALSE == MySetTokenPrivileges(SE_PROF_SINGLE_PROCESS_NAME)) { printf("12"); bRet = FALSE; }
	if (FALSE == MySetTokenPrivileges(SE_INC_BASE_PRIORITY_NAME)) { printf("13"); bRet = FALSE; }
	if (FALSE == MySetTokenPrivileges(SE_CREATE_PAGEFILE_NAME)) { printf("14"); bRet = FALSE; }
	//if (FALSE == MySetTokenPrivileges(SE_CREATE_PERMANENT_NAME)){ printf("15"); bRet = FALSE;}
	if (FALSE == MySetTokenPrivileges(SE_BACKUP_NAME)) { printf("16"); bRet = FALSE; }
	if (FALSE == MySetTokenPrivileges(SE_RESTORE_NAME)) { printf("17"); bRet = FALSE; }
	if (FALSE == MySetTokenPrivileges(SE_SHUTDOWN_NAME)) { printf("18"); bRet = FALSE; }
	if (FALSE == MySetTokenPrivileges(SE_DEBUG_NAME)) { printf("19"); bRet = FALSE; }
	//if (FALSE == MySetTokenPrivileges(SE_AUDIT_NAME)) { printf("20"); bRet = FALSE;}
	if (FALSE == MySetTokenPrivileges(SE_SYSTEM_ENVIRONMENT_NAME)) { printf("21"); bRet = FALSE; }
	if (FALSE == MySetTokenPrivileges(SE_CHANGE_NOTIFY_NAME)) { printf("22"); bRet = FALSE; }
	if (FALSE == MySetTokenPrivileges(SE_REMOTE_SHUTDOWN_NAME)) { printf("23"); bRet = FALSE; }
	if (FALSE == MySetTokenPrivileges(SE_UNDOCK_NAME)) { printf("24"); bRet = FALSE; }
	//if (FALSE == MySetTokenPrivileges(SE_SYNC_AGENT_NAME)) { printf("25"); bRet = FALSE;}
	//if (FALSE == MySetTokenPrivileges(SE_ENABLE_DELEGATION_NAME)) { printf("26"); bRet = FALSE;}
	if (FALSE == MySetTokenPrivileges(SE_MANAGE_VOLUME_NAME)) { printf("27"); bRet = FALSE; }

	return bRet;
}

VOID UpdateMemoryInfo(HWND hwnd)
{
	MEMORYSTATUSEX statex = { 0 };
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	SetDlgItemInt(hwnd, IDC_EDIT2, statex.ullTotalPhys / DIV, TRUE);
	SetDlgItemInt(hwnd, IDC_EDIT3, statex.ullAvailPhys / DIV, TRUE);
	SetDlgItemInt(hwnd, IDC_EDIT4, statex.ullTotalPageFile / DIV, TRUE);
	SetDlgItemInt(hwnd, IDC_EDIT5, statex.ullAvailPageFile / DIV, TRUE);
	SetDlgItemInt(hwnd, IDC_EDIT6, statex.ullTotalVirtual / DIV, TRUE);
	SetDlgItemInt(hwnd, IDC_EDIT7, statex.ullAvailVirtual / DIV, TRUE);
	SetDlgItemInt(hwnd, IDC_EDIT11, statex.dwMemoryLoad, TRUE);
	SetDlgItemInt(hwnd, IDC_EDIT12, statex.ullAvailExtendedVirtual / DIV, TRUE);
}

DWORD WINAPI MemoryWatchProc(LPVOID lpParam)
{
	char cCurrTime[MAX_PATH] = { 0 };
	SYSTEMTIME st;
	SIZE_T stTmp = 0;
	HWND hDlg = (HWND)lpParam;
	do
	{
		MEMORYSTATUSEX statex = { 0 };
		statex.dwLength = sizeof(statex);
		GlobalMemoryStatusEx(&statex);

		if (g_IsMonitoring)
		{
			if (statex.dwMemoryLoad > GetDlgItemInt(hDlg, IDC_EDIT1, NULL, TRUE))
			{
				stTmp = FreeMemoryRutine(hDlg) / 1000000;
				SetDlgItemInt(hDlg, IDC_EDIT9, stTmp, FALSE);

				g_stTotal = g_stTotal + stTmp;
				SetDlgItemInt(hDlg, IDC_EDIT10, g_stTotal, FALSE);

				GetSystemTime(&st);
				sprintf_s(cCurrTime, MAX_PATH, "%04d-%02d-%02d %02d:%02d:%02d ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
				SetDlgItemTextA(hDlg, IDC_EDIT8, cCurrTime);


			}
		}
		//SetDlgItemInt(hDlg, IDC_EDIT11, statex.dwMemoryLoad, TRUE);
		UpdateMemoryInfo(hDlg);
		Sleep(2000);
	} while (g_IsThreadRunning);
	
	return 0;
}