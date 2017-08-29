#include "ChatTextEditor.h"
#include <QDebug>

ChatTextEditor::ChatTextEditor(QWidget *parent) :
    QTextBrowser(parent)
{
    setSizeAdjustPolicy(QTextEdit::AdjustToContentsOnFirstShow);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    setReadOnly(true);
    setFrameShape(QFrame::NoFrame);
    setOpenExternalLinks(true);
}

int ChatTextEditor::getMinimumHeight() const
{
    int documentHeight = document()->size().height();
    return documentHeight + contentsMargins().top() + contentsMargins().bottom();
}

QSize ChatTextEditor::sizeHint() const
{
    QSize minimumSize = QTextBrowser::sizeHint();
    int h = getMinimumHeight();
    int w = minimumSize.width();
    return QSize(w, h);
}

QSize ChatTextEditor::minimumSizeHint() const
{
    return sizeHint();
}
