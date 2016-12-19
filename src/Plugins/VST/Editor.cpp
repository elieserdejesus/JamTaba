#include "Editor.h"

#include "JamTabaVSTPlugin.h"
#include "MainWindowPlugin.h"
#include "log/Logging.h"
#include "KeyboardHook.h"
#include <QTimer>

// ++++++++++++++++++++++++++++++++++++++++++++++++++
VstEditor::VstEditor(JamTabaVSTPlugin *jamtaba) :
    widget(NULL),
    jamtaba(jamtaba),
    mainWindow(nullptr)
{
}

void VstEditor::deleteMainWindow()
{
    if (mainWindow) {
        if (mainWindow->isVisible())
            QTimer::singleShot(0, mainWindow, &MainWindow::close);
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

void VstEditor::detachMainController()
{
    if (mainWindow) {
        // mainWindow->detachMainController();
    }
}

bool VstEditor::getRect(ERect **rect)
{
    *rect = &rectangle;
    return true;
}

void VstEditor::resize(int width, int height)
{
    if (widget)
        widget->resize(width, height);
    if (mainWindow)
        mainWindow->resize(width, height);
    rectangle.right = width;
    rectangle.bottom = height;

    // clientResize( (HWND)systemWindow, width, height );
}

bool VstEditor::open(void *ptr)
{
    qCDebug(jtVstPlugin) << "VstEditor::open()...";
    if (!ptr)
        return false;

    if (!jamtaba->isRunning()) {
        // initialize first time editor is opened. This avoid initialize jamtaba when the host is just scanning.
        jamtaba->initialize();
    }

    AEffEditor::open(ptr);
    widget = new QWinWidget(static_cast<HWND>(ptr));
    widget->setAutoFillBackground(false);
    KeyboardHook::installLowLevelKeyboardHook();

    if (!mainWindow) {
        qCDebug(jtVstPlugin) << "Creating MainWindow...";
        MainControllerVST *controller = dynamic_cast<MainControllerVST*>(jamtaba->getController());
        mainWindow = new MainWindowVST(controller);
        controller->setMainWindow(mainWindow);
        mainWindow->initialize();
        rectangle.left = 0;
        rectangle.top = 0;
        rectangle.right = mainWindow->width();
        rectangle.bottom = mainWindow->height();
        qCDebug(jtVstPlugin) << "Creating MainWindow done.";
    }
    mainWindow->setParent(widget);
    mainWindow->move(0, 0);

    rectangle.bottom = mainWindow->height();
    rectangle.right = mainWindow->width();

    widget->move(0, 0);
    widget->resize(rectangle.right, rectangle.bottom);
    widget->setMinimumSize(mainWindow->minimumSize());

    qCDebug(jtVstPlugin) << "Window Rect  width:" << rectangle.right - rectangle.left;

    widget->setPalette(mainWindow->palette());

    widget->show();
    qCDebug(jtVstPlugin) << "VstEditor::open() done.";
    return true;
}

void VstEditor::close()
{
    qCDebug(jtVstPlugin) << "VstEditor::close()...";
    if (mainWindow)
        mainWindow->setParent(nullptr);
    if (widget) {
        delete widget;
        widget = nullptr;
        KeyboardHook::uninstallLowLevelKeyboardKook();
    }
    AEffEditor::close();
    qCDebug(jtVstPlugin) << "VstEditor::close() done.";
}
