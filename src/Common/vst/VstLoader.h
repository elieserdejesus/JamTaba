#ifndef VSTLOADER_H
#define VSTLOADER_H

#include <QString>
#include "aeffectx.h"

namespace Vst {
class VstHost;

class VstLoader
{
public:
    static AEffect *load(const QString &path, Vst::VstHost *host);
    static void unload(AEffect *effect);
};
}

#endif // VSTLOADER_H
