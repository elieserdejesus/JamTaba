#import "DEMOFilter_ViewFactory.h"
#import "DEMOFilter_UIView.h"

@implementation DEMOFilter_ViewFactory

// version 0
- (unsigned) interfaceVersion {
	return 0;
}

// string description of the Cocoa UI
- (NSString *) description {
	return @"JTBA: JamTaba";
}

// N.B.: this class is simply a view-factory,  returning a new autoreleased view each time it's called.
- (NSView *)uiViewForAudioUnit:(AudioUnit)inAU withSize:(NSSize)inPreferredSize {
    //if (![[NSBundle bundleForClass:[self class]] loadNibNamed:@"CocoaView" owner:self topLevelObjects:nil]) {
        //NSLog (@"Unable to load nib for view.");
	//	return nil;
	//}
    
    
    uiFreshlyLoadedView = [[DEMOFilter_UIView alloc] init];
    NSRect newFrame = uiFreshlyLoadedView.frame;
    newFrame.size.width = 800;
    newFrame.size.height = 600;
    uiFreshlyLoadedView.frame = newFrame;
    
    NSLog (@"Loading Cocoa view preferresSize: %f x %f", inPreferredSize.width, inPreferredSize.height);
    [uiFreshlyLoadedView setAU:inAU];
    
    NSView *returnView = uiFreshlyLoadedView;
    uiFreshlyLoadedView = nil;	// zero out pointer.  This is a view factory.  Once a view's been created
                                // and handed off, the factory keeps no record of it.
    
    
    NSLog (@"Returning view %@", returnView);
    return returnView;
}

@end
