#include "AutoRun.h"

BOOL Reg_CurrentUser(LPCWSTR lpszFileName, LPCWSTR lpszValueName)
{
	HKEY hKey;
	printf("[+] length: %d\n",::lstrlen(lpszFileName));
	if (ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_CURRENT_USER,L"software\\Microsoft\\Windows\\CurrentVersion\\Run",0,KEY_WRITE,&hKey))
	{
		printf("[-] Open Register Error,Code: %d\n", GetLastError());
		return FALSE;
	}
	if (ERROR_SUCCESS != ::RegSetValueEx(hKey, lpszValueName, 0, REG_SZ, (BYTE*)lpszFileName, (2 * ::lstrlen(lpszFileName))))
	{
		::RegCloseKey(hKey);
		printf("[-] Register Set Value Error,Code:%d", GetLastError());
		return FALSE;
	}
	::RegCloseKey(hKey);
	return TRUE;
}

void QueryRegistry()
{
	HKEY hKey;
	const wchar_t* subKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	if (RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		wchar_t  valueName[256];
		DWORD valueNameSize = sizeof(valueName) / sizeof(valueName[0]);
		BYTE data[256];
		DWORD dataSize = sizeof(data);
		DWORD index = 0;

		while (RegEnumValue(hKey, index, valueName, &valueNameSize, NULL, NULL, data, &dataSize) == ERROR_SUCCESS)
		{
			char valueNameMb[256];
			WideCharToMultiByte(CP_UTF8, 0, valueName, -1, valueNameMb, sizeof(valueNameMb), NULL, NULL);

			
			char dataMb[256];
			WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)data, -1, dataMb, sizeof(dataMb), NULL, NULL);

			printf("[+] Value Name: %s, Data: ", valueNameMb);
			if (dataSize > 0)
			{
				char dataMb[256];
				WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)data, dataSize / sizeof(wchar_t), dataMb, sizeof(dataMb), NULL, NULL);
				dataMb[sizeof(dataMb) - 1] = '\0'; 

				printf("%s\n", dataMb);
			}
			else {
				printf("null\n");
			}

			valueNameSize = sizeof(valueName) / sizeof(valueName[0]);
			dataSize = sizeof(data);
			index++;
		}

		RegCloseKey(hKey);
	}
	else
	{
		printf("[-] Failed to open registry key.\n");
	}
}


BOOL AutoRun_Startup(char* lpszSrcFilePath, char* lpszDestFileName)
{
	BOOL bRet = FALSE;
	char szStartupPath[MAX_PATH] = { 0 };
	char szDestFilePath[MAX_PATH] = { 0 };

	bRet = ::SHGetSpecialFolderPath(NULL, (LPWSTR)szStartupPath, CSIDL_STARTUP, TRUE);
	printf("[+] Startup folder path is : %s\n", szStartupPath);
	if (FALSE == bRet)
	{
		return FALSE;
	}
	::wsprintf((LPWSTR)szDestFilePath, (LPCWSTR)"%s\\%s", szStartupPath, lpszDestFileName);
	bRet = ::CopyFile((LPCWSTR)lpszSrcFilePath, (LPCWSTR)szDestFilePath, FALSE);
	if (FALSE == bRet)
	{
		return FALSE;
	}
	return TRUE;

}

BOOL ListStartupFiles() {
	wchar_t startupPath[MAX_PATH];
	if (SHGetFolderPathW(NULL, CSIDL_STARTUP, NULL, 0, startupPath) != S_OK) {
		wprintf(L"[-] Failed to get startup directory path.\n");
		return FALSE;
	}
	wprintf(L"[+] Startup Path: %s\n", startupPath);

	wchar_t searchPath[MAX_PATH];
	swprintf(searchPath, MAX_PATH, L"%s\\*.*", startupPath);
	wprintf(L"[+] Search Path: %s\n", searchPath);

	WIN32_FIND_DATAW findFileData;
	HANDLE hFind = FindFirstFileW(searchPath, &findFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		wprintf(L"[-] Failed to list files in startup directory. Error: %lu\n", GetLastError());
		return FALSE;
	}

	do {
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			continue;
		}
		wprintf(L"[+] File: %s\n", findFileData.cFileName);
	} while (FindNextFileW(hFind, &findFileData) != 0);

	FindClose(hFind);
	return TRUE;
}