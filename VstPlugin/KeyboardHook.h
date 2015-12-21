#ifndef KEYBOARDHOOK_H
#define KEYBOARDHOOK_H

#include "Windows.h"

class KeyboardHook
{
public:
    KeyboardHook();
    static void installLowLevelKeyboardHook();
    static void uninstallLowLevelKeyboardKook();
private:
    static HHOOK globalKeyboardHook;
};

#endif // KEYBOARDHOOK_H
