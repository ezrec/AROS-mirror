/*
 * version
 *
 *   revision information
 */

/*
 * 4p - Pixel Pushing Paint Program
 * Copyright (C) 2000 Scott "Jerry" Lawrence
 *                    jsl.4p@umlautllama.com
 *
 *  This is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* the version number */
#define J_VERSION "0.10"

/* the revision number, which happens to look like the date. go figure */
#define J_REVISION "2001.12.18"

/* the prhase of the compile - whatever was on my mind... */
#define J_TPOTC "Tiny Geometries"


/*
 **********************************************************************
 * 2001.12.18 "Tiny Geometries" 0.09
 *	Ray Lynch song on the album "Deep Breakfast"
 *	Fixed the crash bug when a dialog closes
 *
 * 2001.12.12 "Airplane II"
 *	Tried to put in the autofocus for when a dialog closes, but failed
 *	Cleaned up paint/tools/screen.c for file requestors
 *
 * 2001.12.09 "Airplane"
 *	Put in hooks for dealing with a dirty palette.
 *	It currently only works when everything is 32 colors.
 *
 * 2001.12.02 "Waiting"
 *	Space Ghost episode
 *	finished up the first version of the palette editor
 *
 * 2001.11.29 "Water"
 * 	Song by Psychic TV
 *	started working on the palette editor dialog for no good reason
 *
 * 2001.11.26 " - "
 *	Cleaned up the SunOS build
 *	Tweaked the file requestor and listview items 
 *	list widget needs a bit of thought and re-coding before continuing.
 *	perhaps it needs a helper function to determine mouse region?
 *
 * 2001.11.25 "Angel"
 *	Added the acceptor dialog, and threw in a beta notify acceptor 
 *	Most of the listview dialog done
 *	Half of the dialog scanner is done
 *	Half of the file requestor is done
 *
 * 2001.11.25 "Why don't you gentlemen have a Pepsi?"
 *	Spies Like Us
 *	added in return values into the event system (buttons, etc)
 *	changed the res requestor to mind for the closed window
 *	Created the File Requestor and Font/Text Requestor
 *	Load, Save, and Text tools now use the above requestors.
 *
 * 2001.11.23 "Manhunter"
 *	Michael Mann film
 *	Added in the message redraw mechanism for the brush toolbox
 *	- need to reorganize the mechanism structure a bit.
 *
 * 2001.11.22 "Silence"
 *	Silence Of The Hams (movie)
 *	fixed the brush past the left of the screen bug
 *	fixed the overlapping rectangle/dialog bug
 *
 * 2001.11.20 "This is the me of me"
 *	Shock Treatment
 *	added JSUI_F_NOFOCUS so that the focus finder skips the widget.
 *	brush dialog bitmap buttons done (lots of tweaking and recompiling)
 *	3dbox changed to work without text.  oops... never needed that before
 *	changed the color display to show the fg color differently
 *	all positions and sizes of toolbar bits tweaked to look better
 *	added many PAINT_MSG entries to paint.h
 *	switched the toolbar items to broadcast PAINT_MSG'es
 *	paint dialog now receives and processes messages.  
 *	all of the brush tools use the new message mechanism
 *	edit tools, and mode selectors are next
 *	gotta figure out how to handle button toggle messages. hrm.
 *	(ie, i switch to vector using the keyboard, update the screen.)
 *
 * 2001.11.19
 *	switched the new gfx to use GUI colors instead of WHITE and BLACK.
 *	made disabled image buttons not display a frame.
 *
 * 2001.11.18 "Dr. Nightmare, attorney at law"
 *	Space Ghost Coast To Coast
 *	widgets	 added disabled buttons
 *	fileio	 c file saver created (for toolbar)
 *	widget	 imagebutton completed
 *	pulled most of the widgets out into their own files.
 *	made the button okay with left and right "ON"
 *	first version of a graphical toolbar. 
 *	fixed the non-sticky button problem (added code and redraws)
 *
 * 2001.11.15 "Nightmare at 20,000 Feet"
 *	Pop Will Eat Itself song
 *	tools/airbrush tweaked for better random performance.
 *
 * 2001.11.14 " - "
 *	Added BeOS variables.GNU for BeOS builds  (THANKS PAT!) 
 *	(It will probably work.  It is untested.)
 *
 * 2001.11.13 " - "
 *	Added modal dialogs
 *
 * 2001.11.12 "Otaku"
 *	Tetsu Inoue album
 *	pulled out the test dialogs out of main.c
 *	added in a #define to hide the splash screen 
 *	fixed the button callback in the splash screen
 *	fixed the placement of the splash screen
 *
 * 2001.11.11 "Moltar"
 *	widgets tweaked frame startup, and added the CENTER flag
 *	notes about the new tool switch system.
 *
 * 2001.11.09 "Kermit"
 *	widgets	added the NORTH/SOUTH/EAST/WEST for the frame widget
 *	palette	added the color display and picker widgets
 *	toolbar dialog added
 *	button callback function parameters tweaked to be more useful
 *
 **********************************************************************
 * 2001.11.08 "Maximus" 0.08
 *	Gladiator, the film
 *	widgets	finished the text entry dialog.  *phew!*
 *	uievents added keyboard events to dialogs and things.
 *	jsui directory structure seperated out between GUI and GFX
 *
 * 2001.11.07 "Shellshock"
 * 	Don't give up the game until your heart stops beating
 *	paint	added paint_init_screens, paint_init_rubberband 
 *		it no longer requires the images be around to work. YAY!
 *	menu	menu code is flaky and i don't like it.  it confuses me.
 *	splash	added the ulmaultllama llogo
 *	paint	initialized the font again. oops.
 *
 * 2001.11.06 "Through Metamorphic Rock"
 * 	Tangerine Dream song
 *	Menu structures migrated in
 *	Adding menu handler
 *
 * 2001.11.05 "Sick"
 *	Utah Saints song
 *	Fixed the 'is this point in this region' bug
 *	changing over text from 'absynth.com' to 'umlautllama.com'
 *
 * 2001.10.02 "Bus Stop"
 *	Where I am waiting right now.
 *	implemented the META key catchers for single mouse button systems
 *
 * 2001.09.25 "I Feel Loved"
 *	Depeche Mode song
 *	Page_VariBlit_Image() added
 *
 * 2001.09.19 "There's Nothing Out There"
 *	A great movie
 *	zoom, paint implemented the auto enlarge zoom feature.  
 *	 this should be changed eventually, or at least not hardcoded
 *
 **********************************************************************
 * 2001.09.15 "Brain" 0.07
 *	"great brain robbery" game
 *	"Highly Mediocre"
 *	paint	working on a new update mechanism
 *	util	added, extent_*() fcns for the new paint update mechanism
 *	finally got all of the dirty rectangles all happy again
 *	floodfil integrated pat's floodfill algorithm.  YAY!
 *	
 * 2001.09.09 " - "
 *	-
 *	page	added more range checks to blit_image
 *
 * 2001.09.08 "Falling To Pieces"
 *	Faith No More tune
 *	event    converted the event list to use JSUI message events
 *	uievents reconnected the fallback function
 *
 * 2001.09.07 "I Zimbra"
 *	Talking Heads tune
 *	Pat helped me track down the aforementioned bug
 *	I figured out the bug in the quickblit routine. (parenthesis)
 *
 * 2001.09.06 "I forget Utah"
 *	John Linnell - "Utah"
 *	dirty buffer updates have been substantially cleaned up
 *	finding a bug now when dragging UI frames down. *shrug*
 *	(when the window is dragged off the bottom of the screen, and
 *	 a background is set, it skips down to the bottom.  no idea.)
 *
 * 2001.09.05 "Harder, Better, Faster, Stronger"
 *	Daft Punk song
 *	proper dirty rectangle blitting take 2
 *	widgets	jsui_widget_frame() drag was borked.  
 *		made a small change to fix it
 *	dirty rectangles are messed up everywhere in the old code.  
 *	gotta pull all of that code out and re-write it from scratch
 *
 * 2001.09.04 " - "
 * 	various	tried the first implementaion of window DR speedup
 *	came up with a good algorithm, and a second zoom page
 *
 * 2001.09.03.a "Tour de Force"
 *	Covenant song
 *	splash	experimenting with the big "4P" megapixel
 *	paint	converted the old 4p main into paint_(de)init
 *	zoom	added, zoom_clearpage() uses large brown boxes
 *	paint	paint_update() added
 *	main	added an #ifdef around the set resolution to speed debugging....
 *	jsui	added jsui_key_shift and jsui_key_control
 *	uievents	created.  event callback from the old jsui/ui.c
 *	jsuibase	old jsui/ui.c renamed
 *		various other unused jsui / * files removed.
 *
 * 2001.09.02.a "Burnin' for you"
 *	Blue Oyster Cult song
 *	main	pulled out the splash screen generation to splash.[ch]
 *	splash	added the largeish "4P" megapixel to the splash screen
 *	splash	fixed the diagonal lines in splash screen (not lined up)
 *	ui	added _fallback function for event catching
 *
 * 2001.09.01.a " - "
 *	widgets	#ifdef'd out the close button, made the whole dragbar drag
 *	added in the bits of the old 4p that are needed to make the thing
 *	got everything to compile. (YAY!) Integration is next
 *	main	added GRADSPEED to slow down the diagonal gradient
 *
 * 2001.08.31.a "Like a wheel inside a wheel"
 *	"Slippery People" by The Talking Heads
 *	font	added font_render_text_right()
 *	ui	added jsui_active_background_page and dealing code
 *		- this redraw is a hack, and is slow..
 *		  need to do dirty rectangles properly.
 *	main.c	added a background splash page for the initial dialog
 *	main.c	added a hidden message too!
 *	
 * 2001.08.29.a " - "
 *	ui.c Hopefully fixed the filter with _reset_filter
 *	(This never showed up before, cause we never had two unrelated screens
 *       so the second time it runs, the old references in the static variables
 *       are now invalid... oops!)
 *
 * 2001.08.26.a "Just Like Heaven"
 *	song by The Cure
 *	font	added the Dirtify call so redraws work correctly
 *	widgets	added 3Dbox widget (modified text widget)
 *	widgets	added callback to button widget
 *	resolutn  new.  Added and implemented resolution selector
 *
 * 2001.04.11.a "Nervous Breakthrough"
 *	song by Luscious Jackson
 *	Pat helped me get the 'wandering window' idle drag straightened out
 *	
 * 2001.04.10.a "Minuet In Jazz"
 *	Song by Raymond Scott
 *	Added progress bars with _F_FLIP flag
 *	text widget done...  that was easy.
 *
 * 2001.04.09.a "Panda"
 *	ad on TLC
 *	basic event system complete. (mouse, buttons, focus)
 *	sliders are working
 *	buttons are working (radio, sticky, normal, mousefocus, etc)
 *	got rid of the stupid segfault on exit
 *
 * 2001.04.08.a "Across the Rio Grande"
 *	Duran Duran - "Rio"
 *	focus and draw straightened out.
 *
 * 2001.04.05.a "Call Me Names"
 * 	song by Echobelly
 *	primitive_rectangle_fourcolor - used for UI widgets
 *	fixed stupid bugs with blitting to the screen.
 *
 * 2001.04.04.a "Unforgiven"
 *	song by Covenant
 *	placement.c - created
 *	first jsui screen draws! Whee!
 *
 * 2001.04.03.b "-"
 *	pulled out messages to msg.c
 *
 * 2001.04.03.a "-"
 *	created activelist.c  for keeping track of the active windows
 *
 * 2001.04.02.b "windchime"
 *	moved 'palette.h' into jsui.h
 *
 * 2001.04.02.a "windchime"
 *	have one in our house
 *	JSUI integration.
 *	Reformatted graphics routines to reduce globals
 *	(and increase modularity)
 *
 * 2001.03.27.a "Know Where To Run"
 *	song by Orbital
 *	tweaked the crosshairs a bit (ie; rectangle draw)
 *
 * 2001.03.26.a "No Man's Land"
 *	song by Covenant
 *	found the bug with clearing the screen.  oops.
 *	fixed the Dirtify bug when making circles.
 *	fixed clipping dirty rectangle problem
 *	changed "rubberband backfill" into Page_Negative
 *
 **********************************************************************
 * 2001.03.25.a "Dead Stars" 0.06
 *	song by Covenant
 *	Added dirty rectangles.
 *	Increased draw speed
 * 
 * 2001.03.23.a "Dorito"
 * 	snack food
 *	optimized the blit loop
 *	started work on jsui/shwee for the user interface
 *
 * 2001.01.12.a "-"
 * 	view code (note: This was eventually not used... hehe)
 *
 * 2000.12.17.a "Pruitt Igoe"
 *	Philip Glass - "Koyaanisqatsi"
 *	file_backup() in fileio.c  to generate pesky "backup.pic" files.
 *	pulled iff/iff_io.c apart to _load _save and _type
 *
 * 2000.12.15.a "Pictures Of You"
 *	Song by The Cure
 *	Tried a hack to not draw a line on contiguous points in continuous line
 *	- it looked the same, so i pulled it.
 *
 **********************************************************************
 * 2000.12.07.a "The Pact" 0.05
 *	Blue Oyster Cult song
 *	Found the dumb 'B' bug.  I was calling it as a TOOL instead of ONESHOT
 *	Added CB_TOOL and CB_ONESHOT to reduce further error
 *
 * 2000.12.05.a "-"
 *	Fixed the stupid compiler warning in iff_io.c
 *	Worked out logistics for the brushes
 *	Added palette.h with the two default 32 color palettes
 *	Replaced brush code with a design that mimics the page views
 *
 * 2000.12.03.a "-"
 *      Added the circular internal brush
 *      Added the square internal brush
 *      -- need to work out the brush state machine
 *
 * 2000.11.30.a "-"
 *	got the menu and dialog structure hammered out. (borrowed from allegro)
 *	coded up the menu structuring, mimicing Deluxe Paint
 *	menu draw and valves not coded yet
 *
 **********************************************************************
 * 2000.11.28.a "Zorin" 0.04
 *	Christopher Walken in 'A View To A Kill'
 *	pulled apart the iff reader into iff, to keep it modular
 *	iff image loader working.
 *	'L' load image.  type in the path name ie: 'Images/Venus'
 *	fixed the flood fill bug - it would only do fg, never bg. oops
 *
 * 2000.11.27.b "one against one"
 *	Erasure - "It doesn't have to be"
 *	added timing.c and the "cycle_pen", and cycle_pen to the tools
 *	pulled the brush snag routine out from tools.c to page.c
 *	'3' '5' added 3 and 5 point scatterbrush to ibrush.c
 *	'C' filled circle -- cheesy, but kinda works.
 *	re-organized all of the tools into src/tools/___.c 
 *	tools.c was getting monolithic
 *
 * 2000.11.27.a "this is what it feels like"
 *	nine inch nails - "the great collapse"
 *	'c' added circle tool.
 *	pulled the flood fill algorithm out into gfx_prim.c
 *	bugfix: changing tools with the button down drew from the origin.
 *	bugfix: draws at origin when mouse brought outside the screen
 *	cleaned up some state code in ui.c, and tool_cb.c
 *	moved the source code into src/
 *
 * 2000.11.26.a "monkey" 
 *	'F8' added toggle cursor command.
 *	fixed an inconsistency with Deluxe Paint. 'd' <-> 's'
 *
 * 2000.11.25.a "Mr. Tweedy"
 *	Chicken Run
 *	re-arranged the docs to be in the docs directory
 *	added documentation for the Amiga Font format from RKRM
 *	added documentation for the EA IFF ILBM image format from RKRM
 *	started implementing the IFF loader
 *	'x','y' added tools to flip the brush vertically and horizontally
 *	'X','Y' added tools to stretch the brush vertically and horizontally
 *	'H' added tool to double size of the brush
 *	'J' added tool 'copy to spare'
 *	'z' added tool to rotate the brush 90 degrees
 *
 * 2000.11.24.b "It's bigger than an enormous big thing"
 *	Junkyard Wars marathon on TLC
 *	'b' added the snag brush tool
 *	'f1-f3' draw modes for brush (Matte, Color, Replace)
 *	draw tools for brush 
 *	BUG: graphics primitives are too optimized -- brush draw broken
 *
 * 2000.11.24.a "junkyard"
 *	Junkyard Wars marathon on TLC
 *	added the fullscreen crosshairs for the rectangle tool
 *	added the paintcan cursor, which doesn't seem to work...
 *	't' added a rubberband display for the text input tool
 *	'u' added the undo functionality to page.c and tools.c
 *	'j' added the swap functionality   (both of these were trivial.)
 *
 * 2000.11.23.a "turkey"
 *	Turkey Day
 *	cleaned up the rubberband clear code -- gets called from the tools
 *	eyedropper tool
 *	added tool_previous_mode()
 *	added the arrow, eyedropper, and ibeam cursors
 *	added the cursor macros and their use in tools.c
 *
 **********************************************************************
 * 2000.11.22.b "little green neon hands" 0.03
 *	"Hourglass" by Squeeze
 *	Pulled fg/bg out to be seperate entities in the page structure
 *	made all of the primitive tools do fg or bg color
 *	fixed flood fill a bit.
 *	added airbrush.  (and a hack to the event system that i'm not proud of)
 *	added a cheesy text entry method
 *
 * 2000.11.22.a "Hyperspeed"
 *	The Prodigy
 *	Added some graphics primitives - lines, solid and open rectangles
 *	Implemented more tools:
 *	    continuous draw, draw lines (vectors), flood fill, clear screen
 *	flood fill is _VERY_UNSTABLE_
 *	... not bad for 7 days of work ...
 *
 * 2000.11.21.a "The Great Collapse" 
 *	Nine Inch Nails - Things Falling Apart
 *	design & base implementation work on tool callback mechanism
 *	got the mouse events working and hammered out
 *	implemented dotted draw, and palette +/-  '[' and ']'
 *	drawing is now modal.  you cant draw unless you press 'd'
 *
 **********************************************************************
 * 2000.11.19.a "Mrs. Now"  0.02
 *	Too Much Joy song
 *	the only thing referecing bits of jpnm are now in fileio.c
 *	screen blit switched to use 'pages'
 *	page manipulations (create, destroy, duplicate) done.
 *	the first pixels have been drawn on the page!
 *	there is one level of coordinate dereferencing to be done though.
 *	crosshairs cursor added
 *	'DESIGN' updated to include a lot more information
 *	alpha channel added to the 'page' structure
 *	added license information to all .c and .h files
 *
 * 2000.11.18.a "Rocky"
 *	Road trip to NYC to see Rocky Horror on Broadway
 *	more design notes on how to handle 'pages'
 *
 * 2000.11.17.b "Implosion"
 *	The Learning Channel special about destroying buildings
 *	more core design and coding done.  (page.h, pixel.h)
 *	updated pen to use truecolor/RGB or indexed color selection
 *	updated font code also for RGB/Indexed
 *
 * 2000.11.17.a "Carlotta"
 * 	Dead Men Don't Wear Plaid
 *	Optimized the screen blit routines for no reason.
 *
 **********************************************************************
 * 2000.11.16.a "Metropia"  0.01
 *	RJMJ tune on "Logan's Sanctuary"
 *	integrated in FONT and PEN code from mujaki
 *	created the web page with Zarnox  ;)
 *
 * 2000.11.15.b "Ogilvie"
 * 	Remix on "things falling apart"
 *	Expanded the PnM loader to work with more than just P3 and P6
 *	Added support for PJM files :)
 *	Updated the screen blit routine to display paletted images too!
 *
 * 2000.11.15.a "Something Good"
 *	Utah Saints
 *	got the PPM to PJM converter working. (made the PJM/PGM format)
 *
 * 2000.11.14.a "Cornelius"
 *	the ape
 *	Got file loading, proper blit to screen
 *
 * 2000.11.11.a "Beautiful"
 * 	movie on tv
 *	first version, no real code written yet
 */
