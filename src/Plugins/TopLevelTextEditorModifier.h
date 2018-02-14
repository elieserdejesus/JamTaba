#ifndef TOP_LEVEL_TEXT_EDITOR_MODIFIER_H
#define TOP_LEVEL_TEXT_EDITOR_MODIFIER_H

#include "gui/TextEditorModifier.h"

#include <QSharedPointer>

class TopLevelTextEditorModifier : public QObject, public TextEditorModifier
{

public:
    TopLevelTextEditorModifier();
    ~TopLevelTextEditorModifier();

    void modify(QLineEdit *textEditor, bool finishEditorPressingReturnKey) override;
    void modify(QComboBox *comboBox) override;

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;

private:
    void showDialog();

    void transferTextToHackedLineEdit();

    void doInstall(QLineEdit* lineEdit);

    bool isValidFocusInEvent(QEvent *ev) const;

    bool isHackingComboBox() const;

    QLineEdit *hackedLineEdit;
    QLineEdit *topLevelLineEdit;
    bool hideDialogWhenReturnIsPressed;

    static QList<QSharedPointer<TopLevelTextEditorModifier>> createdModifiers;

};

#endif // TOP_LEVEL_TEXT_EDITOR_MODIFIER_H
