#include "driver.h"

#include <exec/exec.h>
#include <dos/dos.h>
#include <intuition/intuition.h>
#include <devices/timer.h>

#include <proto/exec.h>
#include <proto/dos.h>

#include "aros_main.h"

/************************************************************************************/

int ignorecfg = 1;

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Library *CyberGfxBase;
struct Device *TimerBase;

struct MsgPort *TimerMP;
struct timerequest *TimerIO;

/************************************************************************************/

int osd_init()
{
  return(0);
}

/************************************************************************************/

void osd_exit()
{
}

/************************************************************************************/

void osd_pause(int paused)
{
}

/************************************************************************************/

void osd_save_snapshot(void)
{
}

/************************************************************************************/

static void Cleanup(char *msg)
{
    if (msg) printf("AROS Mame: %s\n", msg);
    
    if (TimerIO)
    {
    	if (TimerBase)
	{
	    if (!CheckIO(&TimerIO->tr_node)) AbortIO(&TimerIO->tr_node);
	    WaitIO(&TimerIO->tr_node);
	    
	    CloseDevice(&TimerIO->tr_node);
	}
        DeleteIORequest(&TimerIO->tr_node);
    }
    
    if (TimerMP) DeleteMsgPort(TimerMP);
    
    if (CyberGfxBase) CloseLibrary(CyberGfxBase);
    if (GfxBase) CloseLibrary((struct Library *)GfxBase);
    if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
    
    exit(0);
}

/************************************************************************************/

