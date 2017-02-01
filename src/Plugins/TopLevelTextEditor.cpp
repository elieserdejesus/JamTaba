#include "TopLevelTextEditor.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QDebug>
#include <QEvent>

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
        lineEdit->setObjectName(textEditor->objectName());
        lineEdit->setAlignment(textEditor->alignment());

        layout->addWidget(lineEdit);

        connect(lineEdit, &QLineEdit::editingFinished, [=]{
            close();
            textEditor->returnPressed(); // simulate the RETURN key pressing
        });
    }

protected:
    void TopLevelTextEditor::Dialog::closeEvent(QCloseEvent *ev) override
    {
        QDialog::closeEvent(ev);

        if (textEditor) {
            textEditor->setText(lineEdit->text());
        }
    }

private:
    TopLevelTextEditor *textEditor;
    QLineEdit *lineEdit;

};
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TopLevelTextEditor::TopLevelTextEditor(const QString &text, QWidget *parent)
    : QLineEdit(text, parent),
      dialog(nullptr)
{
    setAttribute(Qt::WA_ShowWithoutActivating);
}

void TopLevelTextEditor::showDialog()
{
    dialog = new TopLevelTextEditor::Dialog(this);

    dialog->resize(frameSize());
    dialog->move(mapToGlobal(rect().topLeft()));

    dialog->raise();
    dialog->activateWindow();

    dialog->show();
}

void TopLevelTextEditor::focusInEvent(QFocusEvent *ev)
{
    QLineEdit::focusInEvent(ev);
    showDialog();
}
