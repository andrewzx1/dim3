/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Obj-C App Delegate

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

#import "View.h"

#include "glue.h"
#include "interface.h"

@implementation View

/* =======================================================

      Initialize and Shutdown View
	        
======================================================= */

-(id)initWithFrame:(NSRect)frame
{
	NSOpenGLPixelFormatAttribute pf_attrib[]= 
				{
					NSOpenGLPFAWindow,
					NSOpenGLPFAAccelerated,
					NSOpenGLPFANoRecovery,
					NSOpenGLPFADoubleBuffer,
					NSOpenGLPFAColorSize,24,
					NSOpenGLPFAAlphaSize,8,
					NSOpenGLPFADepthSize,24,
					0
				};
	
		// init the view
		
	self=[super initWithFrame:frame];
    if (self==nil) return(nil);
	
		// openGL context
		
	NSOpenGLPixelFormat *pf=[[NSOpenGLPixelFormat alloc] initWithAttributes:pf_attrib]; 
		
	ctx=[[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil];
	[ctx makeCurrentContext];
	
	[pf release];
	
		// setup window
		
	main_wind_initialize();
	
	self->k=0;
    
    return self;
}

-(void)dealloc
{
		// shutdown the window
		
	main_wind_shutdown();
	
		// dealloc the view
		
    [super dealloc];
}

/* =======================================================

      Drawing
      
======================================================= */

-(BOOL)isOpaque
{
	return YES;
}

-(void)lockFocus
{
  [super lockFocus];
  
  if ([ctx view]!=self) [ctx setView:self];
  
  [ctx makeCurrentContext];
}

-(void)drawRect:(NSRect)dirtyRect
{
	[ctx update];
	
	k++;
	switch (self->k%3) {
		case 0:
			glClearColor(1.0f,0.0f,0.0f,1.0f);
			break;
		case 1:
			glClearColor(0.0f,1.0f,0.0f,1.0f);
			break;
		case 2:
			glClearColor(0.0f,0.0f,1.0f,1.0f);
			break;
	}
	glClear(GL_COLOR_BUFFER_BIT);
	
	[ctx flushBuffer];
}

-(void)openGLFlush
{
	[ctx flushBuffer];
}

/* =======================================================

      Events
      
======================================================= */

-(BOOL)acceptsFirstResponder
{
    return YES;
}

-(void)mouseDown:(NSEvent*)theEvent
{
	[self display];

}

-(void)keyDown:(NSEvent *)theEvent
{
	int				key;
	NSString		*chars;
	
	chars=[theEvent charactersIgnoringModifiers];
	if ([chars length]!=1) return;
	
	key=[chars characterAtIndex:0];
	
	[self display];
	
	/*
	[theEvent modifierFlags]&
	enum {
    NSAlphaShiftKeyMask = 1 << 16,
    NSShiftKeyMask      = 1 << 17,
    NSControlKeyMask    = 1 << 18,
    NSAlternateKeyMask  = 1 << 19,
    NSCommandKeyMask    = 1 << 20,
    NSNumericPadKeyMask = 1 << 21,
    NSHelpKeyMask       = 1 << 22,
    NSFunctionKeyMask   = 1 << 23,
    NSDeviceIndependentModifierFlagsMask = 0xffff0000U
};
*/
	
	// NSLeftArrowFunctionKey
	// need to call glue here
	
//	[super keyDown:theEvent];
}



@end
