#include "Editor.h"

#include "Plugin.h"
#include <Windows.h>
#include <QTimer>
#include <QKeyEvent>
#include <QLineEdit>
#include "MainWindowVST.h"
#include "../log/logging.h"

/*
    This is a callback function to hook VST host key pressing. This is a workaround to
solve the problem of "space bar" pressing in VST hosts. In general hosts
use the space bar to stop/play. If the host is using space bar as a HotKey the Jamtaba
text input widgets never receive the WM_KEY[DOWN/UP] messages. I confirmed
this issue disabling the play/stop hotkey in Reaper and see the space bar
working without problem.

    So, I'm hooking the global key pressing, and IF A JAMTABA QLineEdit WIDGET is FOCUSED pass the
key[press/release] message to this widget and not pass the same message to VST host,
avoiding start/stop VST host every time we press space bar in chat.
*/
LRESULT CALLBACK globalKeyboardHookProcedure(int nCode, WPARAM wParam, LPARAM lParam){
    if(nCode == HC_ACTION){
        KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *) lParam;
        if(p->vkCode == VK_SPACE){
            QWidget* focusWidget = QApplication::focusWidget();
            bool widgetIsQLineEditInstance = qobject_cast<QLineEdit*>(focusWidget);//qobject_cast return NULL when the cast fail.
            if(widgetIsQLineEditInstance){//just apply the keyboard hook
                QKeyEvent::Type evType = (wParam == WM_KEYDOWN) ? QKeyEvent::KeyPress : QKeyEvent::KeyRelease;
                QKeyEvent* ev = new QKeyEvent(evType, Qt::Key_Space, Qt::NoModifier, " ");
                QCoreApplication::postEvent(QApplication::focusWidget(), ev );
                return 1;//The VST host don't receive the key event, we are handling this event now.
            }
        }
    }
    return CallNextHookEx(NULL,nCode,wParam,lParam);// Forward the event to VST host
}
//++++++++++++++++++++++++++++++++++++++++++++++++++
/***
This class is used to allow ARROWS and RETURN key in VST plugin text input widgets
Using QWinWidget directly the ARROW and RETURN keys don't work. The Return/ENTER
key are very important in chat, so this workaround is necessary.
*/
class CustomWinWidget : public QWinWidget{
public:
    CustomWinWidget(HWND parent)
        :QWinWidget(parent){
        //installing the global keyboard hook in constructor and de-installing in destructor.
        //See the description in the first lines of this file to understand why we are using
        //this keyboard hook.
        globalKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, globalKeyboardHookProcedure, qWinAppInst(), NULL);
        if (!globalKeyboardHook){
            qCritical() << "Hook failed for application instance" << qWinAppInst() << "with error:" << GetLastError();
        }
    }

    ~CustomWinWidget(){
        if(globalKeyboardHook){
            UnhookWindowsHookEx(globalKeyboardHook);
        }
    }
private:
    HHOOK globalKeyboardHook;
protected:
    //Trapping the native message and say to Windows: Hey, I want all key pressing, including Arrows and RETURN!
    //This not work when VST Host is using hotKeys, a very common case. The globalKeyboardHook
    //is used to solve the scenarios where VST host is using hotKeys and we need these keys too.
    bool nativeEvent(const QByteArray &eventType, void *message, long *result){
        MSG *msg = (MSG *)message;
        if(msg->message == WM_GETDLGCODE){
           *result = DLGC_WANTALLKEYS;
           return true;
        }
        return QWinWidget::nativeEvent(eventType, message, result);
    }

};
//++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++
VstEditor::VstEditor(JamtabaPlugin *jamtaba)
    :widget(NULL), jamtaba(jamtaba), mainWindow(nullptr)
{
}

void VstEditor::deleteMainWindow(){
    if(mainWindow){
        if(mainWindow->isVisible()){
            QTimer::singleShot(0, mainWindow, &MainWindow::close);
        }
        mainWindow->deleteLater();
        mainWindow = nullptr;
    }
}

VstEditor::~VstEditor()
{
    qCDebug(jtVstPlugin) << "VstEditor detructor...";
    deleteMainWindow();
    qCDebug(jtVstPlugin) << "VstEditor detructor done.";
}

void VstEditor::detachMainController(){
    if(mainWindow){
        //mainWindow->detachMainController();
    }
}

bool VstEditor::getRect (ERect** rect)
{
    *rect = &rectangle;
    return true;
}

void VstEditor::clientResize(HWND h_parent, int width, int height)
{
    RECT rcClient, rcWindow;
    POINT ptDiff;
    GetClientRect(h_parent, &rcClient);
    GetWindowRect(h_parent, &rcWindow);
    ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
    ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
    MoveWindow(h_parent, rcWindow.left, rcWindow.top, width + ptDiff.x, height + ptDiff.y, TRUE);
}

bool VstEditor::open(void* ptr){
    qCDebug(jtVstPlugin) << "VstEditor::open()...";
    if(!ptr )
        return false;

    if(!jamtaba->isRunning()){
        jamtaba->initialize();//initialize first time editor is opened
    }

    AEffEditor::open(ptr);
    widget = new CustomWinWidget(static_cast<HWND>(ptr));
    widget->setAutoFillBackground(false);
    //widget->setObjectName("QWinWidget");

    if(!mainWindow){
        qCDebug(jtVstPlugin) << "Creating MainWindow...";
        mainWindow = new MainWindowVST(jamtaba->getController());
        jamtaba->getController()->setMainWindow(mainWindow);
        rectangle.left = 0;
        rectangle.top = 0;
        rectangle.right = mainWindow->width();
        rectangle.bottom = mainWindow->height();
        qCDebug(jtVstPlugin) << "Creating MainWindow done.";
    }
    mainWindow->setParent(widget);
    mainWindow->move( 0, 0 );

    rectangle.bottom = mainWindow->height();
    rectangle.right = mainWindow->width();

    widget->move( 0, 0 );
    widget->resize( rectangle.right, rectangle.bottom );
    widget->setMinimumSize(mainWindow->minimumSize());

    qCDebug(jtVstPlugin) << "Window Rect  width:" << rectangle.right - rectangle.left;

    widget->setPalette( mainWindow->palette() );

//    resizeH = new ResizeHandle(widget);
//    QPoint pos( widget->geometry().bottomRight() );
//    pos.rx()-=resizeH->width();
//    pos.ry()-=resizeH->height();
//    resizeH->move(pos);
//    resizeH->show();

//    connect(resizeH, SIGNAL(Moved(QPoint)),
//            this, SLOT(OnResizeHandleMove(QPoint)));

    widget->show();
    qCDebug(jtVstPlugin) << "VstEditor::open() done.";
    return true;
 }


void VstEditor::close(){
    qCDebug(jtVstPlugin) << "VstEditor::close()...";
    if(mainWindow){
        mainWindow->setParent(nullptr);
    }
    if(widget){
        delete widget;
        widget = nullptr;
    }
    AEffEditor::close();
    qCDebug(jtVstPlugin) << "VstEditor::close() done.";
}
