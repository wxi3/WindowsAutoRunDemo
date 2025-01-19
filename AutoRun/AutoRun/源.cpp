#include "AutoRun.h"
#include "TaskSchedule.h"
#include "ServiceOperate.h"
#include <iostream>


int main(int argc, CHAR* argv[])
{
	if (argc < 3)
	{
		printf("[*] Usage: %s <type> <Name> [FilePath]\n", argv[0]);
		printf("[*] Available types:\n");
		printf("  Registry  <Name> <FilePath> : Modify the registry for the specified name with the provided file path.\n");
		printf("  TaskSchedule <Name> <FilePath> : Create a scheduled task with the specified name and file path.\n");
		printf("  Service <FilePath> : Create and start a system service with the file path.\n");
		printf("\n");
		printf("[*] Examples:\n");
		printf("  %s Registry MyRegName C:\\Path\\To\\File.exe\n", argv[0]);
		printf("  %s TaskSchedule MyTask C:\\Path\\To\\Task.exe\n", argv[0]);
		printf("  %s Service C:\\Path\\To\\Service.exe\n\n", argv[0]);
		printf("\n");
		printf("[*] Getting Registry.\n");
		QueryRegistry();
		printf("[*] Getting Startup Files.\n");
		ListStartupFiles();
		return 1;
	}
	char *type = argv[1];
	printf("[*] Type : %s\n", type);
	if (strcmp(type, "Registry") == 0)
	{
		if (argc < 4) 
		{
			printf("[-] Registry operation requires three parameters.\n");
			return 1;
		}
		wchar_t wFilePath[MAX_PATH];
		wchar_t wRegistryName[50];
		size_t convertedChars = 0;
		mbstowcs_s(&convertedChars, wRegistryName, argv[2], _TRUNCATE);
		mbstowcs_s(&convertedChars, wFilePath, argv[3], _TRUNCATE);
		printf("[+] FilePath: %ls\n", wFilePath);
		printf("[+] RegistryName: %ls\n", wRegistryName);

		if (FALSE == Reg_CurrentUser(wFilePath, wRegistryName))
		{
			printf("[-] Register modify failed.\n");
		}
		printf("[+] Reg OK.\n");
		QueryRegistry();
		system("pause");
		
	}
	if (strcmp(type,"TaskSchedule") == 0)
	{
		if (argc < 4) 
		{
			printf("[-] Registry operation requires three parameters.\n");
			return 1;
		}
		if (!IsUserAnAdmin())
		{
			printf("[*] Use TaskSchedule need Administrator privilege.\n");
			char input;
			printf("[*] Do you want to list scheduled tasks? (y/n): ");
			scanf(" %c", input);
			if (input == 'y' || input == 'Y') {
				system("schtasks /query /fo LIST /v");
			}
			else {
				printf("[*] Operation cancelled.\n");
			}
			return 1;
		}
		CMyTaskSchedule task;
		BOOL bRet = FALSE;

		bRet = task.NewTask(argv[2], argv[3], (char *)"", "");
		if (FALSE == bRet)
		{
			printf("[-] Create Task Schedule Error.\n");
			return 1;
		}
		printf("[+] Task Schedule created Successfully.\n");
		system("pause");
	}
	if (strcmp(type,"Service") == 0 )
	{
		BOOL bRet = FALSE;
		if (!IsUserAnAdmin())
		{
			printf("[*] Creating a system service requires Administrator privilege.\n");
			char input;
			printf("[*] Do you want to list system services? (y/n): ");
			scanf(" %c", input);
			if (input == 'y' || input == 'Y') {
				system("sc query");
			}
			else {
				printf("[*] Operation cancelled.\n");
			}
			return 1;
		}
		printf("[*] Starting to Create system service.....\n");
		char* szFileName = argv[2];
		bRet = SystemServiceOperate(szFileName);
		if (FALSE == bRet)
		{
			printf("[-] Create service error.\n");
			return 1;
		}
		printf("[+] Service created and started successfully.\n");
	}
	return 0;
}