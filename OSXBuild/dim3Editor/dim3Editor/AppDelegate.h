/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Obj-C App Delegate Header

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

#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"
#import "View.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>
{
@public
	View				*view;
	NSWindow			*window;
}

@end
