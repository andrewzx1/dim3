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
				
	
		// initialize application

	if (!main_app_initialize()) {
		[NSApp terminate:self];
		return self;
	}
	
		// init the view
		
	self=[super initWithFrame:frame];
    if (self==nil) return(nil);

		// openGL context
		
	NSOpenGLPixelFormat *pf=[[NSOpenGLPixelFormat alloc] initWithAttributes:pf_attrib]; 
		
	ctx=[[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil];
	[ctx makeCurrentContext];
	
	[pf release];
	
		// initialize window
		
	main_wind_initialize();
    
    return self;
}

-(void)dealloc
{
		// shutdown
		
	main_wind_shutdown();
	main_app_shutdown();
	
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
	fprintf(stdout,"GOT DRAW\n");
//	main_wind_draw();
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
