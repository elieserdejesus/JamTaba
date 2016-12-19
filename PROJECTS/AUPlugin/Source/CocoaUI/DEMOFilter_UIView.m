
#import "DEMOFilter_UIView.h"

@implementation DEMOFilter_UIView

- (void)dealloc {

	[[NSNotificationCenter defaultCenter] removeObserver: self];
	
    [super dealloc];
}



- (void)setAU:(AudioUnit)inAU {
	
	mAU = inAU;

}


//- (void)drawRect:(NSRect)rect
//{
//	[mBackgroundColor set];
//	NSRectFill(rect);		// this call is much faster than using NSBezierPath, but it doesn't handle non-opaque colors
//	
//	[super drawRect: rect];	// we call super to draw all other controls after we have filled the background
//}


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
