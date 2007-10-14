/*
    Copyright © 2007, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/debug.h>
#include <exec/types.h>

#include <proto/dos.h>

#include <ctype.h>
#include <string.h>

#include "config.h"

#define TEST

/*
General options:
accuracy=n [default=2]
  Set emulator accuracy to n. The default is n = 2, which means the
  emulator will try to be as accurate as possible. This no longer
  does much in this version, and I'll probably remove it.
framerate=n [default=1]
  Sets the frame rate to 1/n. Only every nth screen will be drawn.  Using a
  higher value can speed up the emulator, at the expense of graphics quality.
autoconfig=bool [default=yes]
  If this is enabled, all expansion devices provided by the emulation will be
  automounted. You should only disable this if you have a Kickstart ROM
  earlier than 1.3 which can't cope with this. Some badly written games and
  demos might also be incompatible with this.
floppy0=file [default=df0.adf]
  Try to use the specified file as diskfile for drive 0 instead of df0.adf.
  The options floppy1, floppy2, and floppy3 also exist.
kickstart_rom_file=file [default=kick.rom]
  Use the specified file instead of kick.rom as Kickstart image.
  You can also use an 8k sized A1000 boot ROM.  The emulator will detect it
  by its small size and behave like an A1000.
kickstart_key_file=file [default=none]
  For some strange licensing reasons, Cloanto had to encrypt the ROM images
  they ship with the Amiga Forever package.  This option lets you select the
  key file; this is only necessary if you are using one of the ROMs from
  Amiga Forever.
joyport0=mode [default=mouse]
  Specify how to emulate joystick port 0. You can use "mouse", "joy0", or
  "joy1" to use the corresponding input devices of your machine, or you can
  select several different keyboard replacements for a joystick:
    "kbd1" for the numeric pad.  '0' is the fire button.  Three keys on the
           numeric pad act as autofire toggle: '.' (or ',' depending on your
           keyboard language), Enter and the division key.
    "kbd2" for the cursor keys with right control key as fire button and the
           right shift key as autofire toggle
    "kbd3" for T/F/H/B with the left Alt key as fire button and the left Shift
           key as autofire toggle.
  The autofire toggle keys will turn on autofire (25 shots per second), it
  will stay enabled until you hit the autofire toggle again.
joyport1=mode [default=joy0]
  Like joyport0, but for the Amiga's joystick port 1.
use_gui=state [default=yes]
  If enabled, show a user-interface that enables changing these options at
  run-time.  Possible settings are "no", "yes" and "nowait".  If "nowait" is
  used, the GUI will be shown at program startup, but the emulator will also
  start immediately without waiting for the user to change any of the
  settings.
immediate_blits=bool [default=no]
  If enabled, all blits will finish immediately, which can be nice for speed,
  but may cause incompatibilities.
collision_level=level [default=sprites]
  This can have a value of "none", "sprites", "playfields", or "full".  If
  set to "sprites", the emulator will only compute collisions between sprites.
  If set to "playfields", it will additionally compute collisions between
  sprites and the background.  This is much slower and used only rarely.  When
  "full" level is selected, collisions of the background with itself are also
  computed.  This is also used very rarely, and makes the emulator even slower.
  Using "sprites" or "none" will almost always be enough.
cpu_speed=speed [default=4]
  This can have a value of "real", "max", or an integer between 1 and 20.
  "real" will try to give the CPU emulation exactly as many cycles, relative
  to the other chips, as on a real A500.  "max" will try to give you the
  maximum CPU emulation speed achievable on your machine.  Numeric values
  specify a fixed relation between CPU and custom chip emulation, where lower
  values prioritize CPU emulation, while higher values prioritize custom chip
  emulation.
finegrain_cpu_speed=speed
  Like cpu_speed, but only accepts numeric values.  These numeric values are
  equivalent to the ones given to cpu_speed multiplied by 512.  This means
  that "cpu_speed=2" and "finegrain_cpu_speed=1024" have the same effect.
  Using this option you can give the CPU emulation a much higher priority by
  using small values.
cpu_type=type [default=68000]
  Controls which CPU is emulated. This can be "68000", "68010", "68020" or
  "68020/68881".  In some cases, you may need to use "68ec020" or
  "68ec020/68881" to emulate a crippled variant of the 68020 that has only a
  24 bit address bus.  Some software, including some Kickstart versions, does
  not work with a normal 68020 that has a 32 bit address bus.
  Careful: using an "ec" variant has harmful side effects, and should be
  disabled unless absolutely needed (you lose Z3 memory and Picasso
  emulation).
cpu_compatible=bool [default=no]
  If enabled, a slower but slightly more accurate variant of the CPU emulation
  will be used.  This is needed for some types of copy protection, among other
  things. This is only meaningful for a CPU type of "68000".
nr_floppies=n [default=4]
  The emulator will emulate this many external floppy drives.  Some very old
  games apparently have problems if this is larger than 1, but for all normal
  programs the default is good enough.

Emulating external devices (harddisk, CD-ROM, printer, serial port):
filesystem=access,volume:path [default=no filesystems mounted]
  Mount the host's file system at "path" as an Amiga filesystem with volume
  name "VOLUME:".  "access" can be either "ro" (for readonly), or "rw" (for
  read-write).  If you want to mount a CD-ROM, you should use a readonly
  mount.  You can mount multiple file systems.
  See below.
hardfile=access,secs,heads,reserved,bsize,file [default=no hardfiles mounted]
  Mount the hardfile "file" as an emulated harddisk, using a geometry of
  "secs" sectors per track, "heads" surfaces and "nr" reserved blocks.
  Each sector should have "bsize" bytes. This can be abused to mount
  floppy images.  You can mount multiple hardfiles.
  See below.

Sound options:
sound_output=type [default=none]
  The type of sound output can be "none" (no sound at all), "interrupts"
  (emulated for the internal side effects that can be noticed by programs,
  but no sound output), "normal" (emulated, and sound output), "exact" (a
  slightly more accurate emulation that may be necessary in some cases, but
  can also be slower).
sound_channels=type [default=mono]
  Can be "mono", "stereo", or "mixed".  If set to "stereo", the left and right
  channels produce the same output as on a real Amiga - two of the four audio
  channels are audible on the left, and the other two on the right.  This is
  exactly what you want, except if you're using headphones - having the sound
  split so strictly can lead to a strange listening experience at times.  This
  is what "mixed" is trying to solve: it mixes the left channel with a low
  volume into the right channel and vice versa.  (This mixing operation loses
  a bit of precision in the output).
sound_bits=n [default varies across UAE versions on different OS types]
  Common values are 8 (low quality) or 16 (high quality)
sound_frequency=n [default varies across UAE versions on different OS types]
  Common values are 22050 or 44100. The quality of sound output increases with
  the frequency.
sound_min_buff=n
sound_max_buff=n [default varies across UAE versions on different OS types]
  You can specify the minimum and maximum size of the sound buffer.
  Smaller buffers reduce latency.  Usually only the minimum size is used.
sound_interpol=type [default none]
  Normally, sound samples are output exactly as they are computed, without
  any post-processing.  This can generate errors in the sound output when the
  output frequency isn't an even multiple of the input frequency.  These
  errors are usuable perceived as a high-frequency noise.
  There are currently two types of interpolation available, both under
  experimentation.  You can use either "rh" or "crux" as value for this
  option.  Note that no interpolation is supported for 8 bit output; you need
  to use 16 bit output to hear a difference.  If you have any comments about
  the effects of either method on audio quality, I'd be very interested to
  hear them.

Memory options:
bogomem_size=n [default=0]
  Emulate n*256K slow memory at 0xC00000. Some demos/games need this.
fastmem_size=n [default=0]
  Emulate n megabytes of fast memory as an expansion board.
z3mem_size=n [default=0]
  Emulate n megabytes of Zorro III fast memory as an expansion board.
chipmem_size=n [default=4]
  Emulate n*512K chip memory. Some very broken programs need specific amounts
  of chip mem to work properly. The largest valid value is 16, which means 8MB
  chip memory.

Display options:
gfx_width=n [default=800]
  Use a window that is n pixels wide for displaying the Amiga screen.
gfx_height=n [default=300]
  Use a window that is n pixels high for displaying the Amiga screen.
gfx_lores=bool [default=no]
  Enable this option if you use a very small window width (320 to 400 pixels)
  to shrink the display horizontally.
gfx_linemode=type [default=scanlines]
  The type can be none (every line is drawn once), "double" (every line is
  drawn twice), and "scanlines" (every line is drawn once, but the image is
  stretched vertically by inserting a black line every other line to simulate
  the display on an old monitor).
  The "double" mode gives best results, but slows down the emulation quite a
  lot. Don't use the "none" mode if you want a decent interlace emulation.
  If you use "double" or "scanlines", your window needs to be twice as high
  as when using the "none" mode.
gfx_correct_aspect=bool [default=none]
  Try to fit the image into the specified window dimensions by leaving out
  certain lines.  Useful if you want to fit a 640x512 Amiga display in a
  640x480 window.
gfx_center_vertical=bool [default=no]
gfx_center_horizontal=bool [default=no]
  If you use a smaller window than 800x300 (400x300 with "gfx_lores" option or
  800x600 with a gfx_linemode other than "none"), not all parts of the display
  will fit on the screen. By enabling the necessary centering options, you can
  ask the emulator to try and move the screen contents so that the relevant
  parts are displayed. If you are unlucky, this can cause the contents to jump
  around a bit in certain cases.
gfx_fullscreen_amiga=bool [default=no]
  Enable if you want to use the full screen, not a window on the desktop, for
  the Amiga display.  Some ports (DOS, SVGAlib) always use fullscreen mode.
gfx_fullscreen_picasso=bool [default=no]
  Like gfx_fullscreen_amiga, but for the Picasso graphics card display.
gfx_color_mode=mode [default=8bit]
  Select a color mode to use.
  Color modes: 8bit (256 colors), 15bit (32768 colors), 16bit (65536 colors),
	       8bit_dithered (256 colors, with dithering to improve quality),
	       4bit_dithered (16 colors, dithered); 32bit (16 million colors)
gfxcard_size=n [default=0]
  Emulate a Picasso 96 compatible graphics card with n MB graphics memory.
  This requires that you use set the CPU type to "68020" or higher, and that
  you do not use 24 bit addressing.

Debugging options (not interesting for most users):
use_debugger=bool [default=no]
  If enabled, don't start the emulator at once, use the built-in debugger.
log_illegal_mem [default=no]
  If enabled, print illegal memory accesses
*/

