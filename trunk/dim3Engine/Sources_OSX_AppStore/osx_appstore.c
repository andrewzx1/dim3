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

// Alter these defines to reflect the bundle identifier
// and bundle version.  This is a check to make sure
// the info plist hasn't been spoofed

#define CHECK_BUNDLE_IDENTIFIER			"com.klinksoftware.dim3.engine"
#define CHECK_BUNDLE_VERSION			"3.0"

#include "interface.h"
 
/* =======================================================

      OS AppStore checks
      
======================================================= */

bool copy_mac_address(unsigned char *mac_address)
{
    kern_return_t             kern_result;
    mach_port_t               master_port;
    CFMutableDictionaryRef    matching_dict;
    io_iterator_t             iterator;
    io_object_t               service,parent_service;
    CFDataRef                 cf_mac_address;
 
    if (IOMasterPort(MACH_PORT_NULL,&master_port)!=KERN_SUCCESS) return(FALSE);
	 
    matching_dict=IOBSDNameMatching(master_port,0,"en0");
    if (!matching_dict) return(FALSE);
	 
    if (IOServiceGetMatchingServices(master_port,matching_dict,&iterator)!=KERN_SUCCESS) return(FALSE);
	
	cf_mac_address=NULL;
	 
    while ((service=IOIteratorNext(iterator))!=0) {

        kern_result=IORegistryEntryGetParentEntry(service,kIOServicePlane,&parent_service);
        if (kern_result==KERN_SUCCESS) {
            if (cf_mac_address!=NULL) CFRelease(cf_mac_address);
            cf_mac_address=(CFDataRef)IORegistryEntryCreateCFProperty(parent_service,CFSTR("IOMACAddress"),kCFAllocatorDefault,0);
            IOObjectRelease(parent_service);
        }
 
        IOObjectRelease(iterator);
        IOObjectRelease(service);
    }
	
	if (cf_mac_address==NULL) return(FALSE);
	
	memmove(mac_address,CFDataGetBytePtr(cf_mac_address),6);
	return(TRUE);
}

/* =======================================================

      OS AppStore checks
      
======================================================= */

bool dim3_osx_appstore_check(unsigned char *mac_address)
{
	return(FALSE);
}


/* =======================================================

      OS AppStore Check
      
======================================================= */

bool dim3_osx_appstore_main(void)
{
	char				str_identifier[256],str_version[256];
	unsigned char		mac_address[6];
	CFStringRef			cf_str_identifier,cf_str_version;

		// first make sure the plist is OK
		// and not spoofed
		
	cf_str_identifier=(CFStringRef)CFBundleGetValueForInfoDictionaryKey(CFBundleGetMainBundle(),kCFBundleIdentifierKey);
	cf_str_version=(CFStringRef)CFBundleGetValueForInfoDictionaryKey(CFBundleGetMainBundle(),kCFBundleVersionKey);
	
	CFStringGetCString(cf_str_identifier,str_identifier,256,kCFStringEncodingMacRoman);
	CFStringGetCString(cf_str_version,str_version,256,kCFStringEncodingMacRoman);
	
	if (strcmp(str_identifier,CHECK_BUNDLE_IDENTIFIER)!=0) return(FALSE);
	if (strcmp(str_version,CHECK_BUNDLE_VERSION)!=0) return(FALSE);
	
		// get mac address
	
	if (!copy_mac_address(mac_address)) return(FALSE);

		// start running through the checks
		
	return(dim3_osx_appstore_check(mac_address));
}
