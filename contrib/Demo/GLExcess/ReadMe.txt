10/09/2003
----------

This is the source code for the demo part only, benchmarking code is not included.
All references to Win32-specific code have been removed, in order to make the
whole thing easily portable. Unfortunately, this also implies that there is no
realiable timing anymore (glut timing just could not do the work), though you can
adjust the timing step with 'a' and 'z' keys. Space bar will jump to the next scene.

Command line arguments:
-----------------------
Arg1: Windowed/Fullscreen (0/1)
Arg2: Width
Arg3: Height
Arg4: Bpp
Arg5: VFreq

Notice about textures:
----------------------
All textures have been converted to a simple R8G8B8 format, where the first two bytes
contain width and height as powers of 2. This makes it possible to load and edit
textures inside Photoshop, just remember to set 3 (interleaved) color channels and
a header size of 2 bytes, most times the program will automatically guess the right
dimensions.

License:
--------

The source code is published under the terms of the GNU General Public License V2.
For more details about the license, read the LICENSE file available in the source
distribution.

Building the demo on Win32+MSVC:
--------------------------------

You need to have the opengl.lib, glu.lib and glut.lib files available somewhere in
the your linking lib path.

Then open the glxglut.dsw project, and click on compile (F7 shortcut).

Building the demo on Unix:
--------------------------

You need to have the development headers of opengl/mesa, glu and glut installed with
corresponding libs.

Then type in the root dir of this source distribution just type:
$ make

There is no install rule, and the demo must be run in current path (it needs access
to ./data)

----------------------------------------------------------
As usual, feel free to drop comments: bustard@glexcess.com
----------------------------------------------------------
