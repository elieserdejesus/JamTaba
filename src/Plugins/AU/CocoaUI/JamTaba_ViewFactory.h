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

@class JamTaba_UIView;

@interface JamTaba_ViewFactory : NSObject <AUCocoaUIBase>
{
    IBOutlet JamTaba_UIView *uiFreshlyLoadedView;
}															
- (NSString *) description;	// string description of the view

@end