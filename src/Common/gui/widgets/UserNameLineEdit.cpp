#include "UserNameLineEdit.h"
#include <QValidator>

UserNameLineEdit::UserNameLineEdit(QWidget *parent)
    :QLineEdit(parent)
{
    static QString userNamePattern("[a-zA-Z0-9_-]{1,16}"); //allowing lower and upper case letters, numbers, _ and - symbols, at least 1 character and max 16 characters.
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(QRegularExpression(userNamePattern), this);
    setValidator(validator);

    setMaxLength(16); //this maxLength need be changed if we decide support non ASCII user names

    connect(this, SIGNAL(editingFinished()), this, SLOT(updateTextAlignment()));
}

void UserNameLineEdit::focusInEvent(QFocusEvent *e)
{
    lastValidUserName = text();
    QLineEdit::focusInEvent(e);
}

void UserNameLineEdit::focusOutEvent(QFocusEvent *e)
{
    Q_UNUSED(e)

    //if the user name is invalid use the last valid user name
    QString currentName = text();
    int pos;
    if (validator()->validate(currentName, pos) != QValidator::Acceptable) {
        setText(lastValidUserName);
    }
    lastValidUserName = text();
    QLineEdit::focusOutEvent(e);
}

void UserNameLineEdit::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e)
    updateTextAlignment();
}

void UserNameLineEdit::updateTextAlignment()
{
    Qt::Alignment alignment = Qt::AlignCenter;
    int maxWidth = sizeHint().width();
    int textWidth = fontMetrics().width(text());
    if (textWidth > maxWidth) {
        alignment = Qt::AlignLeft | Qt::AlignVCenter;
        if (!hasFocus()) {
            setCursorPosition(0); //ensure the first letters are visible when left aligned (big names) and focus is losted
        }
    }
    setAlignment(alignment);
}
