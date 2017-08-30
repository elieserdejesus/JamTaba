#ifndef _JTBA_CAMERA_H
#define _JTBA_CAMERA_H

#include <QPixmap>

class Camera
{

public:
    virtual ~Camera() {}

    virtual QPixmap getFrame() const = 0;
};

#endif
