
#include <config.h>

#include <devices/ahi.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <utility/hooks.h>

#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/ahi.h>

#include <stdio.h>
#include <math.h>

#include "version.h"

static const char version[] = "$VER: PlaySineEverywhere " VERS "\n\r";

long __oslibversion = 37;

struct Library* AHIBase = NULL;

int
PlaySineEverywhere( void );

void
SoundFunc( struct Hook*            hook,
	   struct AHIAudioCtrl*    actrl,
	   struct AHISoundMessage* sm );


int
main( int argc, char* argv[] ) {
  int rc = RETURN_OK;

  if( argc != 1 ) {
    fprintf( stderr, "Usage: %s\n", argv[ 0 ] );
    rc = RETURN_ERROR;
  }
  else {
    struct MsgPort* mp = CreateMsgPort();
    
    if( mp != NULL ) {
      struct AHIRequest* io = (struct AHIRequest *)
	CreateIORequest( mp, sizeof( struct AHIRequest ) );

      if( io != NULL ) {
	io->ahir_Version = 4;

	if( OpenDevice( AHINAME, AHI_NO_UNIT, (struct IORequest *) io, 0 )
	    == 0 ) {
	  AHIBase = (struct Library *) io->ahir_Std.io_Device;

	  rc = PlaySineEverywhere();

	  CloseDevice( (struct IORequest *) io );
	}
	else {
	  fprintf( stderr, "Unable to open '" AHINAME "' version 4.\n" );
	  rc = RETURN_FAIL;
	}
	
	DeleteIORequest( (struct IORequest *) io );
      }
      else {
	fprintf( stderr, "Unable to create IO request.\n" );
	rc = RETURN_FAIL;
      }

      DeleteMsgPort( mp );
    }
    else {
      fprintf( stderr, "Unable to create message port.\n" );
      rc = RETURN_FAIL;
    }
  }
  
  return rc;
}

int
PlaySineEverywhere( void ) {
  int   rc = RETURN_OK;
  int   sine_length = 44100 / 10;
  WORD* sine;

  sine = AllocVec( sizeof( WORD ) * sine_length, MEMF_ANY | MEMF_PUBLIC );
  
  if( sine != NULL ) {
    ULONG mode = AHI_INVALID_ID;
    int   i;

    for( i = 0; i < sine_length; ++i ) {
      sine[ i ] = (WORD) ( 32767 * sin( i * 2 * M_PI * 1000 / sine_length ) );
    }

    while( rc == RETURN_OK &&
	   ( mode = AHI_NextAudioID( mode ) ) != AHI_INVALID_ID ) {
      struct AHIAudioCtrl* actrl;
      char                 name[ 64 ];
      struct Hook          sound_hook = {
	{ NULL, NULL },
	HookEntry,
	(HOOKFUNC) SoundFunc,
	FindTask( NULL )
      };
    
      AHI_GetAudioAttrs( mode, NULL,
			 AHIDB_Name, (ULONG) &name,
			 AHIDB_BufferLen, 64,
			 TAG_DONE );
    
      printf( "Mode 0x%08lx: %s\n", mode, name );

      actrl = AHI_AllocAudio( AHIA_AudioID,   mode,
			      AHIA_MixFreq,   44100,
			      AHIA_Channels,  1,
			      AHIA_Sounds,    1,
			      AHIA_SoundFunc, (ULONG) &sound_hook,
			      AHIA_UserData,  0,
			      TAG_DONE );

      if( actrl != NULL ) {
	struct AHISampleInfo sample = {
	  AHIST_M16S,
	  sine,
	  sine_length
	};
	
	if( AHI_LoadSound( 0, AHIST_SAMPLE, &sample, actrl) == AHIE_OK ) {
	  AHI_Play( actrl,
		    AHIP_BeginChannel, 0,
		    AHIP_Sound,        0,
		    AHIP_Freq,         44100,
		    AHIP_Vol,          0x10000,
		    AHIP_Pan,          0x08000,
		    AHIP_EndChannel,   0,
		    TAG_DONE );

	  // Now, when everything is "armed", lets start processing.

	  SetSignal( 0, SIGF_SINGLE );

	  if( AHI_ControlAudio( actrl,
				AHIC_Play, TRUE,
				TAG_DONE ) == AHIE_OK ) {
	    Wait( SIGF_SINGLE );

	    AHI_ControlAudio( actrl,
			      AHIC_Play, FALSE,
			      TAG_DONE );
	  }
	  else {
	    fprintf( stderr, "Unable start playback.\n" );
	    rc = RETURN_ERROR;
	  }

	  AHI_UnloadSound( 0, actrl );
	}
	else {
	  fprintf( stderr, "Unable load sound.\n" );
	  rc = RETURN_ERROR;
	}
	
	AHI_FreeAudio( actrl );
      }
      else {
	fprintf( stderr, "Unable to allocate audio.\n" );
	rc = RETURN_ERROR;
      }
    }

    FreeVec( sine );
  }
  else {
    fprintf( stderr, "Unable to allocate memory for sine\n" );
  }

  return rc;
}

void
SoundFunc( struct Hook*            hook,
	   struct AHIAudioCtrl*    actrl,
	   struct AHISoundMessage* sm ) {
  struct Task* task = hook->h_Data;

  ++( (ULONG) actrl->ahiac_UserData );

  if( (ULONG) actrl->ahiac_UserData == 10 ) {
    AHI_SetSound( 0, 0, 0, 0, actrl, AHISF_NONE );
  }
  else if( (ULONG) actrl->ahiac_UserData == 11 ) {
    Signal( task, SIGF_SINGLE );
  }
}

