#ifndef VSTLOADER_H
#define VSTLOADER_H

#include <QString>
#include "aeffectx.h"

namespace Vst {
class Host;

class VstLoader
{
public:
    static AEffect *load(QString path, Vst::Host *host);
    static void unload(AEffect *effect);
};
}

#endif // VSTLOADER_H
