
#import "JamTaba_UIView.h"

@implementation JamTaba_UIView

-(id)init
{
//    [NSEvent addLocalMonitorForEventsMatchingMask:NSKeyDownMask handler:^(NSEvent *incomingEvent) {
//        NSEvent *result = incomingEvent;
//        NSLog(@"evento %f:", result.keyCode);
//        return result;
//    }];
    
    return [super init];
}

- (void)dealloc {

	[[NSNotificationCenter defaultCenter] removeObserver: self];
    [super dealloc];
}

- (void)setAU:(AudioUnit)inAU {
	
	mAU = inAU;

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
