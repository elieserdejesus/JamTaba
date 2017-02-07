#include "TopLevelTextEditorModifier.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QDebug>
#include <QKeyEvent>
#include <QLineEdit>
#include <QTimer>
#include <QSharedPointer>

QList<QSharedPointer<TopLevelTextEditorModifier>> TopLevelTextEditorModifier::createdModifiers;

/**

 This class is showing a QDialog with a QLineEdit to avoid hosts (AU and VST) eating keystrokes.
 When the text input field is inside a toplevel window (like a QDialog) the keystrokes are working ok.

*/

TopLevelTextEditorModifier::TopLevelTextEditorModifier()
    :
      topLevelLineEdit(nullptr),
      hackedLineEdit(nullptr),
      hideDialogWhenReturnIsPressed(false)
{
    TopLevelTextEditorModifier::createdModifiers.append(QSharedPointer<TopLevelTextEditorModifier>(this));
}

TopLevelTextEditorModifier::~TopLevelTextEditorModifier()
{
    //
}

void TopLevelTextEditorModifier::transferTextToHackedLineEdit()
{
    hackedLineEdit->setText(topLevelLineEdit->text());
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
    this->topLevelLineEdit->setAttribute(Qt::WA_MacShowFocusRect, 0); // remove border focus on Mac
    Qt::WindowFlags flags = Qt::Window | Qt::WindowStaysOnTopHint | Qt::NoDropShadowWindowHint;
#ifdef Q_OS_MAC
    flags |= Qt::Tool | Qt::FramelessWindowHint;
#else
    flags |= Qt::Popup; // Popup works nice in Windows, the focus is handled correctly
#endif
     this->topLevelLineEdit->setWindowFlags(flags);

    connect(topLevelLineEdit, &QLineEdit::returnPressed, [=]{

        if (hideDialogWhenReturnIsPressed) {
            topLevelLineEdit->clearFocus(); //force the focusOut event
        }
        else { // transfer data to hacked line edit when return is pressed
            transferTextToHackedLineEdit();
            topLevelLineEdit->clear();
        }

        hackedLineEdit->returnPressed();

    });
}

void TopLevelTextEditorModifier::modify(QComboBox *comboBox)
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

    topLevelLineEdit->setObjectName(comboBox->objectName() + "-topLevel");

    comboBox->installEventFilter(this);

    hideDialogWhenReturnIsPressed = true;
}

void TopLevelTextEditorModifier::modify(QLineEdit *lineEdit, bool hideWhenReturnIsPressed)
{

    doInstall(lineEdit);

    this->hackedLineEdit->setFocusPolicy(Qt::NoFocus);

    this->hackedLineEdit->installEventFilter(this);

    this->hideDialogWhenReturnIsPressed = hideWhenReturnIsPressed;

    this->topLevelLineEdit->setObjectName(hackedLineEdit->objectName() + "-topLevel");
}

void TopLevelTextEditorModifier::showDialog()
{

    topLevelLineEdit->setAlignment(hackedLineEdit->alignment());
    topLevelLineEdit->setText(hackedLineEdit->text());

    topLevelLineEdit->setContentsMargins(hackedLineEdit->contentsMargins());
    QString styleSheet;
    if (isHackingComboBox())
        styleSheet += "padding: 0px; margin: 0px;";

    topLevelLineEdit->setStyleSheet(styleSheet);

    topLevelLineEdit->resize(hackedLineEdit->frameSize());
    QPoint hackedLineEditTopLeft = hackedLineEdit->rect().topLeft();
    topLevelLineEdit->move(hackedLineEdit->mapToGlobal(hackedLineEditTopLeft));

    topLevelLineEdit->show();
    topLevelLineEdit->raise();
    topLevelLineEdit->activateWindow();
    topLevelLineEdit->setFocus();
}

bool TopLevelTextEditorModifier::isHackingComboBox() const
{
    if (!hackedLineEdit || !(hackedLineEdit->parentWidget()))
        return false;
    
    return qobject_cast<QComboBox*>(hackedLineEdit->parentWidget()) != nullptr;
}

bool TopLevelTextEditorModifier::isValidFocusInEvent(QEvent *ev) const
{
    if (!isHackingComboBox()) {
        return ev->type() == QEvent::MouseButtonPress && !hackedLineEdit->isReadOnly(); // user name line edit can't be edited when playing in ninjam server
    }

    // hacking a combo box
    if (ev->type() == QEvent::FocusIn) {
        QFocusEvent *focusEvent = static_cast<QFocusEvent *>(ev);
        if (focusEvent->reason() == Qt::MouseFocusReason) { // we are interested only in focusIn when user is clicking
            QComboBox* combo = qobject_cast<QComboBox*>(hackedLineEdit->parentWidget());
            if (combo) {
                QPoint mousePos = hackedLineEdit->mapFromGlobal(QCursor::pos());
                return hackedLineEdit->rect().contains(mousePos); // check if click was in lineEdit, not in combo box drop down arrow
            }
        }
    }

    return false;
}

bool TopLevelTextEditorModifier::eventFilter(QObject *obj, QEvent *ev)
{
    if (ev->type() != QEvent::FocusIn && ev->type() != QEvent::FocusOut && ev->type() != QEvent::MouseButtonPress)
        return false; // skip all other events

    // clear focus when mouse is pressed in non top level LineEdit area
    if (QApplication::focusWidget() == obj && obj == topLevelLineEdit && ev->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(ev);
        if (mouseEvent->button() == Qt::RightButton)
            return false;

        if (!topLevelLineEdit->rect().contains(mouseEvent->pos())) {
            topLevelLineEdit->clearFocus();
            return false;
        }
    }

    if (isValidFocusInEvent(ev)) {
        showDialog();
        return true;
    }
    else if (ev->type() == QEvent::FocusOut && obj == topLevelLineEdit) {
        topLevelLineEdit->hide();
        transferTextToHackedLineEdit();
        hackedLineEdit->clearFocus();
        return true;
    }

    return false;
}
