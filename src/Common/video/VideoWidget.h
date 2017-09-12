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
    void visibilityChanged(bool visible);

protected:
    void paintEvent(QPaintEvent *ev) override;
    void resizeEvent(QResizeEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void hideEvent(QHideEvent *ev) override;
    void showEvent(QShowEvent *ev) override;

    QSize sizeHint() const override;

    QSize minimumSizeHint() const override;

private:
    QImage currentImage;
    QImage scaledImage;
    QRect targetRect;

    void updateScaledImage();

    bool activated;

    QIcon webcamIcon;

};

#endif // VIDEOWIDGET_H
