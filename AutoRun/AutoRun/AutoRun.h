#include <Windows.h>
#include <stdio.h>
#include <ShlObj.h>

BOOL Reg_CurrentUser(LPCWSTR lpszFileName, LPCWSTR lpszValueName);
void QueryRegistry();
BOOL AutoRun_Startup(char* lpszSrcFilePath, char* lpszDestFileName);
BOOL ListStartupFiles();


