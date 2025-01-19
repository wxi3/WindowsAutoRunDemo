#ifndef _MY_TASK_SCHEDULT_H_
#define _MY_TASK_SCHEDULT_H_


#include <Atlbase.h>
#include <comdef.h>
#include <taskschd.h>
#pragma comment(lib, "taskschd.lib")

class CMyTaskSchedule
{
private:
	ITaskService* m_lpITS;
	ITaskFolder* m_lpRootFolder;


public:
	CMyTaskSchedule(void);
	~CMyTaskSchedule(void);

public:
	BOOL Delete(char* lpszTaskName);
	BOOL DeleteFolder(char* lpszFolderName);

	BOOL NewTask(char* lpszTaskName, char* lpszProgramPath, char* lpszParameters, const char* lpszAuthor = "");

	BOOL IsExist(char* lpszTaskName);

	BOOL IsTaskValid(char* lpszTaskName);

	BOOL Run(char* lpszTaskName, char* lpszParam);

	BOOL IsEnable(char* lpszTaskName);

	BOOL SetEnable(char* lpszTaskNAme, BOOL bEnable);
};

#endif
