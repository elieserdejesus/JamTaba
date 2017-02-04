#ifndef TEXT_EDITOR_MODIFIER_H
#define TEXT_EDITOR_MODIFIER_H

#include <QLineEdit>
#include <QComboBox>

class TextEditorModifier
{
public:
    virtual void modify(QLineEdit *lineEdit, bool finishOnReturnKeyPressed, const QString &dialogObjectName = "") = 0;
    virtual void modify(QComboBox *comboBox) = 0;
};

#endif // TEXT_EDITOR_MODIFIER_H
