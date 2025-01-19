#ifndef _SERVICE_OPERATE_H_
#define _SERVICE_OPERATE_H_


#include <Windows.h>
#include <Shlwapi.h>
#include <stdio.h>
#pragma comment(lib, "Shlwapi.lib")



BOOL SystemServiceOperate(char* lpszDriverPath);


#endif