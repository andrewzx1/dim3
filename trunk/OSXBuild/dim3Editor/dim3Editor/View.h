#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>

@interface View : NSView <NSWindowDelegate>
{
	int					k;
	NSOpenGLContext		*ctx;
}

-(void)openGLFlush;

@end
