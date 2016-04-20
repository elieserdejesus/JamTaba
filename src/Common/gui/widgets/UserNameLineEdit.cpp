#include "UserNameLineEdit.h"
#include <QValidator>
#include <QKeyEvent>

UserNameLineEdit::UserNameLineEdit(QWidget *parent)
    :QLineEdit(parent)
{
    static QString userNamePattern("[a-zA-Z0-9 _-]{1,16}"); //allowing lower and upper case letters, numbers, _ and - symbols, blank space, at least 1 character and max 16 characters.
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(QRegularExpression(userNamePattern), this);
    setValidator(validator);

    setMaxLength(16); //this maxLength need be changed if we decide support non ASCII user names

    connect(this, SIGNAL(editingFinished()), this, SLOT(updateText()));
}

void UserNameLineEdit::keyPressEvent(QKeyEvent *e)
{
    QLineEdit::keyPressEvent(getModifiedEvent(e));
}

void UserNameLineEdit::keyReleaseEvent(QKeyEvent *e)
{
    QLineEdit::keyReleaseEvent(getModifiedEvent(e));
}

QKeyEvent *UserNameLineEdit::getModifiedEvent(QKeyEvent *e)
{
    if (e->key() != Qt::Key_Space)
        return e;

    //replace spacebar with '_'
    e->accept();
    return new QKeyEvent(e->type(), Qt::Key_Underscore, e->modifiers(), "_");
}

void UserNameLineEdit::updateText()
{
    sanitizeBlankSpaces();
    updateTextAlignment();
}

void UserNameLineEdit::sanitizeBlankSpaces()
{
    QString currentText = text();
    setText( currentText.replace(" ", "_") );
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
    int maxWidth = size().width();
    int textWidth = fontMetrics().width(text());
    if (textWidth > maxWidth) {
        alignment = Qt::AlignLeft | Qt::AlignVCenter;
        if (!hasFocus()) {
            setCursorPosition(0); //ensure the first letters are visible when left aligned (big names) and focus is losted
        }
    }
    setAlignment(alignment);
}
