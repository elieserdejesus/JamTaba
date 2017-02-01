#ifndef TOP_LEVEL_TEXT_EDITOR_H
#define TOP_LEVEL_TEXT_EDITOR_H

#include <QLineEdit>

class TopLevelTextEditor : public QLineEdit
{

public:
    TopLevelTextEditor(QWidget *parent);

protected:
    void focusInEvent(QFocusEvent *ev) override;

private:
    class Dialog;

    Dialog *dialog;

    void showDialog();
};

#endif // TOP_LEVEL_TEXT_EDITOR_H
