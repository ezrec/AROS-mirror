/*
                     MAND4.C - Information
             Mandelbrot Self-Squared Dragon Generator
                    For the Commodore Amiga
                         Version 2.00

                      Accompanies MAND.C

             Copyright (C) 1985, Robert S. French
                  Placed in the Public Domain

     Assorted Goodies and Intuition-stuff by =RJ Mical=  1985

This program may be distributed free of charge as long as the above
notice is retained.

*/

/* === IMPORTANT NOTE: =================================================
 *   If you are going to design Information pages, make sure that all
 *   lines are a maximum of 59 characters long.  Also, make sure that
 *   there are 19 lines at most per page.  These two restrictions
 *   combined comprise the reason why the info pages are (and should
 *   remain) terse.
 * =====================================================================
 */


#include "mand.h"

extern FILE *console;


void Information(SHORT page)
{
   FILE *c;

   c = console;
 
   switch (page) {

case 0:
case 1:
fprintf(c, "Mandelbrot Self-Squared Dragon Generator Version %s\n", VERSION);
fputs("Copyright (C) 1985 - Robert S. French\n",c);
fputs("Extensively augmented (with Intuition!) by  =RJ Mical=  1985\n",c);
fputs("Placed in the public domain\n",c);
fputs("\n",c);
fputs("Inspired by Scientific American, August/1985\n",c);
fputs("Corrections and improvements suggested by\n",c);
fputs("\"The Fractal Geometry of Nature\"\n",c);
fputs("By Benoit Mandelbrot, W.H. Freeman and Company, 1983\n",c);
fputs("(Used to be Z=Z^2+C, now is Z=Z^2-u, etc.)\n",c);
fputs("\n",c);
fputs("Robert S. French may be contacted at:\n",c);
fputs("   USPS:   2740 Frankfort Avenue\n",c);
fputs("           Louisville, Ky  40206\n",c);
fputs("   Phone: (502) 897-5096   ARPA: French#Robert%d@LLL-MFE\n",c);
fputs("=RJ Mical= may be contacted at:\n",c);
fputs("         Commodore-Amiga, Inc.\n",c);
fputs("         983 University Avenue\n",c);
fputs("         Los Gatos, CA  95030\n",c);
break;

case 2:
fputs("The Mandelbrot set consists of pairs of numbers, one number\n",c);
fputs("real and the other complex.  This set can be graphed using\n",c);
fputs("one axis for the real numbers and the other for the complex\n",c);
fputs("numbers.  The Mandelbrot set is located on a graph roughly\n",c);
fputs("centered over the origin (0,0).  The default settings of\n",c);
fputs("this program create a display that shows the entire set.\n",c);
fputs("All of the black display pixels represent Mandelbrot pairs.\n",c);
fputs("The shaded pixels show the pairs that are *almost* in the\n",c);
fputs("set.  The pairs nearest the set on the display just miss\n",c);
fputs("making it into the set numerically; these are white.  The\n",c);
fputs("more grey a position, the further the pair is from the set.\n",c);
fputs("    This program lets you explore the Mandelbrot set by\n",c);
fputs("panning across and zooming in to look at details of the set.\n",c);
fputs("You can also change colors and display mode.  For instance:\n",c);
fputs(" : CS 1      ; Use the fancy arrangements of color set 1\n",c);
fputs(" : CO 29     ; Set the initial color offset\n",c);
fputs(" : CI 4      ; Set the color increment\n",c);
fputs(" : MC 29     ; Set the maximum tests count\n",c);
fputs(" : G         ; Generate the picture\n",c);
break;


case 3:
fputs("The MC command lets you specify how many times each position\n",c);
fputs("is tested to see whether or not it's in the Mandelbrot set.\n",c);
fputs("A low MC value means that very few tests will be done on\n",c);
fputs("position, which will cause the display to be created more\n",c);
fputs("quickly, but which will also have a cruder approximation of\n",c);
fputs("the Mandelbrot set and, consequently, fewer colors.\n",c);
fputs("\n",c);
fputs("The MX and MY commands set the size of the display window.\n",c);
fputs("The larger your window, the longer it takes for the picture\n",c);
fputs("to be resolved.  While you're experimenting with the\n",c);
fputs("program, you should create a smaller window with a lower\n",c);
fputs("count value until you get approximately the picture that you\n",c);
fputs("want to see.  Then when you've gotten a picture you want to\n",c);
fputs("see in large scale, increase the size and count resolution\n",c);
fputs("to see the detailed version of your picture.  For instance:\n",c);
fputs(" : MC 8        ; Each position will be tested only 8 times\n",c);
fputs(" : MX 80       ; The width is 25% of full width\n",c);
fputs(" : MY 50       ; The height is 25% of full height\n",c);
fputs(" : G           ; Generate the picture\n",c);
break;

case 4:
fputs(" : CS 1        ; Color set 1 (special ranges of colors)\n",c);
fputs(" : CI 1        ; Color increment of 1 (smallest increment)\n",c);
fputs(" : MC 29       ; Count position and number of colors\n",c);
fputs("Then, by setting the initial color offset into color set 1 \n",c);
fputs("using the command CO, you can select from these ranges:\n",c);
fputs("      ======    ===========================================\n",c);
fputs("        1-15    unit steps of blue\n",c);
fputs("       16-30    unit steps of green\n",c);
fputs("       31-45    unit steps of red\n",c);
fputs("       46-60    unit steps of sky sky blue (blue and green)\n",c);
fputs("       61-75    unit steps of purple (blue and red)\n",c);
fputs("       76-90    unit steps of yellow (red and green)\n",c);
fputs("       91-115   unit steps of white (all colors)\n",c);
fputs("CM is the command to set the display mode.  Add up these:\n",c);
fputs("    o  add 1 to get NO HOLD-AND-MODIFY\n",c);
fputs("    o  add 2 to get INTERLACE\n",c);
fputs("    o  add 4 to get HIRES (640 columns; low-res is 320)\n",c);
fputs("For instance, for HIRES and NO HOLD-AND-MODIFY, add 1 + 4:\n",c);
fputs(" : CM 5\n",c);
break;

case 5:
fputs("The starting (left) and ending (right) edge values for the\n",c);
fputs("axis of the real numbers (the horizontal axis) can be set\n",c);
fputs("using the commands SR (Start Real) and ER (End Real).  For\n",c);
fputs("the complex numbers axis (vertical) the start (bottom) and\n",c);
fputs("end (top) edge values are set using SI and EI.  Got it?\n",c);
fputs("    The graph that you get with this program's default\n",c);
fputs("values has the real numbers along the horizontal axis.  The\n",c);
fputs("leftmost position represents the real number -2.85.  The\n",c);
fputs("rightmost position represents 2.85.  The complex numbers\n",c);
fputs("are charted along the vertical axis, starting at the bottom\n",c);
fputs("with a value of -2.05 and ranging up to 2.05 at the top.\n",c);
fputs("    You control the range of these axes using the text\n",c);
fputs("commands SR, ER, SI and EI.  The commands ZR and ZI let you\n",c);
fputs("zoom in or out on the real and imaginary axes respectively.\n",c);
fputs("ZB lets you zoom on both proportionally.\n",c);
fputs("    Once the display is built, there are ZOOM menu commands\n",c);
fputs("which allow you to zoom in and out automatically.  These\n",c);
fputs("are more convenient than using the text commands.  On the\n",c);
fputs("other hand, the text commands lend precision.\n",c);
break;

case 6:
fputs("There's lots more to this program than what's described on\n",c);
fputs("these info pages.  You should read the available text on\n",c);
fputs("Mandelbrot sets (see info page 1).  This program has more \n",c);
fputs("features than what's described here.  If you can get your\n",c);
fputs("hands on the source, you're welcome to expand these pages.\n",c);
fputs("In fact, this program is still far from complete.  Care to\n",c);
fputs("have a bash at it?  We need the greater-precision \n",c);
fputs("floating point.  Also, we seriously need to be able to\n",c);
fputs("save some or all of the image to a disk file in IFF format.\n",c);
fputs("\n",c);
fputs("Good luck.  Have fun!  Send mail (on USENET at least!) if\n",c);
fputs("you find spectacular scenes.\n",c);
break;


default:
fputs("SORRY:  there's not that many pages of info available!\n",c);
AvailableCommands();
break;

   }

}


