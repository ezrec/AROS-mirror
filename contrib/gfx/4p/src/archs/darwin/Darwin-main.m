#import "SDL.h"
#import <Cocoa/Cocoa.h>
#import <sys/param.h>
#import <unistd.h>

@interface SDLMain : NSObject
{
}
- (IBAction)quit:(id)sender;
@end

static int gArgc;
static char **gArgv;

@implementation SDLMain
    - (void) quit:(id)sender
    {
	SDL_Event event;
	event.type = SDL_QUIT;
	SDL_PushEvent( &event );
    }

    - (void) setupWorkingDirectory
    {
	char parentDir[ MAXPATHLEN + 1 ];
	char *ch;

	strncpy( parentDir, gArgv[ 0 ], MAXPATHLEN );
	ch = parentDir;

	while ( *ch != '\0' ) {
	    ++ch;
	}

	while ( *ch != '/' ) {
	    --ch;
	}

	*ch = '\0';

	assert( chdir( parentDir ) == 0 );
	assert( chdir( "../../../" ) == 0 );
    }

    - (void) applicationDidFinishLaunching:(NSNotification*)note
    {
	int status;
	[ self setupWorkingDirectory ];

	status = SDL_main( gArgc, gArgv );
	exit( status );
    }

@end

    int
    ObjectiveMain( int argc, char** argv )
    {
	int ii;

	if ( argc >= 2 && strncmp( argv[1], "-psn", 4 ) == 0 ) {
	    gArgc = 1;
	} else {
	    gArgc = argc;
	}

	gArgv = (char**)malloc( sizeof(*gArgv) * (gArgc+1) );
	assert( gArgv != NULL );
	for ( ii=0; ii < gArgc; ++ii ) {
	    gArgv[ ii ] = argv[ ii ];
	}

	gArgv[ ii ] = NULL;

	NSApplicationMain( argc, argv );

	return 0;
    }
