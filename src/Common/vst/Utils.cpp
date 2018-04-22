#include "Utils.h"


QString vst::utils::getPluginVendor(AEffect *plugin)
{
    if (!plugin)
        return QString();

    char temp[128]; //kVstMaxVendorStrLen]; // some dumb plugins don't respect kVstMaxVendorStrLen
    plugin->dispatcher(plugin, effGetVendorString, 0, 0, temp, 0);

    return QString::fromUtf8(temp);
}

QString vst::utils::getPluginName(AEffect *plugin)
{
    if (!plugin)
        return QString();

    char temp[128]; //kVstMaxEffectNameLen]; // some dumb plugins don't respect kVstMaxEffectNameLen
    plugin->dispatcher(plugin, effGetEffectName, 0, 0, temp, 0);

    return QString::fromUtf8(temp);
}


audio::PluginDescriptor vst::utils::createDescriptor(AEffect *plugin, const QString &pluginPath)
{
    auto pluginName = vst::utils::getPluginName(plugin);
    auto manufacturer = vst::utils::getPluginVendor(plugin);
    auto category = audio::PluginDescriptor::VST_Plugin;
    return audio::PluginDescriptor(pluginName, category, manufacturer, pluginPath);
}