static void OpenLibs(void)
{
    if (!(IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",39)))
    {
        Cleanup("Can't open intuition.library V39!");
    }
    
    if (!(GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",39)))
    {
        Cleanup("Can't open graphics.library V39!");
    }
    
    if (!(CyberGfxBase = OpenLibrary("cybergraphics.library",39)))
    {
        Cleanup("Can't open cybergraphics.library V39!");
    }
    
 }

/************************************************************************************/

static void OpenTimer(void)
{
    if (!(TimerMP = CreateMsgPort()))
    {
        Cleanup("Can't create Timer MsgPort!");
    }
    
    if (!(TimerIO = (struct timerequest *)CreateIORequest(TimerMP, sizeof(struct timerequest))))
    {
        Cleanup("Can't create Timer IO Request!");
    }
    
    if (OpenDevice("timer.device", UNIT_VBLANK, &TimerIO->tr_node, 0) != 0)
    {
        Cleanup("Can't open timer.device!");
    }
    
    TimerBase = TimerIO->tr_node.io_Device;
    
    TimerIO->tr_node.io_Message.mn_Node.ln_Type = NT_FREEMSG;
}

/************************************************************************************/

/* fuzzy string compare, compare short string against long string        */
/* e.g. astdel == "Asteroids Deluxe". The return code is the fuzz index, */
/* we simply count the gaps between maching chars.                       */
int fuzzycmp (const char *s, const char *l)
{
    int gaps = 0;
    int match = 0;
    int last = 1;

    for (; *s && *l; l++)
    {
	if (*s == *l)
	    match = 1;
	else if (*s >= 'a' && *s <= 'z' && (*s - 'a') == (*l - 'A'))
	    match = 1;
	else if (*s >= 'A' && *s <= 'Z' && (*s - 'A') == (*l - 'a'))
	    match = 1;
	else
	    match = 0;

	if (match)
	    s++;

	if (match != last)
	{
	    last = match;
	    if (!match)
		gaps++;
	}
    }

    /* penalty if short string does not completely fit in */
    for (; *s; s++)
	gaps++;

    return gaps;
}

/************************************************************************************/

int main(int argc, char **argv)
{
	int res, i, j = 0, game_index;
    char *playbackname = NULL;

    OpenLibs();
    OpenTimer();

    ignorecfg = 1;
    errorlog = options.errorlog = 0;

    game_index = -1;

    for (i = 1;i < argc;i++) /* V.V_121997 */
    {
	    if (stricmp(argv[i],"-ignorecfg") == 0) ignorecfg = 1;
	    if (stricmp(argv[i],"-log") == 0)
		    errorlog = options.errorlog = fopen("error.log","wa");

	    if (stricmp(argv[i],"-playback") == 0)
	    {
		    i++;
		    if (i < argc)  /* point to inp file name */
			    playbackname = argv[i];
            }
    }

	/* If not playing back a new .inp file */
    if (game_index == -1)
    {
        /* take the first commandline argument without "-" as the game name */
        for (j = 1; j < argc; j++)
        {
            if (argv[j][0] != '-') break;
        }
		/* do we have a driver for this? */
#ifdef MAME_DEBUG
        /* pick a random game */
        if (stricmp(argv[j],"random") == 0)
        {
#if 0
            struct timeval t;

            i = 0;
            while (drivers[i]) i++;	/* count available drivers */

            gettimeofday(&t,0);
            srand(t.tv_sec);
            game_index = rand() % i;

            printf("Running %s (%s) [press return]\n",drivers[game_index]->name,drivers[game_index]->description);
            getchar();
#endif
        }
        else
#endif
        {
            for (i = 0; drivers[i] && (game_index == -1); i++)
            {
                if (stricmp(argv[j],drivers[i]->name) == 0)
                {
                    game_index = i;
                    break;
                }
            }

            /* educated guess on what the user wants to play */
            if (game_index == -1)
            {
                int fuzz = 9999; /* best fuzz factor so far */

                for (i = 0; (drivers[i] != 0); i++)
                {
                    int tmp;
                    tmp = fuzzycmp(argv[j], drivers[i]->description);
                    /* continue if the fuzz index is worse */
                    if (tmp > fuzz)
                        continue;

                    /* on equal fuzz index, we prefer working, original games */
                    if (tmp == fuzz)
                    {
						/* game is a clone */
						if (drivers[i]->clone_of != 0
								&& !(drivers[i]->clone_of->flags & NOT_A_DRIVER))
                        {
                            /* if the game we already found works, why bother. */
                            /* and broken clones aren't very helpful either */
                            if ((!drivers[game_index]->flags & GAME_NOT_WORKING) ||
                                (drivers[i]->flags & GAME_NOT_WORKING))
                                continue;
                        }
                        else continue;
                    }

                    /* we found a better match */
                    game_index = i;
                    fuzz = tmp;
                }

                if (game_index != -1)
                    printf("fuzzy name compare, running %s\n",drivers[game_index]->name);
            }
        }

        if (game_index == -1)
        {
            printf("Game \"%s\" not supported\n", argv[j]);
            return 1;
        }
    }

    #ifdef MESS
    /* This function has been added to MESS.C as load_image() */
    load_image(argc, argv, j, game_index);
    #endif

    /* parse generic (os-independent) options */
    parse_cmdline (argc, argv, game_index);

    {	/* Mish:  I need sample rate initialised _before_ rom loading for optional rom regions */
	extern int soundcard;

	if (soundcard == 0) {    /* silence, this would be -1 if unknown in which case all roms are loaded */
	    Machine->sample_rate = 0; /* update the Machine structure to show that sound is disabled */
	    options.samplerate=0;
	}
    }

    /* handle record which is not available in mame.cfg */
    for (i = 1; i < argc; i++)
    {
	if (stricmp(argv[i],"-record") == 0)
	{
    	    i++;
	    if (i < argc)
		options.record = osd_fopen(argv[i],0,OSD_FILETYPE_INPUTLOG,1);
	}
    }

    if (options.record)
    {
        INP_HEADER inp_header;

        memset(&inp_header, '\0', sizeof(INP_HEADER));
        strcpy(inp_header.name, drivers[game_index]->name);
        /* MAME32 stores the MAME version numbers at bytes 9 - 11
         * MAME DOS keeps this information in a string, the
         * Windows code defines them in the Makefile.
         */
        /*
        inp_header.version[0] = 0;
        inp_header.version[1] = VERSION;
        inp_header.version[2] = BETA_VERSION;
        */
        osd_fwrite(options.record, &inp_header, sizeof(INP_HEADER));
    }

    #ifdef MESS
    /* Build the CRC database filename */
    sprintf(crcfilename, "%s/%s.crc", crcdir, drivers[game_index]->name);
    #endif

/* go for it */

    res = run_game (game_index);

    /* close open files */
    if (options.errorlog) fclose (options.errorlog);
    if (options.playback) osd_fclose (options.playback);
    if (options.record)   osd_fclose (options.record);

    Cleanup(0);
}

/************************************************************************************/


