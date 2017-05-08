#include "FakeCameraView.h"

#include <QPainter>


FakeCameraView::FakeCameraView(QWidget *parent)
    : QVideoWidget(parent)
{

}

void FakeCameraView::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev);

    QPainter painter(this);
    painter.fillRect(rect(), palette().background());

    static int counter = 0;
    const QString text = QString::number(++counter % 10);// "JamTaba";

    painter.setPen(palette().text().color());
    painter.drawText(rect(), text, QTextOption(Qt::AlignCenter));
}

QSize FakeCameraView::minimumSizeHint() const
{
    return QSize(160, 120);
}
