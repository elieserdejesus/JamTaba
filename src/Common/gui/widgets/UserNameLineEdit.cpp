#include "UserNameLineEdit.h"
#include <QValidator>

UserNameLineEdit::UserNameLineEdit(QWidget *parent)
    :QLineEdit(parent)
{
    static QString userNamePattern("[a-zA-Z0-9_-]{2,}"); //allowing lower and upper case letters, numbers, _ and - symbols, at least 2 characters.
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(QRegularExpression(userNamePattern), this);
    setValidator(validator);
}

void UserNameLineEdit::focusInEvent(QFocusEvent *e)
{
    lastValidUserName = text();
    QLineEdit::focusInEvent(e);
}

void UserNameLineEdit::focusOutEvent(QFocusEvent *e)
{
    //if the user name is invalid use the last valid user name
    QString currentName = text();
    int pos;
    if (validator()->validate(currentName, pos) != QValidator::Acceptable) {
        setText(lastValidUserName);
    }
    lastValidUserName = text();
    QLineEdit::focusOutEvent(e);
}
