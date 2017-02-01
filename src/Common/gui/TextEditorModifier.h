#ifndef TEXT_EDITOR_MODIFIER_H
#define TEXT_EDITOR_MODIFIER_H

#include <QLineEdit>

class TextEditorModifier
{
public:
    virtual void installModifier(QLineEdit *lineEdit) = 0;
};

#endif // TEXT_EDITOR_MODIFIER_H
