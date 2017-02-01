Description: 	Thumbnail generator
Version: 		1.1
Date: 			10 Dec 10
Author: 		Yannick Erb
Submitter: 		Yannick Erb
Homepage: 		http://www.dusabledanslherbe.eu/AROSPage/
Category: 		utility/shell
License: 		APL

This little tool creates thumbnails icons for any image format recognised by the datatype system

Options:

FILE/A				--  input file (WILDCARD Accepted)
ALL/S,				--  deep scan into sub directories
QUIET/S,			--  suppress all output
W=WIDTH/N,			--  Thumbnails width (default 128pixels)
H=HEIGHT/N,			--  Thumbnails height (default 128pixels)
F=FORCEASPECT/S,	--  force aspect ratio
M=METHOD,			--  NEAREST(default), BILINEAR, AVERAGE
DEFTOOL				--  Default tool to attach to the icon (default : multiview)

Best results are obtained with M=AVERAGE, that might however take more time.