static TEXT buffer[255];

static CONST_STRPTR CPU_Type[]             = {"68000", "68010", "68020", "68020/68881", "68040"};
static CONST_STRPTR SND_Mode[]             = {"none", "interrupts", "normal", "exact"};
static CONST_STRPTR SND_Channels[]         = {"mono", "stereo", "mixed"};
static CONST_STRPTR SND_Resolution[]       = {"8", "16"};
static CONST_STRPTR GFX_Chipset[]          = {"ocs", "ecs_agnus", "ecs", "aga"};
static CONST_STRPTR GFX_SpriteCollisions[] = {"none", "sprites", "playfields", "full"};

static BOOL config_parse_line(char *buffer, char **variable, char **value);
static LONG config_search_array(char *value, CONST_STRPTR *array, LONG count, LONG def);
static BOOL config_check_bool(char *value);
static void config_write_bool(BPTR fh, LONG value);

BOOL config_write(struct UAEConfigdata *cfg, STRPTR filename)
{
#ifdef TEST
    filename = "RAM:test.uaerc";
#endif
    BPTR fh = Open(filename, MODE_NEWFILE);
    if (fh)
    {
        /* CPU */
        FPuts(fh, "cpu_type = ");
        FPuts(fh, CPU_Type[cfg->cpu_type]);

        FPuts(fh, "cpu_compatible = ");
        config_write_bool(fh, cfg->cpu_compatible);
        
        /* Graphics */
        FPuts(fh, "immediate_blits = ");
        config_write_bool(fh, cfg->gfx_immediate_blits);
        
        FPuts(fh, "chipset = ");
        FPuts(fh, GFX_Chipset[cfg->gfx_chipset]);
        
        /* Sound */
        FPuts(fh, "sound_output = ");
        FPuts(fh, SND_Mode[cfg->snd_mode]);
        
        FPuts(fh, "sound_channels = ");
        FPuts(fh, SND_Channels[cfg->snd_channels]);
        
        FPuts(fh, "sound_bits = ");
        FPuts(fh, SND_Resolution[cfg->snd_resolution]);
        
        Close(fh);
    }
    return TRUE;
}

