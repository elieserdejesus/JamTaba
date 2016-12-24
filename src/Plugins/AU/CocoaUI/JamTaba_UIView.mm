
#import "JamTaba_UIView.h"

@implementation JamTaba_UIView


-(id)init
{
    NSLog(@"VIEW INIT");
    return [super init];
}

- (void)dealloc {

	[[NSNotificationCenter defaultCenter] removeObserver: self];
	NSLog(@"VIEW DEALOC");
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
