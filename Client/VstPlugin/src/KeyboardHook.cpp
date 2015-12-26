#include "KeyboardHook.h"
#include <QWidget>
#include <QApplication>
#include <QKeyEvent>
#include <QLineEdit>
#include <QDebug>

HHOOK KeyboardHook::globalKeyboardHook = nullptr;

/**
    This is a callback function to hook VST host key pressing. In general VST hosts are using some Keys as
HotKeys. So, the host code get the key pressing event activate the host HotKey and don't send the key pressing
to Jamtaba.
    So, I'm hooking the global key pressing, and IF A JAMTABA QLineEdit WIDGET is FOCUSED passing the
key[press/release] message to this widget and not passing the same message to VST host.

*/
LRESULT CALLBACK globalKeyboardHookProcedure(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION) {
        QWidget *focusWidget = QApplication::focusWidget();

        // qobject_cast return NULL when the cast fail.
        bool widgetIsQLineEditInstance = qobject_cast<QLineEdit *>(focusWidget);

        if (widgetIsQLineEditInstance) {
            QKeyEvent *ev = nullptr;
            QKeyEvent::Type eventType
                = (wParam == WM_KEYDOWN) ? QKeyEvent::KeyPress : QKeyEvent::KeyRelease;
            KBDLLHOOKSTRUCT *lowLevelKeyboardStruct = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);

            switch (lowLevelKeyboardStruct->vkCode) {
            case VK_SPACE:
                ev = new QKeyEvent(eventType, Qt::Key_Space, Qt::NoModifier, " ");
                break;
            case VK_RETURN:
                ev = new QKeyEvent(eventType, Qt::Key_Return, Qt::NoModifier);
                break;
            case VK_BACK:
                ev = new QKeyEvent(eventType, Qt::Key_Backspace, Qt::NoModifier);
                break;
            case VK_LEFT:
                ev = new QKeyEvent(eventType, Qt::Key_Left, Qt::NoModifier);
                break;
            case VK_RIGHT:
                ev = new QKeyEvent(eventType, Qt::Key_Right, Qt::NoModifier);
                break;
            case VK_DELETE:
                ev = new QKeyEvent(eventType, Qt::Key_Delete, Qt::NoModifier);
                break;
            case VK_SHIFT:
            case VK_LSHIFT:
            case VK_RSHIFT:
                return 0; // return 0 (the event is not consumed) and keep the SHIFT modifier in the keyboard buffer.
            default:
                if (eventType == QKeyEvent::KeyPress) { // handling only KeyPress to avoid problems with 'é', 'ã', etc
                    wchar_t buffer[5];
                    BYTE keyboard_state[256];
                    if (GetKeyboardState(keyboard_state)) {
                        // Try to convert the key information to text
                        int result = ToUnicodeEx(lowLevelKeyboardStruct->vkCode,
                                                 lowLevelKeyboardStruct->scanCode, keyboard_state,
                                                 buffer, 5, lowLevelKeyboardStruct->flags, GetKeyboardLayout(
                                                     0));
                        if (result > 0)
                            ev = new QKeyEvent(eventType, lowLevelKeyboardStruct->vkCode,
                                               Qt::ShiftModifier, QString::fromWCharArray(buffer));
                    }
                }
            }
            if (ev)
                QApplication::postEvent(focusWidget, ev);
            return 1; // if the QLineEdit is the focused widget always consume the event and don't forward to the Vst host.
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);// Forward the event to VST host
}

KeyboardHook::KeyboardHook()
{
}

void KeyboardHook::installLowLevelKeyboardHook()
{
    if (!globalKeyboardHook) {// not installed?
        globalKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, globalKeyboardHookProcedure,
                                              qWinAppInst(), NULL);
        if (!globalKeyboardHook)
            qCritical() << "Hook failed for application instance" << qWinAppInst()
                        << "with error:" << GetLastError();
    }
}

void KeyboardHook::uninstallLowLevelKeyboardKook()
{
    if (globalKeyboardHook) {
        UnhookWindowsHookEx(globalKeyboardHook);
        globalKeyboardHook = nullptr;
    }
}
