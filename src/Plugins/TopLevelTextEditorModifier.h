#ifndef TOP_LEVEL_TEXT_EDITOR_MODIFIER_H
#define TOP_LEVEL_TEXT_EDITOR_MODIFIER_H

#include "TextEditorModifier.h"

#include <QLineEdit>

class TopLevelTextEditorModifier : public QObject, public TextEditorModifier
{

public:
    void installModifier(QLineEdit *textEditor) override;

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;

private:
    class Dialog;
    Dialog *dialog;

    void showDialog();

    QLineEdit *hackedLineEdit;
};

#endif // TOP_LEVEL_TEXT_EDITOR_MODIFIER_H
