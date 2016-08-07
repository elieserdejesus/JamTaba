#ifndef KEYBOARDHOOK_H
#define KEYBOARDHOOK_H

#include "Windows.h"

class KeyboardHook
{
public:
    static void installLowLevelKeyboardHook();
    static void uninstallLowLevelKeyboardKook();
    static bool lastImeKeyUpWasReturn;
private:
    static HHOOK globalKeyboardHook;

};

#endif // KEYBOARDHOOK_H
