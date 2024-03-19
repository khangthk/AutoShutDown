#include "power.h"
#include <Windows.h>
#include <powrprof.h>
#include <VersionHelpers.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "powrprof.lib")

bool get_shutdown_privilege()
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    // Get a token for this process.

    if (!OpenProcessToken(GetCurrentProcess(),
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return(false);

    // Get the LUID for the shutdown privilege.

    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
        &tkp.Privileges[0].Luid);

    tkp.PrivilegeCount = 1;  // one privilege to set
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // Get the shutdown privilege for this process.

    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
        (PTOKEN_PRIVILEGES)NULL, 0);

    if (GetLastError() != ERROR_SUCCESS)
        return false;
    return true;
}

bool power(UINT uFlag)
{
    if (!get_shutdown_privilege()) return false;
    return ExitWindowsEx(uFlag, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_UPGRADE | SHTDN_REASON_FLAG_PLANNED);
}

bool suspend(BOOL hibernate)
{
    if (!get_shutdown_privilege()) return false;
    return SetSuspendState(hibernate, TRUE, FALSE);
}

bool Power::sleep()
{
    return suspend(FALSE);
}

bool Power::hibernate()
{
    return suspend(TRUE);
}

bool Power::is_support_hibernate()
{
    return IsPwrHibernateAllowed();
}

bool Power::shutdown(bool force)
{
    if (force) return power(EWX_SHUTDOWN | EWX_FORCE);
    return power(EWX_SHUTDOWN);
}

bool Power::shutdown_hybrib(bool force)
{
    if (IsWindows8OrGreater())
    {
        if (force) return power(EWX_HYBRID_SHUTDOWN | EWX_SHUTDOWN | EWX_FORCE);
        return power(EWX_HYBRID_SHUTDOWN | EWX_SHUTDOWN);
    }
    return false;
}

bool Power::restart(bool force)
{
    if (force) return power(EWX_REBOOT | EWX_FORCE);
    return power(EWX_REBOOT);
}

bool Power::lock()
{
    return LockWorkStation();
}

bool Power::signout()
{
    return ExitWindowsEx(EWX_LOGOFF, 0);
}

void Power::monitor_off()
{
    SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);
}
