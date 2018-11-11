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
    explicit VideoWidget(QWidget *parent, const QIcon &icon, bool activated = true);

    void setCurrentFrame(const QImage &image);

    void activate(bool status);

    inline bool isActivated() const
    {
        return activated;
    }

    void setIcon(const QIcon &icon);

signals:
    void statusChanged(bool activated);
    void visibilityChanged(bool visible);

protected:
    void paintEvent(QPaintEvent *ev) override;
    void resizeEvent(QResizeEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void hideEvent(QHideEvent *ev) override;
    void showEvent(QShowEvent *ev) override;
    bool hasHeightForWidth() const override;
    int heightForWidth(int width) const override;
    QSize minimumSizeHint() const override;

private:
    QImage currentImage;
    QImage scaledImage;
    QRect targetRect;
    qreal imageRatio;

    void updateScaledImage();

    bool activated;

    QIcon webcamIcon;

    static const int MIN_SIZE;

};

#endif // VIDEOWIDGET_H
