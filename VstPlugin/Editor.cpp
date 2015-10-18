#include "Editor.h"

#include "Plugin.h"
#include <Windows.h>
#include <QTimer>

//++++++++++++++++++++++++++++++++++++++++++++++++++
/***
 * This class is used to allow ARROWS and RETURN key in vst plugin. Using
 *  QWinWidget directly the ARROW and RETURN keys don't work. The REturn/ENTER
 *  key are very important in chat, so this workaround is necessary.
 */
class CustomWinWidget : public QWinWidget{
public:
    CustomWinWidget(HWND parent)
        :QWinWidget(parent){
   }
protected:
    //trap the native message and say to Windows: Hey, I want all key pressing, including Arrows and RETURN!
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
    qCDebug(pluginVst) << "VstEditor detructor...";
    deleteMainWindow();
    qCDebug(pluginVst) << "VstEditor detructor done.";
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
    qCDebug(pluginVst) << "VstEditor::open()...";
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
        qCDebug(pluginVst) << "Creating MainWindow...";
        mainWindow = new MainWindow(jamtaba->getController());
        jamtaba->getController()->setMainWindow(mainWindow);
        rectangle.left = 0;
        rectangle.top = 0;
        rectangle.right = mainWindow->width();
        rectangle.bottom = mainWindow->height();
        qCDebug(pluginVst) << "Creating MainWindow done.";
    }
    mainWindow->setParent(widget);
    mainWindow->move( 0, 0 );

    rectangle.bottom = mainWindow->height();
    rectangle.right = mainWindow->width();

    widget->move( 0, 0 );
    widget->resize( rectangle.right, rectangle.bottom );
    widget->setMinimumSize(mainWindow->minimumSize());

    qCDebug(pluginVst) << "Window Rect  width:" << rectangle.right - rectangle.left;

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
    qCDebug(pluginVst) << "VstEditor::open() done.";
    return true;
 }


void VstEditor::close(){
    qCDebug(pluginVst) << "VstEditor::close()...";
    if(mainWindow){
        mainWindow->setParent(nullptr);
    }
    if(widget){
        delete widget;
        widget = nullptr;
    }
    AEffEditor::close();
    qCDebug(pluginVst) << "VstEditor::close() done.";
}
