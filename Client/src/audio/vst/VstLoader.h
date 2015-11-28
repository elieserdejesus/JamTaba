#ifndef VSTLOADER_H
#define VSTLOADER_H

#include <QString>
#include "aeffectx.h"

namespace Vst {

class Host;

class VstLoader
{
public:
    static AEffect* load(QString pluginPath, Vst::Host* host);
};

}

#endif // VSTLOADER_H
