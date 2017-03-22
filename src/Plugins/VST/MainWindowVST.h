#ifndef MAINWINDOWVST_H
#define MAINWINDOWVST_H

#include "MainWindowPlugin.h"

class MainControllerVST;

class MainWindowVST : public MainWindowPlugin
{
public:
    MainWindowVST(MainControllerVST *controller);

protected:
    TextEditorModifier *createTextEditorModifier() override;
};

#endif // MAINWINDOWVST_H
