/*
     File: Filter.h
 Abstract: Filter.h
  Version: 1.0
 
*/

#define kNumberOfResponseFrequencies 512


// Here we define a custom property so the view is able to retrieve the current frequency
// response curve.  The curve changes as the filter's cutoff frequency and resonance are
// changed...

// custom properties id's must be 64000 or greater
// see <AudioUnit/AudioUnitProperties.h> for a list of Apple-defined standard properties
//
enum
{
	kAudioUnitCustomProperty_FilterFrequencyResponse = 65536
};

// We'll define our property data to be a size kNumberOfResponseFrequencies array of structs
// The UI will pass in the desired frequency in the mFrequency field, and the Filter AU
// will provide the linear magnitude response of the filter in the mMagnitude field
// for each element in the array.
typedef struct FrequencyResponse
{
	Float64		mFrequency;
	Float64		mMagnitude;
} FrequencyResponse;
