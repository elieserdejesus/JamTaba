#ifndef KEYBOARDHOOK_H
#define KEYBOARDHOOK_H

#include "Windows.h"
#include <QString>

namespace keyboardHook
{
    void installLowLevelKeyboardHook();
    void uninstallLowLevelKeyboardKook();

    QString vkCodeToText(DWORD vkCode, DWORD scanCode);

    extern bool lastImeKeyUpWasReturn;
    extern HHOOK globalKeyboardHook;
}

#endif // KEYBOARDHOOK_H
