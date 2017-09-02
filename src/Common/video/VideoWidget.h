#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>

class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoWidget(QWidget *parent);

    void setCurrentFrame(const QImage &image);

protected:
    void paintEvent(QPaintEvent *ev) override;

    inline QSize minimumSizeHint() const override
    {
        return QSize(120, 90);
    }

private:
    QImage currentImage;

};

#endif // VIDEOWIDGET_H
