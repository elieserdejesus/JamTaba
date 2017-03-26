#import "JamTaba_UIView.h"

@implementation JamTaba_UIView

-(id)init
{
    return [super init];
}

- (void)dealloc {

	[[NSNotificationCenter defaultCenter] removeObserver: self];
    [super dealloc];
}


- (void)setAudioUnit:(AudioUnit)unit {
    audioUnit = unit;
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
