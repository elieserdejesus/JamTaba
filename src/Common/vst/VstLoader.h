#ifndef VSTLOADER_H
#define VSTLOADER_H

#include <QString>
#include "aeffectx.h"

namespace vst {

class VstHost;

class VstLoader
{
public:
    static AEffect *load(const QString &path, vst::VstHost *host);
    static void unload(AEffect *effect);
};
}

#endif // VSTLOADER_H
