// wmccli.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#pragma comment( lib, "psapi" )

#define WIDTH 7

#include <windows.h>
#include <Psapi.h>
#include <stdio.h>
#include <stdlib.h>

BOOL MysetAllPrivileges();
BOOL MySetTokenPrivileges(wchar_t *strValue);

int main()
{
	SIZE_T empty = 0;
	SIZE_T ret = 0;
	MEMORYSTATUSEX statex = { 0 };

	printf(" \n");
	printf(" wmccli - Windows Memory Cleaner \n");
	printf(" [Command line interface version]\n");
	printf(" Version 1.0.0.1\n");
	printf("\n");
	printf(" Copyright (c) by https://windowsmemorycleaner.com/\n"); 
	printf(" All rights reserved.\n");
	printf(" \n");

	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	_tprintf(TEXT("Befor -> Memory in use: %*ld %%\n"),WIDTH, statex.dwMemoryLoad);

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
	_tprintf(TEXT("Memory been free: %*ld MB\n"), WIDTH, ret/ 1000000);
	Sleep(2000);
	ZeroMemory(&statex, sizeof(statex));
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);

	_tprintf(TEXT("After -> Memory in use: %*ld %%\n"), WIDTH, statex.dwMemoryLoad);

    return 0;
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
