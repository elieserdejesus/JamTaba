#ifndef VST_PLUGIN_CHECKER_H
#define VST_PLUGIN_CHECKER_H

#include <QString>

namespace Vst {


//PluginChecker is implemented in WindowsPluginChecker and MacPluginChecker files. Only the correct file is include in .pro project file.

class PluginChecker{
public:
    static bool isValidPluginFile(const QString &pluginPath);
};

}//namespace

#endif