void AvailableCommands()
{
    FILE *c;

    c = console;

/* === TRY TO KEEP THIS PAGE 18 LINES TALL AT MOST ======================== */
fputs("AVAILABLE COMMANDS:\n",c);
fputs("SH   - Show current settings        G  - Generate picture\n",c);
fputs("I n  - Information pages            Q  - Quit\n",c);
fputs("\n",c);
fputs("SR n / SI n / ER n / EI n - Starting and ending coords\n",c);
fputs("MX n / MY n / MC n - Maximum x and y resolution and count\n",c);
fputs("XR n / XI n - Displace coord\n",c);
fputs("ZR n / ZI n / ZB n - Zoom around center point\n",c);
fputs("\n",c);
fputs("CM n - Graphics mode                F n - Dragon function\n",c);
fputs("CI n - Color increment              CD  - Color divisor\n",c);
fputs("CO n - Color offset                 CS  - Color set\n",c);
fputs("CT n - Color for points in set      P n - Start from Preset\n",c);
fputs("\n",c);
fputs("D  - Display picture                A  - Analyze\n",c);
fputs("SA filename - Save picture          L name - Load picture\n",c);
fputs("< filename - Redirect input         ; string - Comment\n",c);
fprintf(c,"MM n - Maximum number of %d pixel lines in memory\n",MAXX);

}
