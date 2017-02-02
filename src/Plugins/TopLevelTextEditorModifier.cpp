#include "TopLevelTextEditorModifier.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QDebug>
#include <QKeyEvent>
#include <QLineEdit>
#include <QTimer>

/**

 This class is showing a QDialog with a QLineEdit to avoid hosts (AU and VST) eating keystrokes.
 When the text input field is inside a toplevel window (like a QDialog) the keystrokes are working ok.

*/

TopLevelTextEditorModifier::TopLevelTextEditorModifier()
    : dialog(nullptr),
      topLevelLineEdit(nullptr),
      hackedLineEdit(nullptr),
      finishPressingReturnKey(false)
{
    //
}

QDialog *TopLevelTextEditorModifier::createDialog() const
{
    QDialog *newDialog = new QDialog(nullptr, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    newDialog->setAttribute(Qt::WA_DeleteOnClose);

    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    newDialog->setLayout(layout);

    return newDialog;
}

void TopLevelTextEditorModifier::transferTextToHackedLineEdit()
{
    hackedLineEdit->setText(topLevelLineEdit->text());
}

void TopLevelTextEditorModifier::install(QLineEdit *lineEdit, bool finishEditorPressingReturnKey)
{
    if (!lineEdit)
        return;

    hackedLineEdit = lineEdit;

    hackedLineEdit->installEventFilter(this);

    this->finishPressingReturnKey = finishEditorPressingReturnKey;

    topLevelLineEdit = new QLineEdit();
    topLevelLineEdit->setObjectName(hackedLineEdit->objectName());
    topLevelLineEdit->setValidator(hackedLineEdit->validator());

    QObject::connect(topLevelLineEdit, &QLineEdit::returnPressed, [=]{
        if (dialog) {
            if (finishPressingReturnKey) {
                topLevelLineEdit->clearFocus(); //force the focusOut event
            }
            else { // transfer data to hacked line edit when return is pressed
                transferTextToHackedLineEdit();
                hackedLineEdit->returnPressed();

                topLevelLineEdit->clear();
            }
         }
    });

    topLevelLineEdit->installEventFilter(this);
}

void TopLevelTextEditorModifier::showDialog()
{
    if (!dialog) {
        dialog.reset(createDialog());
        dialog->layout()->addWidget(topLevelLineEdit); // topLevelLineEdit will be owned by Dialog
    }

    topLevelLineEdit->setAlignment(hackedLineEdit->alignment());
    topLevelLineEdit->setText(hackedLineEdit->text());

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
    else if (obj == topLevelLineEdit && ev->type() == QEvent::FocusOut) {
        if (dialog) {
            dialog->hide();
            transferTextToHackedLineEdit();
        }
    }

    return false;
}
