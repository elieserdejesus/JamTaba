#include "WindowsStackWalker.h"

#include <QDebug>

WindowsStackWalker::WindowsStackWalker() :
        StackWalker()
{

}

void WindowsStackWalker::OnOutput(LPCSTR szText)
{
    qCritical() << szText;
}

void WindowsStackWalker::OnSymInit(LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName)
{
    Q_UNUSED(szSearchPath);
    Q_UNUSED(symOptions);
    Q_UNUSED(szUserName);
}

void WindowsStackWalker::OnLoadModule(LPCSTR , LPCSTR mod, DWORD64 baseAddr, DWORD size, DWORD result,
                                LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion)
{
    Q_UNUSED(mod);
    Q_UNUSED(baseAddr);
    Q_UNUSED(size);
    Q_UNUSED(result);
    Q_UNUSED(symType);
    Q_UNUSED(pdbName);
    Q_UNUSED(fileVersion);
}

void WindowsStackWalker::OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr)
{
    Q_UNUSED(szFuncName);
    Q_UNUSED(gle);
    Q_UNUSED(addr);
}

LONG WINAPI WindowsStackWalker::topLevelExceptionHandler(PEXCEPTION_POINTERS)
{
    WindowsStackWalker stackWalker;
    stackWalker.ShowCallstack();

    return EXCEPTION_CONTINUE_SEARCH;
}
