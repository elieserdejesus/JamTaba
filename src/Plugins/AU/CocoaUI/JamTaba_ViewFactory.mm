#import "JamTaba_ViewFactory.h"
#import "JamTaba_UIView.h"

#include <QMacNativeWidget>
#include <QPushButton>
#include <QApplication>
#include <QVBoxLayout>

#include <QtPlugin>
Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin);

#include <QMessageBox>
#include <QLineEdit>
void teste()
{
    QMessageBox::information(nullptr, "Teste", "testando");
}


@implementation JamTaba_ViewFactory

- (unsigned) interfaceVersion {
	return 0;
}

- (NSString *) description {
	return @"JTBA: JamTaba";
}


- (NSView *)uiViewForAudioUnit:(AudioUnit)inAU withSize:(NSSize)inPreferredSize {
    
    uiFreshlyLoadedView = [[JamTaba_UIView alloc] initWithFrame:NSMakeRect(0, 0, 800, 400)];
    
    if (!qApp)
    {
        int argc = 0;
        QApplication::instance()->setAttribute(Qt::AA_MacPluginApplication);
        (void)new QApplication(argc, 0);
    }
    
    QMacNativeWidget *nativeWidget = new QMacNativeWidget();
    nativeWidget->setPalette(QPalette(Qt::red));
    nativeWidget->setAutoFillBackground(true);
    nativeWidget->setWindowFlags(Qt::Tool); // without this flag the plugin window is not showed in AULab and Logic 9
    
    nativeWidget->clearFocus();
    nativeWidget->releaseKeyboard();
    nativeWidget->setAttribute(Qt::WA_ShowWithoutActivating);
    nativeWidget->setAttribute(Qt::WA_NativeWindow);
    nativeWidget->nativeView();
    
    QVBoxLayout *layout = new QVBoxLayout();
    QPushButton *pushButton = new QPushButton("An Embedded Qt Button!");
    pushButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // Don't use the layout rect calculated from QMacStyle.
    layout->addWidget(pushButton);
    nativeWidget->setLayout(layout);
    
    QObject::connect(pushButton, &QPushButton::clicked, teste);
    
    QLineEdit *lineEdit = new QLineEdit();
    layout->addWidget(lineEdit);
    
   
    //cast voodoo learned from http://lists.qt-project.org/pipermail/interest/2014-January/010655.html
    NSView *nativeWidgetView = (__bridge NSView *)reinterpret_cast<void*>(nativeWidget->winId());
    [nativeWidgetView setFrame:[uiFreshlyLoadedView frame]]; // full screen

    [uiFreshlyLoadedView addSubview:nativeWidgetView positioned:NSWindowAbove relativeTo:nil];
    
    nativeWidget->show();
    
    [uiFreshlyLoadedView setAU:inAU];
    
    NSView *returnView = uiFreshlyLoadedView;
    uiFreshlyLoadedView = nil;	// zero out pointer.  This is a view factory.  Once a view's been created
                                // and handed off, the factory keeps no record of it.
    
    
    return returnView;
}

@end
