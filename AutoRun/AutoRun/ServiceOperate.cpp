#include "ServiceOperate.h"

BOOL SystemServiceOperate(char* lpszDriverPath) {
    BOOL bRet = TRUE;
    WCHAR szName[MAX_PATH] = { 0 };
    WCHAR wDriverPath[MAX_PATH] = { 0 };

    MultiByteToWideChar(CP_ACP, 0, lpszDriverPath, -1, szName, MAX_PATH);
    ::PathStripPath(szName);
    wprintf(L"[+] Service name is:%s.\n", szName);

    MultiByteToWideChar(CP_ACP, 0, lpszDriverPath, -1, wDriverPath, MAX_PATH);
    SC_HANDLE shOSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!shOSCM) { 
        printf("[-] Failed to open Service Control Manager. Error code: %lu\n", GetLastError());
        return FALSE;
    }

    SC_HANDLE shCS = ::CreateService(shOSCM, szName, szName,
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
        SERVICE_DEMAND_START,
        SERVICE_ERROR_NORMAL,
        wDriverPath, NULL, NULL, NULL, NULL,NULL);
    if (!shCS) {
        printf("[-] Failed to create service. Error code: %lu\n", GetLastError());
        ::CloseServiceHandle(shOSCM);
        return FALSE;
    }

    if (!::StartService(shCS, 0, NULL)) {
        printf("[-] Failed to start service. Error code: %lu\n", GetLastError());
        bRet = FALSE;
    }

    ::CloseServiceHandle(shCS);
    ::CloseServiceHandle(shOSCM);
    return bRet;
}
