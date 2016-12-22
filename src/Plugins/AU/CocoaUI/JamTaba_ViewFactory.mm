#import "JamTaba_ViewFactory.h"
#import "JamTaba_UIView.h"

#include <QMacNativeWidget>
#include <QApplication>
#include <QVBoxLayout>
#include "MainWindowPlugin.h"
#include "JamTaba.h"

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
    
    uiFreshlyLoadedView = [[JamTaba_UIView alloc] init];
    
    if (!qApp)
    {
        int argc = 0;
        QApplication::setApplicationName("Jamtaba 2");
        QApplication::setAttribute(Qt::AA_MacPluginApplication);
        QApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
        (void)new QApplication(argc, 0);
    }
    
    QMacNativeWidget *nativeWidget = new QMacNativeWidget();
    nativeWidget->setWindowFlags(Qt::Tool); // without this flag the plugin window is not showed in AULab and Logic 9
    
    nativeWidget->clearFocus();
    nativeWidget->releaseKeyboard();
    nativeWidget->setAttribute(Qt::WA_ShowWithoutActivating);
    nativeWidget->setAttribute(Qt::WA_NativeWindow);
    
    nativeWidget->nativeView();
    
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    
    
    JamTaba* jamtaba = new JamTaba(inAU);
    if (!jamtaba->isRunning()) {
        Configurator *configurator = Configurator::getInstance();
        if (!configurator->setUp())
            qCWarning(jtConfigurator) << "JTBConfig->setUp() FAILED !";
        jamtaba->initialize();
    }
    
    MainControllerPlugin *mainController = jamtaba->getController();
    MainWindowPlugin* mainWindow = new MainWindowPlugin(mainController);
    mainController->setMainWindow(mainWindow);
    mainWindow->initialize();

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
    uiFreshlyLoadedView = nil;	// zero out pointer.  This is a view factory.  Once a view's been created
                                // and handed off, the factory keeps no record of it.
    
    
    return returnView;
}

@end
