#ifndef TOP_LEVEL_TEXT_EDITOR_MODIFIER_H
#define TOP_LEVEL_TEXT_EDITOR_MODIFIER_H

#include "TextEditorModifier.h"

#include <QLineEdit>
#include <QSharedPointer>

class TopLevelTextEditorModifier : public QObject, public TextEditorModifier
{

public:
    TopLevelTextEditorModifier();

    void install(QLineEdit *textEditor, bool finishEditorPressingReturnKey) override;

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;

private:
    QSharedPointer<QDialog> dialog;

    QDialog *createDialog() const;

    void showDialog();

    void transferTextToHackedLineEdit();

    QLineEdit *hackedLineEdit;
    QLineEdit *topLevelLineEdit;
    bool finishPressingReturnKey;
};

#endif // TOP_LEVEL_TEXT_EDITOR_MODIFIER_H
