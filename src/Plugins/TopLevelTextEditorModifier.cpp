#include "TopLevelTextEditorModifier.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QDebug>
#include <QEvent>
#include <QLineEdit>

/**

 This class is showing a QDialog with a QLineEdit to avoid hosts (AU and VST) eating keystrokes.
 When the text input field is inside a toplevel window (like a QDialog) the keystrokes are working ok.

*/

// Nested class used to show a frameless dialog behaving like a QLineEdit
class TopLevelTextEditorModifier::Dialog : public QDialog
{

public:
    TopLevelTextEditorModifier::Dialog(QLineEdit *lineEditToHack)
        : QDialog(nullptr, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint),
        hackedLineEdit(lineEditToHack)
    {
        setAttribute(Qt::WA_DeleteOnClose);

        QLayout *layout = new QVBoxLayout();
        layout->setContentsMargins(0, 0, 0, 0);
        setLayout(layout);
        internalLineEdit = new QLineEdit(lineEditToHack->text(), this);
        internalLineEdit->setObjectName(lineEditToHack->objectName());
        internalLineEdit->setAlignment(lineEditToHack->alignment());

        layout->addWidget(internalLineEdit);

        connect(internalLineEdit, &QLineEdit::editingFinished, [=]{

            hackedLineEdit->setText(internalLineEdit->text());
            hackedLineEdit->returnPressed(); // simulate the RETURN key pressing

            close(); // close and delete the dialog
        });
    }

private:
    QLineEdit *hackedLineEdit;
    QLineEdit *internalLineEdit;

};
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void TopLevelTextEditorModifier::installModifier(QLineEdit *lineEdit)
{
    if (!lineEdit)
        return;

    hackedLineEdit = lineEdit;

    lineEdit->installEventFilter(this);
}

void TopLevelTextEditorModifier::showDialog()
{
    dialog = new TopLevelTextEditorModifier::Dialog(hackedLineEdit);

    dialog->resize(hackedLineEdit->frameSize());
    QPoint hackedLineEditTopLeft = hackedLineEdit->rect().topLeft();
    dialog->move(hackedLineEdit->mapToGlobal(hackedLineEditTopLeft));

    dialog->raise();
    dialog->activateWindow();

    dialog->show();
}

bool TopLevelTextEditorModifier::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == hackedLineEdit && ev->type() == QEvent::FocusIn) {
        showDialog();
    }

    return false;
}
