
#import "JamTaba_UIView.h"
#include "MainWindowPlugin.h"

@implementation JamTaba_UIView


-(id)init
{
        
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
