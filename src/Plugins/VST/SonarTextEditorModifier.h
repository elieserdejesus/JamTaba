#ifndef _SONAR_TEXT_EDITOR_MODIFIER_H
#define _SONAR_TEXT_EDITOR_MODIFIER_H

#include <QObject>
#include "gui/TextEditorModifier.h"

class SonarTextEditorModifier : public QObject, public TextEditorModifier
{
    Q_OBJECT

public:
    SonarTextEditorModifier();
    ~SonarTextEditorModifier();

    void modify(QLineEdit *lineEdit, bool finishOnReturnKeyPressed) override;
    void modify(QComboBox *comboBox) override;
};

#endif
