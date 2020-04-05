#ifndef VSTLOADER_H
#define VSTLOADER_H

#include <QString>
#include "../../VST_SDK/VST2_SDK/public.sdk/source/vst2.x/aeffectx.h"

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
