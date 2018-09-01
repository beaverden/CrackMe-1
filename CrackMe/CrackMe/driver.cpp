#include "driver.h"

BOOLEAN InstallService(LPCSTR lpBinaryPath)
{
    SC_HANDLE hSCManager = OpenSCManager(
        NULL,
        NULL,
        SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE
    );

    if (hSCManager == NULL)
    {
        DebugOutput("Cannot open SCManager");
        return FALSE;
    }

    SC_HANDLE hService = OpenService(
        hSCManager,
        "Salam",
        SERVICE_QUERY_STATUS // no rights needed
    );

    if (hService != NULL)
    {
        DebugOutput("Service already installed.");
        CloseServiceHandle(hService);
        return FALSE;
    }

    SC_HANDLE hSCDriver = CreateService(
        hSCManager,
        "Salam",
        "Salam",
        SERVICE_ALL_ACCESS,
        SERVICE_KERNEL_DRIVER,
        SERVICE_DEMAND_START,
        SERVICE_ERROR_CRITICAL,
        lpBinaryPath,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    );

    if (hSCDriver == NULL)
    {
        CloseServiceHandle(hSCManager);
        DebugOutput("Cannot register driver");
        return FALSE;
    }

    if (!StartService(hSCDriver, NULL, NULL))
    {
        CloseServiceHandle(hSCDriver);
        CloseServiceHandle(hSCManager);
        DebugOutput("Unable to start service (%d)", GetLastError());
        return FALSE;
    }

    DebugOutput("Service started.");
    CloseServiceHandle(hSCDriver);
    CloseServiceHandle(hSCManager);
    return TRUE;
}

BOOLEAN UninstallService()
{
    SC_HANDLE hSCManager = OpenSCManager(
        NULL,
        NULL,
        SC_MANAGER_CONNECT
    );

    if (hSCManager == NULL)
    {
        DebugOutput("Cannot open SCManager");
        return FALSE;
    }

    SC_HANDLE hService = OpenService(
        hSCManager,
        "Salam",
        DELETE | SERVICE_QUERY_STATUS | SERVICE_STOP
    );

    if (hService == NULL)
    {
        DWORD err = GetLastError();
        if (err != ERROR_SERVICE_DOES_NOT_EXIST)
        {
            CloseServiceHandle(hSCManager);
            DebugOutput("Unable to get installed service");
            return FALSE;
        }
        else
        {
            CloseServiceHandle(hSCManager);
            DebugOutput("Nothing to uninstall.");
            return TRUE;
        }
    }

    if (ServiceStop(hService))
    {
        DebugOutput("Service stopped.");
    }
    else 
    {
        CloseServiceHandle(hSCManager);
        CloseServiceHandle(hService);
        return FALSE;
    }

    BOOL result = DeleteService(hService);
    if (result != 0)
    {
        DebugOutput("Service uninstalled.");
    }
    else
    {
        CloseServiceHandle(hSCManager);
        CloseServiceHandle(hService);
        DebugOutput("Unable to delete the service");
        return FALSE;
    }
    CloseServiceHandle(hSCManager);
    CloseServiceHandle(hService);

    return TRUE;
}

BOOLEAN ServiceStop(SC_HANDLE hService)
{
    SERVICE_STATUS_PROCESS ctl_status = { 0 };
    DWORD needed = 0;
    if (!QueryServiceStatusEx(
        hService,
        SC_STATUS_PROCESS_INFO,
        (LPBYTE)&ctl_status,
        sizeof(ctl_status),
        &needed
    ))
    {
        DebugOutput("Control status failed, bytes needed: %d", needed);
        return FALSE;
    }
    if (ctl_status.dwCurrentState == SERVICE_STOPPED)
    {
        DebugOutput("Service already stopped.");
        return TRUE;
    }

    while (ctl_status.dwCurrentState == SERVICE_STOP_PENDING)
    {
        DWORD dwWaitTime = ctl_status.dwWaitHint / 10;

        if (dwWaitTime < MIN_WAIT_TIME)
            dwWaitTime = MIN_WAIT_TIME;
        else if (dwWaitTime > MAX_WAIT_TIME)
            dwWaitTime = MAX_WAIT_TIME;

        Sleep(dwWaitTime);
        if (!QueryServiceStatusEx(
            hService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&ctl_status,
            sizeof(ctl_status),
            &needed
        ))
        {
            DebugOutput("Control status failed, bytes needed: %d", needed);
            return FALSE;
        }
    }

    SERVICE_STATUS stop_status;
    DWORD controlResult = ControlService(
        hService,
        SERVICE_CONTROL_STOP,
        &stop_status
    );

    if (controlResult == 0) {
        DebugOutput("Service stopping failed");
        return FALSE;
    }

    return TRUE;
}