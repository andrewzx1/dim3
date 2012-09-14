#import "AppWindow.h"

@implementation AppWindow


-(id)init
{
	fprintf(stdout,"IN WINDOW INIT\n");
	return(self);
}

-(void)dealloc
{
	fprintf(stdout,"IN WINDOW CLOSE\n");
	[super dealloc];
}

-(void)display
{
	[super display];
	fprintf(stdout,"IN DISPLAY\n");
}


@end
