#ifndef VSTEDITOR_H
#define VSTEDITOR_H

#pragma warning(push)
#pragma warning(disable: 4100) // Unreferenced formal parameter
#include "aeffeditor.h"
#pragma warning(pop)

#include <QWinWidget>

class JamtabaPlugin;
class MainWindow;

class VstEditor : public AEffEditor
{
    QWinWidget *widget;
    JamtabaPlugin *jamtaba;
    ERect rectangle;
    MainWindow *mainWindow;

public:
    VstEditor(JamtabaPlugin *jamtaba);
    ~VstEditor();
    bool getRect(ERect **rect);
    void resize(int width, int height);
    bool open(void *ptr);
    void close();
    void detachMainController();
    void deleteMainWindow();
};

#endif // VSTEDITOR_H