BOOL config_read(struct UAEConfigdata *cfg, STRPTR filename)
{
    char *variable;
    char *value;
#ifdef TEST
    filename = "RAM:test.uaerc";
#endif
    BPTR fh = Open(filename, MODE_OLDFILE);
    if (fh)
    {
        while (FGets(fh, buffer, sizeof(buffer)))
        {
            config_parse_line(buffer, &variable, &value);
            
            /* CPU */
            if (!strcmp(variable, "cpu_type"))
            {
                cfg->cpu_type =
                    config_search_array(value, CPU_Type, sizeof(CPU_Type)/sizeof(STRPTR), 0);
                bug("gefunden :%d\n", cfg->cpu_type);
            }
            else if (!strcmp(variable, "cpu_compatible"))
            {
                cfg->cpu_compatible = config_check_bool(value);
            }
            
            /* Graphics */
            else if (!strcmp(variable, "immediate_blits"))
            {
                cfg->gfx_immediate_blits = config_check_bool(value);
            }
            else if (!strcmp(variable, "chipset"))
            {
                cfg->gfx_chipset =
                    config_search_array(value, GFX_Chipset, sizeof(GFX_Chipset)/sizeof(STRPTR), 0);
            }
            
            /* Sound */
            else if (!strcmp(variable, "sound_output"))
            {
                cfg->snd_mode =
                    config_search_array(value, SND_Mode, sizeof(SND_Mode)/sizeof(STRPTR), 0);
            }
            else if (!strcmp(variable, "sound_channels"))
            {
                cfg->snd_channels =
                    config_search_array(value, SND_Channels, sizeof(SND_Channels)/sizeof(STRPTR), 0);
            }
            else if (!strcmp(variable, "sound_bits"))
            {
                cfg->snd_resolution =
                    config_search_array(value, SND_Resolution, sizeof(SND_Resolution)/sizeof(STRPTR), 0);
            }
        }
        Close(fh);
    }
    return TRUE;
}


