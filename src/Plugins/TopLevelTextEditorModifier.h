#ifndef TOP_LEVEL_TEXT_EDITOR_MODIFIER_H
#define TOP_LEVEL_TEXT_EDITOR_MODIFIER_H

#include "TextEditorModifier.h"

#include <QSharedPointer>

class TopLevelTextEditorModifier : public QObject, public TextEditorModifier
{

public:
    TopLevelTextEditorModifier();

    void install(QLineEdit *textEditor, bool finishEditorPressingReturnKey, const QString &dialogObjectName = "") override;
    void install(QComboBox *comboBox) override;

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;

private:
    QSharedPointer<QDialog> dialog;

    QDialog *createDialog() const;

    void showDialog();

    void transferTextToHackedLineEdit();

    void doInstall(QLineEdit* lineEdit);

    bool isValidFocusInEvent(QEvent *ev) const;

    bool isHackingComboBox() const;

    QLineEdit *hackedLineEdit;
    QLineEdit *topLevelLineEdit;
    bool hideDialogWhenReturnIsPressed;
    QString dialogObjectName;

};

#endif // TOP_LEVEL_TEXT_EDITOR_MODIFIER_H
