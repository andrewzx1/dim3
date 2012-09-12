#import "View.h"
#include "interface.h"

@implementation View

/* =======================================================

      Initialize and Release
      
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
	
		// startup the application
	/* supergumba -- cocoa
	if (!main_app_initialize()) {
		[NSApp terminate:self];
		return self;
	}
	*/
		// openGL context
		
	NSOpenGLPixelFormat *pf=[[NSOpenGLPixelFormat alloc] initWithAttributes:pf_attrib]; 
		
	ctx=[[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil];
	[ctx makeCurrentContext];
	
	[pf release];
	
	self->k=0;
    
    return self;
}

-(void)dealloc
{
		// shutdown
		
//	main_app_shutdown();		// supergumba -- cocoa
	
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
  
  if ([ctx view]!= self) [ctx setView:self];
  
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
