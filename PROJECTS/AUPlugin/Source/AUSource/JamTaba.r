/*
     File: Filter.r 
 Abstract:  Filter.r  
  Version: 1.01
 hacked into functionality by Alex Kenis
  
*/
#include <AudioUnit/AudioUnit.r>

#include "JamTabaVersion.h"

// Note that resource IDs must be spaced 2 apart for the 'STR ' name and description
#define kAudioUnitResID_Filter				1000

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Filter~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define RES_ID			kAudioUnitResID_Filter
#define COMP_TYPE		kAudioUnitType_Effect
#define COMP_SUBTYPE	Filter_COMP_SUBTYPE
#define COMP_MANUF		Filter_COMP_MANF	

#define VERSION			kFilterVersion
#define NAME			"Jamtaba : Ninjam"
#define DESCRIPTION		"Jamtaba - Ninjam Client"
#define ENTRY_POINT		"FilterEntry"

#include "AUResources.r"