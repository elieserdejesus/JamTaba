ReadMe for FilterDemo
---------------------

FilterDemo project will build a simple Effect Audio Unit with a Cocoa Custom View (UI). The effect is a simple resonant low-pass filter which has two parameters: cutoff frequency and resonance. It demonstrates how to implement a custom property for communicating information between the Audio Unit and its view. 
Also, it shows how to publish factory presets. The Cocoa view features a resizable real-time display of the frequency-response curve which can be directly manipulated through a control point.

There are two targets in the FilterDemo project:
FilterDemo - builds both the Audio Unit component and the CocoaUI bundle
CocoaUI	   - builds just the CocoaUI bundle

Note:
The implementation subclasses the AUEffectBase class which assumes that the effect processes
the same number of input channels as output channels (n->n). Furthermore, AUEffectBase assumes that the processing will occur independently on each of these channels.  This may not be appropriate for some kinds of effects which require access to all channels at the same time (stereo-locked compressors, cross-coupling reverbs).  For these types of effects it is better to subclass AUBase, and override the Render() method.

Sample Requirements
-------------------
This sample project requires:
	
	Mac OS X v10.9 or later
	Xcode 5.0 or later
	
Feedback
--------
To send feedback to Apple about this sample project, use the feedback form at 
this location:

	http://developer.apple.com/contact/

Copyright (C) 2013 Apple Inc. All rights reserved.