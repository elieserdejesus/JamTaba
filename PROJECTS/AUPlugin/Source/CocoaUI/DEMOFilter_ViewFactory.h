/*
     File: DEMOFilter_ViewFactory.h
 Abstract: DEMOFilter_ViewFactory.h
  Version: 1.0
 
*/

#import <Cocoa/Cocoa.h>
#import <AudioUnit/AUCocoaUIView.h>

/************************************************************************************************************/
/* NOTE: It is important to rename ALL ui classes when using the XCode Audio Unit with Cocoa View template	*/
/*		 Cocoa has a flat namespace, and if you use the default filenames, it is possible that you will		*/
/*		 get a namespace collision with classes from the cocoa view of a previously loaded audio unit.		*/
/*		 We recommend that you use a unique prefix that includes the manufacturer name and unit name on		*/
/*		 all objective-C source files. You may use an underscore in your name, but please refrain from		*/
/*		 starting your class name with an undescore as these names are reserved for Apple.					*/
/************************************************************************************************************/

@class DEMOFilter_UIView;

@interface DEMOFilter_ViewFactory : NSObject <AUCocoaUIBase>
{
    IBOutlet DEMOFilter_UIView *uiFreshlyLoadedView;	// This class is the File's Owner of the CocoaView nib
}															// This data member needs to be the same class as the view class the factory will return

- (NSString *) description;	// string description of the view

@end