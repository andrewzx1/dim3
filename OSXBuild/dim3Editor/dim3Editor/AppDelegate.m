#import "AppDelegate.h"

@implementation AppDelegate

@synthesize window;

-(void)dealloc
{
    [super dealloc];
}

-(void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	int			wid,high;
	NSRect		deskTopRect;
	
		// get desktop window
				
	deskTopRect=[[NSScreen mainScreen] visibleFrame];
	wid=deskTopRect.size.width;
	high=deskTopRect.size.height;
	
		// size window to it
		
	[self.window setFrame:NSMakeRect(0,high,wid,high) display:NO];
	
		// menus don't auto-enable
		
	[[NSApp mainMenu] setAutoenablesItems:NO];
}

@end