static BOOL config_parse_line(char *buffer, char **variable, char **value)
{
    *variable = buffer;
    *value = NULL;

    char *equal = strchr(buffer, '=');
    if (equal)
    {
        *equal = '\0';
        *value = equal+1;
        while (isspace(**variable))
        {
            (*variable)++;
        }
        while (isspace(**value))
        {
            (*value)++;
        }
        while ((equal >= buffer) && isspace(*equal))
        {
            *equal = '\0';
            equal--;
        }
        equal = *variable + strlen(*variable) - 1;
        while ((equal >= *variable) && isspace(*equal))
        {
            *equal = '\0';
            equal--;
        }
        return TRUE;
    }
    return FALSE;
}

static LONG config_search_array(char *value, CONST_STRPTR *array, LONG count, LONG def)
{
    LONG index = 0;
    while (index < count)
    {
        if (!strcmp(value, array[index]))
        {
            return index;
        }
        index++;
    }
    return def;
}

static void config_write_bool(BPTR fh, LONG value)
{
    if (value)
    {
        FPuts(fh, "yes");
    }
    else
    {
        FPuts(fh, "no");
    }
}

static BOOL config_check_bool(char *value)
{
    if (!strcmp(value, "true"))
    {
        return TRUE;
    }
    return FALSE;
}
