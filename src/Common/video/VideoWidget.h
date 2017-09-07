#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QIcon>


class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoWidget(QWidget *parent, bool activated = true);

    void setCurrentFrame(const QImage &image);

    void activate(bool status);

    inline bool isActivated() const
    {
        return activated;
    }

signals:
    void statusChanged(bool activated);

protected:
    void paintEvent(QPaintEvent *ev) override;

    void mouseReleaseEvent(QMouseEvent *) override;

    QSize minimumSizeHint() const override;

private:
    QImage currentImage;

    bool activated;

    QIcon webcamIcon;

};

#endif // VIDEOWIDGET_H
