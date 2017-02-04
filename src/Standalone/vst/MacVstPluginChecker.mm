#include "VstPluginChecker.h"
#include <QFileInfo>
#include <QFile>
#include <QDebug>

#import <Cocoa/Cocoa.h>

class MacBundleArchChecker
{
public:
    static bool bundleArchIs64Bits(const QFileInfo bundleFile)
    {
        QString rawPath = bundleFile.absoluteFilePath();
        NSString *fullPath = [NSString stringWithCString:rawPath.toStdString().c_str()
                                           encoding:[NSString defaultCStringEncoding]];

        NSBundle *bundle = bundle = [NSBundle bundleWithPath:fullPath];

        NSArray *archs = [bundle executableArchitectures];
        int archsSize = [archs count];
        for (int i = 0; i < archsSize; ++i) {
            NSNumber *arch = [archs objectAtIndex:i];
            if ([arch intValue] == NSBundleExecutableArchitectureX86_64) {
                return true;
            }
        }

        return false;
    }
};

bool Vst::PluginChecker::isValidPluginFile(const QString &path)
{
    QFileInfo file(path);
    if (file.isBundle() && file.absoluteFilePath().endsWith(".vst")) {
        return MacBundleArchChecker::bundleArchIs64Bits(file);
    }

    return false;
}
