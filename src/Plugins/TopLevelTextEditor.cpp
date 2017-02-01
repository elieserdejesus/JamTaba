#include "TopLevelTextEditor.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QDebug>

/**
 This class is showing a QDialog with a QLineEdit to avoid hosts (AU and VST) eating keystrokes.
 When the text input field is inside a toplevel window (like a QDialog) the keystrokes are working ok.
 */

// Nested class used to show a frameless dialog behaving like a QLineEdit
class TopLevelTextEditor::Dialog : public QDialog
{

public:
    TopLevelTextEditor::Dialog(TopLevelTextEditor *textEditor)
        : QDialog(nullptr, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint),
        textEditor(textEditor)
    {
        setAttribute(Qt::WA_DeleteOnClose);

        QLayout *layout = new QVBoxLayout();
        layout->setContentsMargins(0, 0, 0, 0);
        setLayout(layout);
        lineEdit = new QLineEdit(textEditor->text(), this);
        layout->addWidget(lineEdit);
        lineEdit->setStyleSheet("background: rgba(0, 0, 0, 80);");

        connect(lineEdit, &QLineEdit::editingFinished, this, &Dialog::close);
    }

    void TopLevelTextEditor::Dialog::closeEvent(QCloseEvent *ev)
    {
        QDialog::closeEvent(ev);

        clearFocus();

        if (textEditor) {
            textEditor->setText(lineEdit->text());
        }
    }

    void TopLevelTextEditor::Dialog::focusInEvent(QFocusEvent *ev)
    {
        QDialog::focusInEvent(ev);

        if (lineEdit)
            lineEdit->setFocus();
    }

    void TopLevelTextEditor::Dialog::focusOutEvent(QFocusEvent *ev)
    {
        QDialog::focusOutEvent(ev);
        close();
    }

private:
    TopLevelTextEditor *textEditor;
    QLineEdit *lineEdit;

};
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TopLevelTextEditor::TopLevelTextEditor(QWidget *parent)
    : QLineEdit(parent),
      dialog(nullptr)
{
    setAttribute(Qt::WA_ShowWithoutActivating);
    setFocusPolicy(Qt::ClickFocus);
}

void TopLevelTextEditor::showDialog()
{
    clearFocus();

    dialog = new TopLevelTextEditor::Dialog(this);

    dialog->resize(frameSize());
    dialog->move(mapToGlobal(rect().topLeft()));

    dialog->raise();
    dialog->activateWindow();

    dialog->setFocus();
    dialog->show();
}

void TopLevelTextEditor::focusInEvent(QFocusEvent *ev)
{
    QLineEdit::focusInEvent(ev);
    showDialog();
}
