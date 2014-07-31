This is a Win32 console mode build of DBW_Render, based on John H. Lowery's
MS-DOS port of David B. Wecker's Amiga original. The only changes I have
made to the core ray tracer are to set the default image resolution to
800x600, increase the maximum resolution to 1600x1200, and set the colour
depth to be 7 bits per colour component (from the original version's 4).

This version has been built with GCC with settings turned on to optimize
for speed. The result is that DBW_Render runs several orders of magnitude
faster on a modern high-spec PC than it did on my Amiga A500 when I first
encountered DBW_Render 15 years ago. For comparison, glass.dat would take
over 12 hours to render on an Amiga with anti-aliasing on: now it takes
a bit over two minutes, even though it is being rendered at a higher
resolution. For a direct comparison, glassorig.dat renders the "glass"
scene with exactly the same settings as used on the Amiga at 320x200:
it takes 4 seconds on my PC!

In addition, I have written a completely new version of the Display
program that shows the ray tracer's output. This version displays the
image in a window and can optionally save the image out as a PNG file.

David Kinder
davidk.kinder@virgin.net


The following comments are culled from those provided with the 
AMIGA version of the software by Dave Wecker, but are equally 
applicable to this version:


-> What is the best way to "design" a picture?

In your .DAT file set the following parameters:

	a.	no transmission (everything opaque).
	b.	no "mirrors" (use low reflection/diffusion levels).
	c.	use only 1 light source.
	d.	make the light source infinitely far away ("l" not "L").
	e.	use a small numbers of objects.
	f.	no anti aliasing

This will allow you to prototype pictures VERY quickly (minutes not hours).
BTW: this is why JUGGLER only took its author 1 hour/frame to compute it used
items a,c,d and e,f (depending on def. of "small")

-> I seem to be using A LOT of stack when there are mirrored objects.. why?

If you have two (or more) highly reflective spheres (mirrored) and a bright
light sources, you probably STILL run out of stack because you have to bounce
the light back and forth between them for a LONG time (not infinite do to the
inverse square law)... you can patch the file RAY.C changing all occurrences
of:
	 max_runs = 32767;

to a smaller number (somewhere between 30 and 300 for example). This will set a
much smaller maximum recursion value (and thus much smaller stack usage).

->	How do I go about positioning the eye/camera...

The easiest thing to do is to set the center of your image at (0,0,0). This is
what is done in the GLASS.DAT example. The eye is positioned at:

	(30,180,200) looking in the direction of (-50,-320,-350)

Ok.. so how does this give us (0,0,0)? Well, if you start at the eye and add
the direction vector you get (-20,-180,-150). It turns out that if you go 57%
of the way along the direction vector you get (0,0,0)!!! So you are looking
down and to the left through the origin and then 150 units further! By
adjusting this distance you can magnify or reduce the entire image.

At the tip of the vector you have 256 units in X visible and 170 units in Y
visible. This defines a viewing "cone" (which you can draw the lines out to).
But for example, in the plane of the screen (Z = 0), you have visible:

	x: -73 to +73		(57% of 256)
	y: -48 to +48		(57% of 170)

You can make this calculation for any desired depth.

->	Objects are getting FUZZ even when they have 0.0 "fuzziness"...

Remember that if you don't give the program enough time to run in, it will
"smear" objects by using the AI algorithms to guess at pixel values. Try to
give a small strip of the picture an infinite amount (999.0 hours) to run in
and you should see all pseudo-fuzz go away.

->	What EXACTLY do the AI heuristics do?

Most of the "guess-work" happens in the "ray.c" module. First of all, I keep a
cache of 9 lines (centered on the current line). This is used when the program
runs out of time on a line and needs to guess at a pixels value. Routine
"guess" calls "guess2" to scan out from the current pixel looking for computed
pixels which are averaged for a "best guess".

When you set a resolution (using "R" in the .dat file), an initial estimate is
made of how much time per scan line will be allowed. Pixels that are easy to
compute (max_runs = 10) are computed first. If we still have time (curtime <=
nxttime) or we haven't computed enough pixels (pixelcompute <= compmin
(described below)) we randomly pick pixels at full computational level
(max_runs = 32767) until either of the two previous conditions are met (or all
pixels on a line have been computed).

Here is where the heuristics come in. At the end of a scan line the following
estimates are made:

1.	The next scan line will be allowed to have time of:
		buftime	  - normal time for a scanline
	      + xtrtime/8 - "extra" time: either not used by previous scan
			lines (all pixels computed), or negative time (if
			previous scanlines used too much time).
2.	compthresh = (2/3) * pixelcompute
		This is the threshold at which max_runs switches over from
		10 to 32767 (recursion level). pixelcompute is the total
		number of pixels computed on prev scanline.
3.	compmin = (9/10) * pixelcompute
		This many pixels MUST be computed (even if we run out
		of time (this is how xtrtime can become negative)).

These estimates let us adaptively change how we decide the number of pixels 
per line to compute. All pixels are chosen randomly to keep patterning out 
of the picture.

