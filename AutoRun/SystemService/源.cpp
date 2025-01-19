#include <windows.h>
#include <tchar.h>
#include <fstream>
#include <iostream>

SERVICE_STATUS_HANDLE g_ServiceStatusHandle = NULL;
SERVICE_STATUS g_ServiceStatus = { 0 };

// 日志文件路径
const TCHAR* LOG_FILE = _T("C:\\MyServiceLog.txt");

// 服务的主入口点
void WINAPI ServiceMain(DWORD argc, LPTSTR* argv);
void WINAPI ServiceCtrlHandler(DWORD ctrlCode);
void ReportServiceStatus(DWORD currentState, DWORD exitCode, DWORD waitHint);
void WriteLog(const char* message);

int main()
{
    SERVICE_TABLE_ENTRY ServiceTable[] =
    {
        { LPWSTR("MyService"), (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { NULL, NULL }
    };

    if (StartServiceCtrlDispatcher(ServiceTable) == FALSE)
    {
        return GetLastError();
    }

    return 0;
}

void WINAPI ServiceMain(DWORD argc, LPTSTR* argv)
{
    g_ServiceStatusHandle = RegisterServiceCtrlHandler(_T("MyService"), ServiceCtrlHandler);

    ReportServiceStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

    // 服务启动逻辑
    WriteLog("Service is starting...");

    ReportServiceStatus(SERVICE_RUNNING, NO_ERROR, 0);

    // 进入服务主循环
    while (g_ServiceStatus.dwCurrentState == SERVICE_RUNNING)
    {
        // 这里可以放置服务的主逻辑
        WriteLog("Service is running...");
        Sleep(1000); // 模拟工作
    }

    WriteLog("Service is stopping...");
    ReportServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);
}

void WINAPI ServiceCtrlHandler(DWORD ctrlCode)
{
    switch (ctrlCode)
    {
    case SERVICE_CONTROL_STOP:
        ReportServiceStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
        g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        break;
    default:
        break;
    }
}

void ReportServiceStatus(DWORD currentState, DWORD exitCode, DWORD waitHint)
{
    g_ServiceStatus.dwServiceType = SERVICE_WIN32;
    g_ServiceStatus.dwCurrentState = currentState;
    g_ServiceStatus.dwControlsAccepted = (currentState == SERVICE_STOPPED) ? 0 : SERVICE_ACCEPT_STOP;
    g_ServiceStatus.dwWin32ExitCode = exitCode;
    g_ServiceStatus.dwCheckPoint = 0;
    g_ServiceStatus.dwWaitHint = waitHint;

    SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus);
}

void WriteLog(const char* message)
{
    std::ofstream logFile;
    logFile.open(LOG_FILE, std::ios_base::app); // 以追加模式打开
    if (logFile.is_open())
    {
        logFile << message << std::endl;
        logFile.close();
    }
}
