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
    
    QApplication::setApplicationName("JamTaba 2");
    QApplication::setAttribute(Qt::AA_MacPluginApplication);
    QApplication::setAttribute(Qt::AA_DontUseNativeMenuBar); // not working in qt 5.6.2
    
    Configurator *configurator = Configurator::getInstance();
    if (!configurator->setUp())
        qCWarning(jtConfigurator) << "JTBConfig->setUp() FAILED !";
    
    if (!QApplication::instance())
    {
        int argc = 0;
        new QApplication(argc, 0);
    }

    uiFreshlyLoadedView = [[JamTaba_UIView alloc] init];
    
    JamTabaAUPlugin *auPlugin = JamTabaAUPlugin::getInstance(inAU);
    
    if (!auPlugin->mainWindow) {
        MainControllerPlugin *mainController = auPlugin->getController();
        auPlugin->mainWindow = new MainWindowPlugin(mainController);
        mainController->setMainWindow(auPlugin->mainWindow);
        auPlugin->mainWindow->initialize();
        
        QMacNativeWidget *nativeWidget = new QMacNativeWidget();
        nativeWidget->setWindowFlags(Qt::Tool); // without this flag the plugin window is not showed in AULab and Logic 9
        
        nativeWidget->clearFocus();
        nativeWidget->releaseKeyboard();
        nativeWidget->setAttribute(Qt::WA_ShowWithoutActivating);
        nativeWidget->setAttribute(Qt::WA_NativeWindow);
        
        nativeWidget->nativeView();
        
        auPlugin->nativeView = nativeWidget;
        
        QVBoxLayout *layout = new QVBoxLayout(); // I tried put this layout code inside JamTabaAUPlugin, but doesn�t work :(
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(auPlugin->mainWindow);
        nativeWidget->setLayout(layout);
    }
    
    NSView *nativeWidgetView = auPlugin->nativeView->nativeView();

    [uiFreshlyLoadedView addSubview:nativeWidgetView positioned:NSWindowAbove relativeTo:nil];
    NSRect frame;
    frame.origin.x = 0;
    frame.origin.y = 0;
    frame.size.width  = auPlugin->mainWindow->width();
    frame.size.height = auPlugin->mainWindow->height();
    [nativeWidgetView setFrame: frame];
    [uiFreshlyLoadedView setFrame: frame];
    
    auPlugin->nativeView->show();
    auPlugin->mainWindow->update(); // necessary to solve a bug in Logic when the custom view is hidded (using generic view) and showed
    
    [uiFreshlyLoadedView setAU:inAU];
    
    NSView *returnView = uiFreshlyLoadedView;
    
 
    UInt32 size = sizeof(void *);
    JamTabaAudioUnitListener *listener = auPlugin->listener;
    AudioUnitSetProperty(inAU, kJamTabaSetListener, kAudioUnitScope_Global, 0, listener, size);
    
    
    uiFreshlyLoadedView = nil;	// zero out pointer.  This is a view factory.  Once a view's been created
    // and handed off, the factory keeps no record of it.

    
    return [returnView autorelease];
}


@end
