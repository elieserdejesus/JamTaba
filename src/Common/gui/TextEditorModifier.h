#ifndef TEXT_EDITOR_MODIFIER_H
#define TEXT_EDITOR_MODIFIER_H

#include <QLineEdit>

class TextEditorModifier
{
public:
    virtual void install(QLineEdit *lineEdit, bool finishOnReturnKeyPressed, const QString &dialogObjectName = "") = 0;
};

#endif // TEXT_EDITOR_MODIFIER_H
