#import "JamTaba_UIView.h"

@implementation JamTaba_UIView

-(id)init
{
    mainWindow = nullptr;
    return [super init];
}

- (void)dealloc {

    if (mainWindow) {
        // ensuring json config file will be saved when AU view is closed
        auto controller = mainWindow->getMainController();
        controller->storeWindowSettings(false, QPointF(), mainWindow->size());
        controller->saveLastUserSettings(mainWindow->getInputsSettings());
        mainWindow = nullptr;
    }
    
	[[NSNotificationCenter defaultCenter] removeObserver: self];
    [super dealloc];
}


- (void)setAudioUnit:(AudioUnit)unit {
    audioUnit = unit;
}

- (void)setMainWindow:(MainWindow *)mainWindow {
    self->mainWindow = mainWindow;
}

- (BOOL) acceptsFirstResponder {
	return YES;
}


- (BOOL) becomeFirstResponder {	
	return YES;
}


- (BOOL) isOpaque {
	return YES;
}

@end
