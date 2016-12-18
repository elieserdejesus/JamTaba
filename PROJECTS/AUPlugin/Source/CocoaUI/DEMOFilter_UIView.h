/*
     File: DEMOFilter_UIView.h
 Abstract: DEMOFilter_UIView.h
  Version: 1.0
 
*/


#import <Cocoa/Cocoa.h>
#import <AudioUnit/AudioUnit.h>
#import <AudioToolbox/AudioToolbox.h>

#import "DEMOFilter_GraphView.h"
#import "Filter.h"

/************************************************************************************************************/
/* NOTE: It is important to rename ALL ui classes when using the XCode Audio Unit with Cocoa View template	*/
/*		 Cocoa has a flat namespace, and if you use the default filenames, it is possible that you will		*/
/*		 get a namespace collision with classes from the cocoa view of a previously loaded audio unit.		*/
/*		 We recommend that you use a unique prefix that includes the manufacturer name and unit name on		*/
/*		 all objective-C source files. You may use an underscore in your name, but please refrain from		*/
/*		 starting your class name with an undescore as these names are reserved for Apple.					*/
/************************************************************************************************************/

@interface DEMOFilter_UIView : NSView
{	
	IBOutlet DEMOFilter_GraphView	*graphView;
	IBOutlet NSTextField				*cutoffFrequencyField;
	IBOutlet NSTextField				*resonanceField;
	
    // Other Members
    AudioUnit 				mAU;
	AUEventListenerRef		mAUEventListener;
	
	FrequencyResponse 	   *mData;				// the data used to draw the filter curve
	NSColor				   *mBackgroundColor;	// the background color (pattern) of the view
}

#pragma mark ____ PUBLIC FUNCTIONS ____
- (void)setAU:(AudioUnit)inAU;

#pragma mark ____ INTERFACE ACTIONS ____
- (IBAction) cutoffFrequencyChanged:(id)sender;
- (IBAction) resonanceChanged:(id)sender;

#pragma mark ____ PRIVATE FUNCTIONS
- (void)priv_synchronizeUIWithParameterValues;
- (void)priv_addListeners;
- (void)priv_removeListeners;

#pragma mark ____ LISTENER CALLBACK DISPATCHEE ____
- (void)priv_eventListener:(void *) inObject event:(const AudioUnitEvent *)inEvent value:(Float32)inValue;
@end
