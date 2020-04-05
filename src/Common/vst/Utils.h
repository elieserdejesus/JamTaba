#ifndef VST_UTILS_H
#define VST_UTILS_H

#include <QString>
#include "audio/core/PluginDescriptor.h"
#include "../../VST_SDK/VST2_SDK/public.sdk/source/vst2.x/aeffectx.h"

namespace vst {

    namespace utils {

        QString getPluginVendor(AEffect *plugin);
        QString getPluginName(AEffect *plugin);
        audio::PluginDescriptor createDescriptor(AEffect *plugin, const QString &pluginPath);

    }
}

#endif // VST_UTILS_H
