#ifndef VSTEDITOR_H
#define VSTEDITOR_H

#pragma warning(push)
#pragma warning(disable: 4100) // Unreferenced formal parameter
#include "aeffeditor.h"
#pragma warning(pop)

#include <QWinWidget>

class JamTabaVSTPlugin;
class MainWindow;

class VstEditor : public AEffEditor
{
    QWinWidget *widget;
    JamTabaVSTPlugin *jamtaba;
    ERect rectangle;
    MainWindow *mainWindow;

public:
    VstEditor(JamTabaVSTPlugin *jamtaba);
    ~VstEditor();

    bool getRect(ERect **rect) override;
    bool open(void *ptr) override;
    void close() override;

    void resize(int width, int height);
    void detachMainController();
    void deleteMainWindow();
};

#endif // VSTEDITOR_H
