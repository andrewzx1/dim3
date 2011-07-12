/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: App Store Code

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#ifdef D3_OS_MAC
//	#include <mach-o/arch.h>
#endif

// Alter these defines to reflect the bundle identifier
// and bundle version

#define CHECK_BUNDLE_IDENTIFIER			"com.klinksoftware.dim3.engine"
#define CHECK_BUNDLE_VERSION			"3.0"

#include "interface.h"

/* =======================================================

      OS Support and Setup
      
======================================================= */

bool dim3_osx_appstore_main(void)
{
	char				str_identifier[256],str_version[256];
	CFStringRef			cf_str_identifier,cf_str_version;

		// first make sure the plist is OK
		// and not spoofed
		
	cf_str_identifier=(CFStringRef)CFBundleGetValueForInfoDictionaryKey(CFBundleGetMainBundle(),kCFBundleIdentifierKey);
	cf_str_version=(CFStringRef)CFBundleGetValueForInfoDictionaryKey(CFBundleGetMainBundle(),kCFBundleVersionKey);
	
	CFStringGetCString(cf_str_identifier,str_identifier,256,kCFStringEncodingMacRoman);
	CFStringGetCString(cf_str_version,str_version,256,kCFStringEncodingMacRoman);
	
	if (strcmp(str_identifier,CHECK_BUNDLE_IDENTIFIER)!=0) return(FALSE);
	if (strcmp(str_version,CHECK_BUNDLE_VERSION)!=0) return(FALSE);
	
		// start running through the checks
		
		
	
}
