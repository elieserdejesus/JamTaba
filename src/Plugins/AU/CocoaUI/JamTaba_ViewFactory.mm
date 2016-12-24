#import "JamTaba_ViewFactory.h"
#import "JamTaba_UIView.h"

#include <QMacNativeWidget>
#include <QApplication>
#include <QVBoxLayout>
#include "JamTabaAUPlugin.h"
#import "MainWindowPlugin.h"
#import "JamTabaAUPlugin.h"

#include <QtPlugin>
Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin);

#define APP_VERSION VERSION  // VERSION is defined in Configurator.h

@implementation JamTaba_ViewFactory

- (unsigned) interfaceVersion {
	return 0;
}

- (NSString *) description {
	return @"JTBA: JamTaba";
}

- (NSView *)uiViewForAudioUnit:(AudioUnit)inAU withSize:(NSSize)inPreferredSize {
    
    [self createQtEnvironment];
    
    uiFreshlyLoadedView = [[JamTaba_UIView alloc] init];
    
    JamTabaAUPlugin *auPlugin = JamTabaAUPlugin::getInstance();
    MainWindowPlugin *mainWindow = auPlugin->mainWindow;
    QMacNativeWidget *nativeWidget = auPlugin->nativeView;
    
    QVBoxLayout *layout = new QVBoxLayout(); // I tried put this layout code inside JamTabaAUPlugin, but doesn«t work :(
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(mainWindow);
    nativeWidget->setLayout(layout);

    //cast voodoo learned from http://lists.qt-project.org/pipermail/interest/2014-January/010655.html
    NSView *nativeWidgetView = (__bridge NSView *)reinterpret_cast<void*>(nativeWidget->winId());

    [uiFreshlyLoadedView addSubview:nativeWidgetView positioned:NSWindowAbove relativeTo:nil];
    NSRect frame;
    frame.size.width  = mainWindow->width();
    frame.size.height = mainWindow->height();
    [nativeWidgetView setFrame: frame];
    [uiFreshlyLoadedView setFrame: frame];
    
    nativeWidget->show();
    
    [uiFreshlyLoadedView setAU:inAU];
    
    NSView *returnView = uiFreshlyLoadedView;
    
 
    UInt32 size = sizeof(void *);
    JamTabaAudioUnitListener *listener = auPlugin->listener;
    AudioUnitSetProperty(inAU, kJamTabaSetListener, kAudioUnitScope_Global, 0, listener, size);
    
    
    uiFreshlyLoadedView = nil;	// zero out pointer.  This is a view factory.  Once a view's been created
    // and handed off, the factory keeps no record of it.

    
    return [returnView autorelease];
}

- (void) createQtEnvironment {
    
    if (!QApplication::instance())
    {
        
        Configurator *configurator = Configurator::getInstance();
        if (!configurator->setUp())
            qCWarning(jtConfigurator) << "JTBConfig->setUp() FAILED !";
        
        int argc = 0;
        QApplication::setApplicationName("Jamtaba 2");
        QApplication::setAttribute(Qt::AA_MacPluginApplication);
        QApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
        new QApplication(argc, 0);
    }
    
}

@end
