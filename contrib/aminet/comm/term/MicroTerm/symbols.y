/** symbols.y, this just includes everything under the sun... **/

//#include <devices/audio.h>
//#include <devices/bootblock.h>
#include <devices/clipboard.h>
#include <devices/console.h>
#include <devices/conunit.h>
#include <devices/gameport.h>
#include <devices/input.h>
#include <devices/inputevent.h>
#include <devices/keyboard.h>
#include <devices/keymap.h>
//#include <devices/narrator.h>
#include <devices/parallel.h>
//#include <devices/printer.h>
//#include <devices/prtbase.h>
#include <devices/serial.h>
#include <devices/timer.h>
#include <devices/trackdisk.h>

//#include <hardware/adkbits.h>
//#include <hardware/blit.h>
//#include <hardware/cia.h>
//#include <hardware/custom.h>
//#include <hardware/dmabits.h>
//#include <hardware/intbits.h>

//#include <resources/cia.h>
//#include <resources/disk.h>
//#include <resources/misc.h>
//#include <resources/potgo.h>

/*#include <workbench/icon.h>*/
#include <workbench/startup.h>
#include <workbench/workbench.h>

#include <exec/alerts.h>
#include <exec/devices.h>
#include <exec/errors.h>
#include <exec/exec.h>
#include <exec/execbase.h>
//#include <exec/execname.h>
#include <exec/interrupts.h>
#include <exec/io.h>
#include <exec/libraries.h>
#include <exec/lists.h>
#include <exec/memory.h>
#include <exec/nodes.h>
#include <exec/ports.h>
#include <exec/resident.h>
#include <exec/semaphores.h>
#include <exec/tasks.h>
#include <exec/types.h>

#include <graphics/clip.h>
//#include <graphics/collide.h>
#include <graphics/copper.h>
//#include <graphics/display.h>
#include <graphics/gels.h>
#include <graphics/gfx.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
//#include <graphics/graphint.h>
#include <graphics/layers.h>
#include <graphics/rastport.h>
#include <graphics/regions.h>
#include <graphics/sprite.h>
#include <graphics/text.h>
#include <graphics/view.h>

#include <clib/macros.h>

//#include <libraries/arpbase.h>
#include <libraries/configregs.h>
#include <libraries/configvars.h>
#include <libraries/diskfont.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
/*#include <libraries/expansion.h>*/
#include <libraries/filehandler.h>
#include <libraries/mathffp.h>
//#include <libraries/translator.h>

#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/preferences.h>
#include <intuition/screens.h>

/*#include <functions.h>   */
/*#include <arpfunctions.h>*/
/*#include <assert.h>      */
/*#include <ctype.h>       */
/*#include <errno.h>       */
/*#include <fcntl.h>       */
/*#include <math.h>        */
/*#include <setjmp.h>      */
/*#include <sgtty.h>       */
/*#include <signal.h>      */
/*#include <stat.h>        */
#include <stdio.h>
/*#include <time.h>        */

#include <aros/oldprograms.h>

#include "globals.h"

