#include "Utils.h"

// namespace functions

QString Vst::utils::getPluginVendor(AEffect *plugin)
{
    if (!plugin)
        return QString();

    char temp[128];//kVstMaxVendorStrLen]; //some dumb plugins don't respect kVstMaxVendorStrLen
    plugin->dispatcher(plugin, effGetVendorString, 0, 0, temp, 0);

    return QString::fromUtf8(temp);
}

QString Vst::utils::getPluginName(AEffect *plugin)
{
    if (!plugin)
        return QString();

    char temp[128];//kVstMaxEffectNameLen]; //some dumb plugins don't respect kVstMaxEffectNameLen
    plugin->dispatcher(plugin, effGetEffectName, 0, 0, temp, 0);

    return QString::fromUtf8(temp);
}


Audio::PluginDescriptor Vst::utils::createDescriptor(AEffect *plugin, const QString &pluginPath)
{
    auto pluginName = Vst::utils::getPluginName(plugin);
    auto manufacturer = Vst::utils::getPluginVendor(plugin);
    auto category = Audio::PluginDescriptor::VST_Plugin;
    return Audio::PluginDescriptor(pluginName, category, manufacturer, pluginPath);
}

