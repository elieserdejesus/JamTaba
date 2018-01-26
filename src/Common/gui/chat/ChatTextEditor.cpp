#include "ChatTextEditor.h"
#include <QDebug>
#include <QTextBlock>
#include <QLayout>

ChatTextEditor::ChatTextEditor(QWidget *parent) :
    QTextBrowser(parent),
    bestWidth(50)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

    setReadOnly(true);
    setFrameShape(QFrame::NoFrame);
    setOpenExternalLinks(true);
    viewport()->setAutoFillBackground(false); // transparent

    connect(this, &ChatTextEditor::textChanged, this, &ChatTextEditor::updateWidth);
}

void ChatTextEditor::updateWidth()
{
    bestWidth = computeBestWidth();

    document()->setTextWidth(bestWidth);

    updateGeometry();
}

quint32 ChatTextEditor::computeBestWidth() const
{
    int maxBlockWidth = 0;
    auto block = document()->begin();
    while (block != document()->end()) {
        int w = fontMetrics().width(block.text()) + 30;
        if (w > maxBlockWidth)
            maxBlockWidth = w;

        block = block.next();
    }

    return qMin(maxBlockWidth, maximumWidth());
}

QSize ChatTextEditor::sizeHint() const
{
    return QSize(bestWidth, document()->size().height());
}

QSize ChatTextEditor::minimumSizeHint() const
{
    return sizeHint();
}
