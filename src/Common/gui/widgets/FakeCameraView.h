#ifndef _FAKE_CAMERA_VIEW_
#define _FAKE_CAMERA_VIEW_

#include <QVideoWidget>

class FakeCameraView : public QVideoWidget
{
    Q_OBJECT

public:

    FakeCameraView(QWidget *parent);

protected:
    void paintEvent(QPaintEvent *ev) override;

    QSize minimumSizeHint() const override;

};

#endif
