short: some demo effects (tunnel, bumpmap,etc. ) , including C source
author:Ronald Hof, Ilja Plutschouw
email: knp@chello.nl
version: 1.0

Here are some demo effects we made using KDP, our simple 320x200x8 bit 
graphics system.KDP makes it easier to write and port graphics effects.
Don't worry if you have never heard of it, because this is the first ever release.
We use it to compile the same source on Amiga and Windows computers.
Because it is so easy to port, we quickly made an Elate/Ami version as soon
as we figured out how to open a window,displaying a picture in it and getting
the mouse coordinates.

The only Elate/Ami specific code is in kdp.c, the rest of the code is the same on all platforms.
Don't expect clean/commented sources, as there's been quite some experimenting with most
of these effects. 

Compiling:
  to compile all sources, execute the m.scr script (type 'source m.scr').
  At the moment it gives some warnings ('multiple defining occurrences KDPcoltab').
  (unfortunately i've got no idea why :(   
  
Usage:
  'cd' to the directory where you unpacked the archive and run the programs from there,
  as some of them read data files from the current directory.

  *** this version of KDP does not get mouse events when the mouse is not over the window,
  therefore not everything functions as nice as it should ***

 lmb=left mouse button,rmb=right mouse button.

 *** use both mousebuttons or close gadget to quit **

included programs:

-bob  display object using sprites
  usage:./bob <3d object>  
  move mouse to change rotation speed, lmb toggles blur, rmb toggles transparency and stops rotation.
  both buttons to quit

-bump  bumpmapping example
  usage: ./bump
  lmb+mouse move changes light position
  both buttons to quit
  this program first calculates a large lookup table, so it takes a while before the window is opened.
  you can use your own pictures as color/heightmap, just change shade1.bmp and shade2.bmp
  (320x200 256 color uncompressed bmp files only)

-grav   particles attracted by the mousepointer
  usage: ./grav
  lmb sets particles to random positions
  rmb toggles attraction

-grav2    particles attraced by the mousepointer
  usage: ./grav2
  lmb sets particles to random positions
  rmb changes attraction force  
    
-motionblur    same as bob, only uses scaling sprites and real motionblur 
                           (calculates 16 frames for each frame displayed)
  usage ./motionblur <object>
  move mouse to change rotation
  lmb +mouse moving left/right :change rotation
  rmb :stop rotation
  rmb+mouse moving left/right :move object
  rmb+mouse moving up/down :zoom in/out

-poly shows object using various polygon routines
  usage: ./poly <object>
  move mouse to change rotation
  lmb +mouse left/right change rotation
  lmb +mouse up/down zoom in/out
  rmb stops rotation and goes to next drawing mode
  (some modes need a better palette then the one used at the moment)

-scaletest    show object using scaled sprites
                      (same as motionblur, only without the motionblur :)
  usage: ./scaletest <object>
  move mouse to change rotation
  lmb +mouse moving left/right :change rotation
  rmb :stop rotation
  rmb+mouse moving left/right :move object
  rmb+mouse moving up/down :zoom in/out

-tun    moving tunnel with light
  usage: ./tun
  move mouse up/down: change tunnel speed
  lmb+ move mouse up/down: change tunnel shape
  rmb+move mouse: move tunnel

-tun2   tunnel in tunnel, with shadow
  usage: ./tun2
  move mouse up/down: change tunnel speed
  move mouse left/right: rotate tunnel
  lmb+ move mouse left/right: open/close inner tunnel
  lmb+move mouse up/down: change outer tunnel shape
  rmb+move mouse left/right: change outer tunnel perspective

-tv    tv emulator
  usage: ./tv

-wire   show object in tranparent wireframe
  usage: ./wire <object>
  move mouse: change rotation
  lmb+mouse left/right: change rotation
  lmb+mouse up/down: change color
  rmb: stop rotation
  rmb+mouse up/down: zoom in/out

all programs:

 -<object> is a 3drw object (there is a converter on aminet (tdddtoraw))
     (there are some objects included in the objects dir)
    example: ./wire objects/test.3d
 -Some programs (bump and poly) calculate a large color lookuptable,
    there's a small delay before the window opens.
 -Use both mousebuttons or the window close gadget to quit.



