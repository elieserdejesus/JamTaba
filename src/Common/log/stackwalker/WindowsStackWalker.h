#include <Windows.h>
#include "StackWalker.h"

class WindowsStackWalker :public StackWalker
{

public:
    WindowsStackWalker();

    static LONG WINAPI topLevelExceptionHandler(PEXCEPTION_POINTERS);

protected:
    void OnOutput(LPCSTR szText) override;

    void OnSymInit(LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName) override;

    void OnLoadModule(LPCSTR , LPCSTR mod, DWORD64 baseAddr, DWORD size, DWORD result, LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion) override;

    void OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr) override;

};
