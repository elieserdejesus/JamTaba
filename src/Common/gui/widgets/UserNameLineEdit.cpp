#include "UserNameLineEdit.h"
#include <QValidator>

UserNameLineEdit::UserNameLineEdit(QWidget *parent)
    :QLineEdit(parent)
{
    static QString userNamePattern("[a-zA-Z0-9 _-]{2,}"); //allowing lower and upper case letters, numbers, blank space, _ and - symbols, at least 2 characters.
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(QRegularExpression(userNamePattern), this);
    setValidator(validator);

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
