#ifndef VST_UTILS_H
#define VST_UTILS_H

#include <QString>
#include "audio/core/PluginDescriptor.h"
#include "aeffectx.h"

namespace Vst {

    namespace utils {

        QString getPluginVendor(AEffect *plugin);
        QString getPluginName(AEffect *plugin);
        Audio::PluginDescriptor createDescriptor(AEffect *plugin, const QString &pluginPath);

    }
}

#endif // VST_UTILS_H