->	The General License states that any derivative must fall under the
	terms of the license, does this mean that all pictiures created fall
	under the terms of the license?

No! The License only covers derivates of the CODE. Any pictures you create are
yours to do with as you please.

->	Can you explain "Global index of refraction"?

It's just what the documentation says... the index of the "air". For instance,
if you start increasing this value you can make the picture look like everyting
is under water (or embedded in glass).

->	Where should I set the anti aliasing parameters?

First read the documentation on the "A" setting, it's very detailed. Things to
remember:

1.	This parameter will help you get rid of the dreaded "jaggies"
	(jagged diagonal lines).

2.	If you set the COUNT to 4 each picture will take MINIMALLY
	4 times as long to compute. This is because you are now
	sending at LEAST 4 times as many rays through every pixel.

3.	The reason 2. says MINIMALLY is because of the THRESHOLD parameter.
	This measures the standard deviation in intensity of the COUNT rays
	sent through a pixel. If the SD is greater than the THRESHOLD than
	another COUNT rays are sent... and on and on until either the SD
	falls below the THRESHOLD or you've distributed 128 rays into a pixel
	(maximum).

4.	The results are then averaged to smooth out the "jaggies".

This is why there is a Z command to show you how much anti-aliasing actually
had to be done.

->	Just out of interest what do you do for a living and where did
	you go to school?

Head of AI tools for Digitial Equipment Corporation Database Systems Advanced
Development. Carnegie-Mellon Univ. (BSEE & MSIA).

->	I notice code for CYLINDER objects in RAY, why aren't they documented?

Cylinder was never completed (sorry). 

->	What happens if two objects occupy the same space?

Which ever one is "sorted" first will be visable (pretty random).

->	Can you give me a picture of the coordinate system?

It's a little difficult to draw a picture of the coordinate system, but I'll
try using an example where the eye is at 30,180,200 and the direction vector is
-50,-320,-350. If you add these two points you get the focus point
(-20,-180,-150). At the focus point there are +- 128 units in X and +- 85 units
in Y. Here is the result (anything inside of the "viewing cone" is visable):

		EYE	    SCREEN    FOCUS POINT___---
					   ___---
		 (-73,-48,0)	    ___---|(-148,-265,-150)
			    \ ___---      | :
		       ___---|	    	  | :-128 X, -85 Y
		 ___---	     |57%     100%| :
		<............|............|(-20,-180,-150) 
   (30,180,200)/ ---___	     |\(0,0,0)	  | :
		       ---___|		  | :+128 X, +85 Y
			    / ---___	  | :
		 (+73,+48,0)	    ---___|(108,-95,-150)
					   ---___
						 ---___

-> What are the maximum and minimum values of each axis?

Completely defined by the user. In the example shown above the range of values
(for the axis) are defined by the viewing cone. For example if you move to a Z
of -700, then you have (for a viewable range):

	Center of the screen:	( -70,-540,-700)
	Maximum X,Y:		( 186,-370,-700)
	Minimum X,Y:		(-326,-710,-700)

-> What is the range of the global refraction index?

Whatever you want it to be. For example, if you look up the index for glass
you'll find that it is around 1.2, so if you set "N 1.2" everything will look
like it's embedded in glass.

-> What does the "a" command actually do?

It makes believe that every object is lighted (minimally) by a percentage of
it's natural (diffuse) color. What this means is that the range if from 0 to 1
(100% lighted). A non zero value means (for example) that there is no such
thing as an absolutely black shadow.

-> What does the "H" command do?

It tints objects according to their distance from the eye.

-> What is IDX?

The index of refraction (from any physics book).

-> What is ambient light?

The light comming from all directions at once (background lighting).

-> How does diffuse reflection interact with with the color of light sources?

Each light sends out rays of blue green and red. The percentage of reflection
is defined by the diffuse amount. So, in a light of red and green a diffuse red
object will appear red while a blue object will appear black.

-> How are extents used?

The largest amount of time used in a ray tracer is figuring out what items a
ray hits. Normally EVERY object has to be examined to decide this. However, if
you define extents around groups of objects this group will be set aside
(inside an invisible sphere). When a ray needs to be compared with the members
of the group the program will first check if the ray hits the sphere. If it
does not, then NONE of the objects in the group need to be checked. This can
cause a MASSIVE speed up (especially if you have a very complex pixture with
lots of nested extents).

-> Why do concentric spheres cause the program to blow up?

They don't. It's the one sphere and one quad that have a MIRROR parameter of 1.
(perfect reflection make things go BOOM :-). Use a MIRROR of around .9 and it
works fine. Your guess about infinite reflection is correct. Even though rays
follow the inverse square law, they don't fall off fast enough to prevent stack
overflow with perfect mirroring.

-> Who owns the pictures I create?

You do. I have NO rights to original pictures created by yourself with
DBW_Render.

-> Is there any required order to the items in the .DAT input files?

No, do whatever order you feel most comfortable with.

-> What format must input numbers be in?

Very flexible, 3.0 == 3. == 3 and 0.0 == 0. == 0 == .0

Hope these answers help!
Best regards,
dave


