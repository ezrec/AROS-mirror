/*


4 October 99:
Version id: 0.9

Both - Completed the amigaguide documentation and prepared the files for
       submission to Aminet.  Ensured proper thank yous were done and a
       nice icon generated.
       
       
       Settings - HISoft C++
       
       project default - defines    __chip    NULL 
       (Note: null represents nothing being typed into the replace box)
       Compile option : Ansi C
       
       If you have the default compile option set for Ansi C, then you can simply
       add all the files by shift clicking them to a new project.  Once all the
       proper files, excluding the iffp directory have been added, simply choose
       compile.  You will get 4 warnings in relation to possible asignment operation
       with the = sign.  Ignore these.
       
       The rable project file is not included, since all the paths
       would be wrong.  If it is desired, email Aaron for it
       and he will send it to you.
       
       
       Settings - SAS/C
       
       The SCoptions file is included for a few settings.
       When you compile with SAS you will probably get warned about
       "no reference to identifiers" temp and tempchar.  Just ignore it.
       
       About the iffp directory:
       It contains headers necessary for using IFFParse.
       These headers were written by Commodore.
       


12 Sept. 99:
Version id: 0.9

Aaron - in rable_draw.c improved the draw_free() routine.  Now it draws
        the freehand lines with no gaps.
        In rable.c added a crosshair pointer to window1.
        Aaron also fixed problems caused by an older ilbm.h.

Bob - rable.c
      New defines: #ifdef AZTEC_C
                   #define __chip
                   #endif
      Hopefully this will allow same code with SAS and HiSoftC.
      This was also done in rable_images.h.
      Now window1 (the canvas) will initially be a standard 320x200.
      window1 and window3 now detect IDCMP_GADGETUP events.
      Added settings/canvas size to the menus.
      process_events() now initializes "last" tool and color.
      Added resize_handler().  Now the user can resize the canvas.
      Added slider_handler().  Now the user can change the palette.
      Added slider_refresh().
      SavePic() now does error-checking.
      
      rable_gadgets.c
      Added flag to pgad2 so it's initially selected.
      Added flag to cgad1 so it's initially selected.
      Added structs needed for canvas size requester.
      Added structs needed for palette sliders.




26 August 99:
Version id: 0.8
Bob - Finished save routine (except error-checking.) Details follow:
      Two things made this difficult at first:
      1. You can't save the ColorMap directly from the screen.
         You have to get the values with functions, and convert them
         into ColorRegisters, and finally write the CMAP chunk.
      2. You can't save the window's BitMap directly either.
         You have to blit the window contents from the ViewPort's
         BitMap into a temporary BitMap, then write the BODY chunk.

rable.c - added SavePic() function. Saves an ILBM using IFFParse.
rable.c - now opens and closes IFFParse.library.
rable.c - deleted unused global: struct ViewPort vp.
rable.c - added the #include "iffp/ilbm.h".
          NOTE: From now on we must include "iffp/" directory,
          which contains several IFFParse support header files.
rable.c - got rid of "palette" and "tool" titles in title bars.
          Changed "Paint Window" to "Canvas".
rable.c - Fixed size of the toolbox with temp_w and temp_h variables.
          Also changed palette window to use temp_w and temp_h.
rable.c - File Requester now says "Load or Save Picture".
rable.h - Added the prototype for SavePic().
rable_images.h - Added the __chip keyword where needed for SAS/C.
rable_draw.c - Added RefreshWindowBorder() to each tool to repair
          the borders after they have been drawn on.
          Aaron should check behind me; I hope I did this right!
          
Next:  I want to add a Settings menu, and under Settings there will be
the "Canvas Size" and the "Edit Palette" items.




22 August 99:
Version id: 0.8
Aaron - Rewrote and completed the drawing routines for the triangle.
It can now be drawn at any angle and size, both filled and outline
format.  The routines could be condensed to just one routine, but
this will probably not be done.  Will fix the width of the toolbox,
as its wider than the current number of tools.

Bob - Implementing save routine.  Files are saving, but no data is
being stored in the file.  Files will be stored as IFFs when complete.

Both - Tossing around ideas for next program and deciding what to
do with RABLePaint once its complete.



31 July 99:
Version id: 0.7
Aaron - Got into gear and did some real work on project.  Added new
include file called 'rable_fills.h'.  This file holds all the data
for the fill patterns.  The image data representing the fill is
located with the rest of the gadget imagery.  Added a partial
routine for drawing triangles, but need to adjust it to allow for
rotation of the triangle.  Re-positioned a few of the gadgets and
added triangle, rectangle, and fill patterns.  Found a duplicate
define section in rable.h.  It was commented out, but left in case
other compilers may need it.  It IS NOT current though.  The correct
values for number of gadgets, etc can be found in the rable_draw.h
file.



25 July 99
Version id: 0.7
Aaron - Finnally got the initial fill routines to work!  The oval
and circle routines are now fixed.  The square needs to be done
still and the setup routine needs to be cleaned up.  It looks pretty
messy as is.  Fixed problem with dots being draw in the center of
ovals.  Adjusted gadget images for fill patterns and put in nicer
image for freestyle drawing.



June 99
Version id: 0.6
Both - Did much of nothing.  Worked on side projects and thought
about what to do next.

Bob - Wondered if Aaron was still alive.  :)  Adjusted load routine,
as it wasn't working just right.



23 May 99
Version id: 0.6
Bob - Rewrote the picture load routine to use AddDTObject() when
adding the picture to the window, rather than BlitBitMapRastPort().
Also improved the way it loads the palette of a picture and copies
it into the screen's ColorTable.  Another EasyRequest was added so
there are 2 different warnings, to indicate whether a file was not
a known picture format, or the picture was too big/too many colors.

Improved filename handling by using strcpy instead of sprintf.
Filename length can now be 512 characters.  Added #include string.h.
Deleted unneeded DrawRect code that was in setup_display() function.
Moved initial ColorTable to header file Rable.h.
Got rid of IDCMP_CLOSEWINDOW stuff, since it's not being used.
Decided to leave it at 32 colors.



21 May 99
Version id: 0.5

Aaron -	Completely rewrote idcmp handling.
Introduced concept of handlers for various events.
A handler must be able to take in various messages
and determine where to send program control next
based upon given circumstances.

The program is now structured around handlers:
tool, palette, fill, drawing.
Each drawing routine also acts as a handler.
There are a few globals ALL draw handlers access:
xstart/ystart (starting points for draw)
xlast/ylast (last points accessed)
draw_flag (the current condition or stage for the selected tool)
Added in routines for circle, oval, and line (Colors even work!)
Trying to get initial tool and color when program runs, no luck yet.



20 May 99
Version id: 0.5

Aaron -	Updated all rable .c and .h files to work with HiSoft - 
need to check with SAS
Discovered compiler definition is MAXON, used for #ifdef MAXON checking
Added 5 more gadgets, two fills, ovals, and line
Added wbmain function so program can be started from WB
				
Bob -
Recommended switching to 16 color palette for non-graphics card users
Working on rewriting the picture load routine
			


xx May 99
Version id: 0.1 to 0.4

Both -	Worked on getting code to work with both SAS and Dice compiler.
Created backbone of program and began creating actual layout of program.
Traded various ideas back and forth as to what would be good to implement.
			
*/
