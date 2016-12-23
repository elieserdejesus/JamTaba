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


@implementation JamTaba_ViewFactory

- (unsigned) interfaceVersion {
	return 0;
}

- (NSString *) description {
	return @"JTBA: JamTaba";
}


- (NSView *)uiViewForAudioUnit:(AudioUnit)inAU withSize:(NSSize)inPreferredSize {
    
    if (!QApplication::instance())
    {
        int argc = 0;
        QApplication::setApplicationName("Jamtaba 2");
        QApplication::setAttribute(Qt::AA_MacPluginApplication);
        QApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
        new QApplication(argc, 0);
    }
    
    uiFreshlyLoadedView = [[JamTaba_UIView alloc] init];

    
    QMacNativeWidget *nativeWidget = new QMacNativeWidget();
    nativeWidget->setWindowFlags(Qt::Tool); // without this flag the plugin window is not showed in AULab and Logic 9
    
    nativeWidget->clearFocus();
    nativeWidget->releaseKeyboard();
    nativeWidget->setAttribute(Qt::WA_ShowWithoutActivating);
    nativeWidget->setAttribute(Qt::WA_NativeWindow);
    
    nativeWidget->nativeView();
    
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
 
    
    static JamTabaAUPlugin *auPlugin = new JamTabaAUPlugin();
    static MainWindowPlugin *mainWindow;
    static MainControllerPlugin *mainController;
    
    if (!auPlugin->isRunning()) {
        Configurator *configurator = Configurator::getInstance();
        if (!configurator->setUp())
            qCWarning(jtConfigurator) << "JTBConfig->setUp() FAILED !";
        auPlugin->initialize();
        
        mainController = auPlugin->getController();
        mainWindow = new MainWindowPlugin(mainController);
        mainController->setMainWindow(mainWindow);
        mainWindow->initialize();
    }
    
    
    
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
    
    printf("Setando listener: %d", auPlugin);
    
    UInt32 size = sizeof(void *);
    JamTabaListener *listener = auPlugin->listener;
    AudioUnitSetProperty(inAU, kAudioUnitCustomProperty_JamTabaListener, kAudioUnitScope_Global, 0, listener, size);
    
    
    uiFreshlyLoadedView = nil;	// zero out pointer.  This is a view factory.  Once a view's been created
    // and handed off, the factory keeps no record of it.

    
    return returnView;
}

@end
