#include "TaskSchedule.h"

CMyTaskSchedule::CMyTaskSchedule(void)
{
	m_lpITS = NULL;
	m_lpRootFolder = NULL;

	HRESULT hr = ::CoInitialize(NULL);

	hr = ::CoCreateInstance(CLSID_TaskScheduler,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_ITaskService,
		(LPVOID*)(&m_lpITS));
	hr = m_lpITS->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
	hr = m_lpITS->GetFolder(_bstr_t("\\"), &m_lpRootFolder);

}

CMyTaskSchedule::~CMyTaskSchedule(void)
{
	if (m_lpITS)
	{
		m_lpITS->Release();
	}if (m_lpRootFolder)
	{
		m_lpRootFolder->Release();
	}
	::CoUninitialize();
}

BOOL CMyTaskSchedule::Delete(char* lpszTaskName)
{
	if (NULL == m_lpRootFolder)
	{
		return FALSE;

	}

	CComVariant varianTaskName(NULL);
	varianTaskName = lpszTaskName;
	HRESULT hr = m_lpRootFolder->DeleteTask(varianTaskName.bstrVal, 0);
	if (FAILED(hr))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CMyTaskSchedule::DeleteFolder(char* lpszFolderName)
{
	if (NULL == m_lpRootFolder)
	{
		return FALSE;
	}
	CComVariant variantFolderName(NULL);
	variantFolderName = lpszFolderName;
	HRESULT hr = m_lpRootFolder->DeleteFolder(variantFolderName.bstrVal, 0);
	if (FAILED(hr))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CMyTaskSchedule::NewTask(char* lpszTaskName, char* lpszProgramPath, char* lpszParameters, const char* lpszAuthor)
{
	if (NULL == m_lpRootFolder)
	{
		return FALSE;
	}
	Delete(lpszTaskName);

	ITaskDefinition* pTaskDefinition = NULL;
	HRESULT hr = m_lpITS->NewTask(0, &pTaskDefinition);

	IRegistrationInfo* pRegInfo = NULL;
	CComVariant variantAuthor(NULL);
	variantAuthor = lpszAuthor;
	hr = pTaskDefinition->get_RegistrationInfo(&pRegInfo);

	hr = pRegInfo->put_Author(variantAuthor.bstrVal);
	pRegInfo->Release();

	IPrincipal* pPrincipal = NULL;
	hr = pTaskDefinition->get_Principal(&pPrincipal);

	hr = pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);
	hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
	pPrincipal->Release();

	ITaskSettings* pSetting = NULL;
	hr = pTaskDefinition->get_Settings(&pSetting);

	hr = pSetting->put_StopIfGoingOnBatteries(VARIANT_FALSE);
	hr = pSetting->put_DisallowStartIfOnBatteries(VARIANT_FALSE);
	hr = pSetting->put_AllowDemandStart(VARIANT_TRUE);
	hr = pSetting->put_StartWhenAvailable(VARIANT_FALSE);
	hr = pSetting->put_MultipleInstances(TASK_INSTANCES_PARALLEL);
	pSetting->Release();

	IActionCollection* pActionCollect = NULL;
	hr = pTaskDefinition->get_Actions(&pActionCollect);
	IAction* pAction = NULL;
	hr = pActionCollect->Create(TASK_ACTION_EXEC, &pAction);
	pActionCollect->Release();

	CComVariant variantProgramPath(NULL);
	CComVariant variantParameters(NULL);
	IExecAction* pExecAction = NULL;
	hr = pAction->QueryInterface(IID_IExecAction, (PVOID*)(&pExecAction));
	pAction->Release();

	variantProgramPath = lpszProgramPath;
	variantParameters = lpszParameters;
	pExecAction->put_Path(variantProgramPath.bstrVal);
	pExecAction->put_Arguments(variantParameters.bstrVal);
	pExecAction->Release();

	ITriggerCollection* pTriggers = NULL;
	hr = pTaskDefinition->get_Triggers(&pTriggers);
	ITrigger* pTrigger = NULL;
	hr = pTriggers->Create(TASK_TRIGGER_LOGON, &pTrigger);

	IRegisteredTask* pRegisteredTask = NULL;
	CComVariant variantTaskName(NULL);
	variantTaskName = lpszTaskName;
	hr = m_lpRootFolder->RegisterTaskDefinition(variantTaskName.bstrVal,
		pTaskDefinition,
		TASK_CREATE_OR_UPDATE,
		_variant_t(),
		_variant_t(),
		TASK_LOGON_INTERACTIVE_TOKEN,
		_variant_t(""),
		&pRegisteredTask);

	pTaskDefinition->Release();
	pRegisteredTask->Release();

	return TRUE;
}

BOOL CMyTaskSchedule::IsExist(char* lpszTaskName)
{
	if (NULL == m_lpRootFolder)
	{
		return FALSE;
	}

	HRESULT hr = S_OK;
	CComVariant variantTaskName(NULL);
	CComVariant variantEnable(NULL);
	variantTaskName = lpszTaskName;
	IRegisteredTask* pRegisteredTask = NULL;
	
	hr = m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
	if (FAILED(hr) || (NULL == pRegisteredTask))
	{
		return FALSE;
	}
	pRegisteredTask->Release();
	return TRUE;

}


BOOL CMyTaskSchedule::IsTaskValid(char* lpszTaskName)
{
	if (NULL == m_lpRootFolder)
	{
		return FALSE;
	}
	HRESULT hr = S_OK;
	CComVariant variantTaskName(NULL);
	CComVariant variantEnable(NULL);
	variantTaskName = lpszTaskName;
	IRegisteredTask* pRegisteredTask = NULL;

	hr = m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
	if (FAILED(hr) || (NULL == pRegisteredTask))
	{
		return FALSE;
	}
	TASK_STATE taskState;
	hr = pRegisteredTask->get_State(&taskState);
	if (FAILED(hr))
	{
		pRegisteredTask->Release();
		return FALSE;
	}
	pRegisteredTask->Release();
	
	if (TASK_STATE_DISABLED == taskState)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CMyTaskSchedule::Run(char* lpszTaskName, char* lpszParam)
{
	if (NULL == m_lpRootFolder)
	{
		return FALSE;
	}
	HRESULT hr = S_OK;
	CComVariant variantTaskName(NULL);
	CComVariant variantParameters(NULL);
	variantTaskName = lpszTaskName;
	variantParameters = lpszParam;

	
	IRegisteredTask* pRegisteredTask = NULL;
	hr = m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
	if (FAILED(hr) || (NULL == pRegisteredTask))
	{
		return FALSE;
	}
	
	hr = pRegisteredTask->Run(variantParameters, NULL);
	if (FAILED(hr))
	{
		pRegisteredTask->Release();
		return FALSE;
	}
	pRegisteredTask->Release();

	return TRUE;
}


BOOL CMyTaskSchedule::IsEnable(char* lpszTaskName)
{
	if (NULL == m_lpRootFolder)
	{
		return FALSE;
	}
	HRESULT hr = S_OK;
	CComVariant variantTaskName(NULL);
	CComVariant variantEnable(NULL);
	variantTaskName = lpszTaskName;                   
	IRegisteredTask* pRegisteredTask = NULL;
	
	hr = m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
	if (FAILED(hr) || (NULL == pRegisteredTask))
	{
		return FALSE;
	}
	
	pRegisteredTask->get_Enabled(&variantEnable.boolVal);
	pRegisteredTask->Release();
	if (ATL_VARIANT_FALSE == variantEnable.boolVal)
	{
		return FALSE;
	}

	return TRUE;
}


BOOL CMyTaskSchedule::SetEnable(char* lpszTaskName, BOOL bEnable)
{
	if (NULL == m_lpRootFolder)
	{
		return FALSE;
	}
	HRESULT hr = S_OK;
	CComVariant variantTaskName(NULL);
	CComVariant variantEnable(NULL);
	variantTaskName = lpszTaskName;                     
	variantEnable = bEnable;                            
	IRegisteredTask* pRegisteredTask = NULL;
	
	hr = m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
	if (FAILED(hr) || (NULL == pRegisteredTask))
	{
		return FALSE;
	}
	
	pRegisteredTask->put_Enabled(variantEnable.boolVal);
	pRegisteredTask->Release();

	return TRUE;
}


