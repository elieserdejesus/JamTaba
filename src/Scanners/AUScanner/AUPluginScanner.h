#ifndef AUPLUGINSCANNER_H
#define AUPLUGINSCANNER_H

#include "BaseScanner.h"

#include "audio/core/PluginDescriptor.h"

#include <AudioUnit/AudioUnit.h>

class AUPluginScanner : public BaseScanner
{

public:
    AUPluginScanner();
    ~AUPluginScanner();

protected:

    void scan() override;

    void initialize(int argc, char *argv[]) override;

private:
    static QString osTypeToString(OSType type);

};

#endif // AUPLUGINSCANNER_H
