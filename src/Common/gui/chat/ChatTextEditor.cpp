#include "ChatTextEditor.h"
#include <QDebug>
#include <QTextBlock>
#include <QLayout>
#include <QRegularExpression>

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

uint ChatTextEditor::getEmojisCount() const
{
    const static QRegularExpression REGEX("<img src=\":/emoji/icons");

    return document()->toHtml().count(REGEX);
}

quint32 ChatTextEditor::computeBestWidth() const
{
    int maxBlockWidth = 0;
    auto block = document()->begin();
    while (block != document()->end()) {
        int w = fontMetrics().width(block.text()) + 30;

        w += getEmojisCount() * 22; // add emoji width (22 pixels) because emoji icon is larger than a simple character

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
