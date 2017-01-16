#include "AUPluginScanner.h"
#include "log/Logging.h"

AUPluginScanner::AUPluginScanner() :
    BaseScanner()
{
    qCDebug(jtStandalonePluginFinder) << "Creating Audio Unit (AU) plugin scanner!";
}

AUPluginScanner::~AUPluginScanner()
{

}

QString AUPluginScanner::osTypeToString (OSType type)
{
    const wchar_t s[4] = { (wchar_t) ((type >> 24) & 0xff),
                                  (wchar_t) ((type >> 16) & 0xff),
                                  (wchar_t) ((type >> 8) & 0xff),
                                  (wchar_t) (type & 0xff) };
    return QString::fromWCharArray(s, 4);
}

void AUPluginScanner::scan()
{
    writeToProcessOutput("JT-Scanner-Starting");

    static QList<OSType> supportedAudioUnitTypes;
    supportedAudioUnitTypes << kAudioUnitType_MusicDevice;
    supportedAudioUnitTypes << kAudioUnitType_MusicEffect;
    supportedAudioUnitTypes << kAudioUnitType_Effect;
    supportedAudioUnitTypes << kAudioUnitType_MIDIProcessor;

    AudioComponent comp = nullptr;
    do {
        AudioComponentDescription desc;
        desc.componentType = OSType(0);
        desc.componentSubType = OSType(0);
        desc.componentManufacturer = OSType(0);
        desc.componentFlags = 0;
        desc.componentFlagsMask = 0;

        comp = AudioComponentFindNext(comp, &desc);
        if (comp) {

            OSStatus status = AudioComponentGetDescription(comp, &desc);
            if (status == noErr) {

                if (!supportedAudioUnitTypes.contains(desc.componentType))
                    continue; // skip unsupported types

                AudioComponentInstance instance;
                status = AudioComponentInstanceNew(comp, &instance);
                if (status == noErr) {

                    QString type(osTypeToString(desc.componentType));
                    QString subType(osTypeToString(desc.componentSubType));
                    QString manufacturer(osTypeToString(desc.componentManufacturer));

                    CFStringRef cfName;
                    status = AudioComponentCopyName(comp, &cfName);
                    if (status == noErr) {
                        QString name = QString::fromCFString(cfName);
                        QString path(type + ":" + subType + ":" + manufacturer);
                        writeToProcessOutput("JT-Scanner-Scan-Finished: " + name + ";" + path);
                    }
                    else {
                        qCritical() << "error getting name:" << status;
                    }
                }
                if((status = AudioComponentInstanceDispose(instance)) != noErr)
                    qCritical() << "error disposing:" << status;
            }
            else {
                qCritical() << "Error creating component instance: " << status;
            }

        }

    }
    while(comp);

    writeToProcessOutput("JT-Scanner-Finished");
}

void AUPluginScanner::initialize(int argc, char *argv[])
{
    Q_UNUSED(argc)
    Q_UNUSED(argv)
}

