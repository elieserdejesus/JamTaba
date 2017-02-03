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
      hideDialogWhenReturnIsPressed(false)
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

void TopLevelTextEditorModifier::install(QComboBox *comboBox)
{
    if (!comboBox) {
        qCritical() << "comboBox is null!";
        return;
    }

    if (!comboBox->isEditable()) {
        qCritical() << "comboBox is not editable!";
        return;
    }

    doInstall(comboBox->lineEdit());

    comboBox->installEventFilter(this);

    hideDialogWhenReturnIsPressed = true;

    dialogObjectName = "panelCombos";

}

void TopLevelTextEditorModifier::doInstall(QLineEdit *lineEdit)
{
    if (!lineEdit) {
        qCritical() << "lineEdit is null!";
        return;
    }

    this->hackedLineEdit = lineEdit;

    this->topLevelLineEdit = new QLineEdit();
    this->topLevelLineEdit->setValidator(hackedLineEdit->validator());
    this->topLevelLineEdit->installEventFilter(this);

    connect(topLevelLineEdit, &QLineEdit::returnPressed, [=]{
        if (dialog) {
            if (hideDialogWhenReturnIsPressed) {
                topLevelLineEdit->clearFocus(); //force the focusOut event
            }
            else { // transfer data to hacked line edit when return is pressed
                transferTextToHackedLineEdit();
                topLevelLineEdit->clear();
            }

            hackedLineEdit->returnPressed();
        }
    });
}

void TopLevelTextEditorModifier::install(QLineEdit *lineEdit, bool hideWhenReturnIsPressed, const QString &dialogObjectName)
{

    doInstall(lineEdit);

    this->hackedLineEdit->installEventFilter(this);

    this->hideDialogWhenReturnIsPressed = hideWhenReturnIsPressed;

    this->dialogObjectName = dialogObjectName; // this objectName will be used in QDialog

    this->topLevelLineEdit->setObjectName(hackedLineEdit->objectName());
}

void TopLevelTextEditorModifier::showDialog()
{
    if (!dialog) {
        dialog.reset(createDialog());
        dialog->layout()->addWidget(topLevelLineEdit); // topLevelLineEdit will be owned by Dialog
        if (!dialogObjectName.isEmpty())
            dialog->setObjectName(dialogObjectName);
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

bool TopLevelTextEditorModifier::isValidFocusInEvent(QEvent *ev) const
{
    bool hackingComboBox = qobject_cast<QComboBox*>(hackedLineEdit->parentWidget()) != nullptr;
    if (!hackingComboBox) {
        return ev->type() == QEvent::FocusIn;
    }


    // hacking a combo box
    if (ev->type() == QEvent::FocusIn) {
        QFocusEvent *focusEvent = static_cast<QFocusEvent *>(ev);
        if (focusEvent && focusEvent->reason() == Qt::MouseFocusReason) { // we are interested only in focusIn when user is clicking
            QComboBox* combo = qobject_cast<QComboBox*>(hackedLineEdit->parentWidget());
            if (combo) {
                QPoint mousePos = combo->mapFromGlobal(QCursor::pos());
                return combo->lineEdit()->rect().contains(mousePos); // check if click was in lineEdit, not in combo box drop down arrow
            }
        }
    }

    return false;
}

bool TopLevelTextEditorModifier::eventFilter(QObject *obj, QEvent *ev)
{
    if (isValidFocusInEvent(ev)) {
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
