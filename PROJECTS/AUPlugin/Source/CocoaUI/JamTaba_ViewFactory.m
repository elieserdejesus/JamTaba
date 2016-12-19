#import "JamTaba_ViewFactory.h"
#import "JamTaba_UIView.h"

@implementation JamTaba_ViewFactory

- (unsigned) interfaceVersion {
	return 0;
}

- (NSString *) description {
	return @"JTBA: JamTaba";
}

- (NSView *)uiViewForAudioUnit:(AudioUnit)inAU withSize:(NSSize)inPreferredSize {
    
    uiFreshlyLoadedView = [[JamTaba_UIView alloc] init];
    NSRect newFrame = uiFreshlyLoadedView.frame;
    newFrame.size.width = 800;
    newFrame.size.height = 600;
    uiFreshlyLoadedView.frame = newFrame;
    
    [uiFreshlyLoadedView setAU:inAU];
    
    NSView *returnView = uiFreshlyLoadedView;
    uiFreshlyLoadedView = nil;	// zero out pointer.  This is a view factory.  Once a view's been created
                                // and handed off, the factory keeps no record of it.
    
    
    return returnView;
}

@end
