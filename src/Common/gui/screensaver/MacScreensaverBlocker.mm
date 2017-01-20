#include "ScreensaverBlocker.h"

#include "log/Logging.h"

#import <IOKit/pwr_mgt/IOPMLib.h>

IOPMAssertionID assertionID;

ScreensaverBlocker::ScreensaverBlocker()
{
    CFStringRef reasonForActivity = CFSTR("JamTaba blocking screensaver");

    IOReturn success = IOPMAssertionCreateWithName(kIOPMAssertionTypeNoDisplaySleep, kIOPMAssertionLevelOn, reasonForActivity, &assertionID);

    if (success != kIOReturnSuccess)
        qCritical() << "Can´t disable screensaver.";
}

ScreensaverBlocker::~ScreensaverBlocker()
{
    IOPMAssertionRelease(assertionID);
}

void ScreensaverBlocker::update()
{

}

