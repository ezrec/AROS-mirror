/*	gadgets for rubik   MJR 1st Feb '90 (ain't touched this for a month! */

#include "header.c"

struct TextAttr ta = 
{
"topaz.font",
8,
0,
FPF_ROMFONT
};

unsigned short rollrightdata[] =
{
0x0000,0x01E0,0x0000,0x01E0,0x0000,0x01E0,0x0000,0x03C0,
0x0000,0x03C0,0xFF00,0x07C0,0xFE00,0x0780,
0xFC00,0x0F80,0xFE00,0x3F00,0xFF80,0xFE00,0xFFFF,0xFC00,
0xCFFF,0xF800,0x83FF,0xE000,0x007F,0x0000,
0x0000,0x01E0,0x0000,0x01E0,0x0000,0x01E0,0x0000,0x03C0,
0x0000,0x03C0,0xFF00,0x07C0,0xFE00,0x0780,
0xFC00,0x0F80,0xFE00,0x3F00,0xFF80,0xFE00,0xFFFF,0xFC00,
0xCFFF,0xF800,0x83FF,0xE000,0x007F,0x0000,
0x0000,0x01E0,0x0000,0x01E0,0x0000,0x01E0,0x0000,0x03C0,
0x0000,0x03C0,0xFF00,0x07C0,0xFE00,0x0780,
0xFC00,0x0F80,0xFE00,0x3F00,0xFF80,0xFE00,0xFFFF,0xFC00,
0xCFFF,0xF800,0x83FF,0xE000,0x007F,0x0000
};

unsigned short rollleftdata[] =
{
0x007F,0x0000,0x83FF,0xE000,0xCFFF,0xF800,0xFFFF,0xFC00,
0xFF80,0xFE00,0xFE00,0x3F00,0xFC00,0x0F80,
0xFE00,0x0780,0xFF00,0x07C0,0x0000,0x03C0,0x0000,0x03C0,
0x0000,0x01E0,0x0000,0x01E0,0x0000,0x01E0,
0x007F,0x0000,0x83FF,0xE000,0xCFFF,0xF800,0xFFFF,0xFC00,
0xFF80,0xFE00,0xFE00,0x3F00,0xFC00,0x0F80,
0xFE00,0x0780,0xFF00,0x07C0,0x0000,0x03C0,0x0000,0x03C0,
0x0000,0x01E0,0x0000,0x01E0,0x0000,0x01E0,
0x007F,0x0000,0x83FF,0xE000,0xCFFF,0xF800,0xFFFF,0xFC00,
0xFF80,0xFE00,0xFE00,0x3F00,0xFC00,0x0F80,
0xFE00,0x0780,0xFF00,0x07C0,0x0000,0x03C0,0x0000,0x03C0,
0x0000,0x01E0,0x0000,0x01E0,0x0000,0x01E0
};

unsigned short yawleftdata[] =
{
0x001E,0x03E0,0x7E00,0xC000,0xE000,0xFFC0,0xFFE0,
0xFFF0,0xFFFA,0xFFFC,0xFFF8,0xFFF0,0x7FE0,0x1FC0,
0x001E,0x03E0,0x7E00,0xC000,0xE000,0xFFC0,0xFFE0,
0xFFF0,0xFFFA,0xFFFC,0xFFF8,0xFFF0,0x7FE0,0x1FC0,
0x001E,0x03E0,0x7E00,0xC000,0xE000,0xFFC0,0xFFE0,
0xFFF0,0xFFFA,0xFFFC,0xFFF8,0xFFF0,0x7FE0,0x1FC0
};

unsigned short yawrightdata[] =
{
0xF000,0x0F80,0x00FC,0x0006,0x000E,0x07FE,0x0FFE,
0x1FFE,0xBFFE,0x7FFE,0x3FFE,0x1FFE,0x0FFC,0x07F0,
0xF000,0x0F80,0x00FC,0x0006,0x000E,0x07FE,0x0FFE,
0x1FFE,0xBFFE,0x7FFE,0x3FFE,0x1FFE,0x0FFC,0x07F0,
0xF000,0x0F80,0x00FC,0x0006,0x000E,0x07FE,0x0FFE,
0x1FFE,0xBFFE,0x7FFE,0x3FFE,0x1FFE,0x0FFC,0x07F0
};

unsigned short climbdata[] =
{
0x1FF0,0x3FF8,0x2FF8,0x27FC,0x27FC,
0x27FC,0x67FC,0x47FC,0x47FC,0x47FC,
0x43F8,0x81F0,0x80E0,0x8040,0x8080,
0x1FF0,0x3FF8,0x2FF8,0x27FC,0x27FC,
0x27FC,0x67FC,0x47FC,0x47FC,0x47FC,
0x43F8,0x81F0,0x80E0,0x8040,0x8080,
0x1FF0,0x3FF8,0x2FF8,0x27FC,0x27FC,
0x27FC,0x67FC,0x47FC,0x47FC,0x47FC,
0x43F8,0x81F0,0x80E0,0x8040,0x8080
};

unsigned short divedata[] =
{
0x8080,0x8040,0x80E0,0x81F0,0x43F8,
0x47FC,0x47FC,0x47FC,0x67FC,0x27FC,
0x27FC,0x27FC,0x2FF8,0x3FF8,0x1FF0,
0x8080,0x8040,0x80E0,0x81F0,0x43F8,
0x47FC,0x47FC,0x47FC,0x67FC,0x27FC,
0x27FC,0x27FC,0x2FF8,0x3FF8,0x1FF0,
0x8080,0x8040,0x80E0,0x81F0,0x43F8,
0x47FC,0x47FC,0x47FC,0x67FC,0x27FC,
0x27FC,0x27FC,0x2FF8,0x3FF8,0x1FF0
};

struct Image col6i =
{
0,0,
7,7,3,
NULL,
0,6,
NULL
};

struct Image col5i =
{
0,0,
7,7,3,
NULL,
0,5,
NULL
};

struct Image col4i =
{
0,0,
7,7,3,
NULL,
0,4,
NULL
};

struct Image col3i =
{
0,0,
7,7,3,
NULL,
0,3,
NULL
};

struct Image col2i =
{
0,0,
7,7,3,
NULL,
0,2,
NULL
};

struct Image col1i =
{
0,0,
7,7,3,
NULL,
0,1,
NULL
};

struct Image rollright =
{
0,0,				/*	LeftEdge, TopEdge		*/
27,14,3,			/*	Width, Height, Depth	*/
&rollrightdata[0],
7,0,				/*	PlanePick, PlaneOnOff	*/
NULL				/*	Pointer to next Image	*/
};

struct Image rollleft =
{
0,0,				/*	LeftEdge, TopEdge		*/
27,14,3,			/*	Width, Height, Depth	*/
&rollleftdata[0],
7,0,				/*	PlanePick, PlaneOnOff	*/
NULL				/*	Pointer to next Image	*/
};

struct Image yawleft =
{
0,0,				/*	LeftEdge, TopEdge		*/
15,14,3,			/*	Width, Height, Depth	*/
&yawleftdata[0],
7,0,				/*	PlanePick, PlaneOnOff	*/
NULL				/*	Pointer to next Image	*/
};

struct Image yawright =
{
0,0,				/*	LeftEdge, TopEdge		*/
15,14,3,			/*	Width, Height, Depth	*/
&yawrightdata[0],
7,0,				/*	PlanePick, PlaneOnOff	*/
NULL				/*	Pointer to next Image	*/
};

struct Image climb =
{
0,0,				/*	LeftEdge, TopEdge		*/
14,15,3,			/*	Width, Height, Depth	*/
&climbdata[0],
7,0,				/*	PlanePick, PlaneOnOff	*/
NULL				/*	Pointer to next Image	*/
};

struct Image dive =
{
0,0,				/*	LeftEdge, TopEdge		*/
14,15,3,			/*	Width, Height, Depth	*/
&divedata[0],
7,0,				/*	PlanePick, PlaneOnOff	*/
NULL				/*	Pointer to next Image	*/
};

struct IntuiText minustext = {0,7,JAM2,0,1,&ta,(UBYTE *)"-",NULL};

struct IntuiText plustext = {0,7,JAM2,0,1,&ta,(UBYTE *)"+",NULL};

struct IntuiText abouttext = {0,7,JAM2,1,1,&ta,(UBYTE *)"ABOUT",NULL};

struct IntuiText helptext = {0,7,JAM2,0,1,&ta,(UBYTE *)"HELP",NULL};

struct IntuiText slowertext = {0,7,JAM2,0,0,&ta,(UBYTE *)"SLOWER",NULL};

struct IntuiText fastertext = {0,7,JAM2,0,0,&ta,(UBYTE *)"FASTER",NULL};

struct IntuiText backtext = {0,7,JAM2,1,1,&ta,(UBYTE *)"BACK",NULL};

struct IntuiText fronttext = {0,7,JAM2,1,1,&ta,(UBYTE *)"FRONT",NULL};

struct Gadget colour6 =
{
NULL,
224,177,7,7,
GADGHCOMP | GADGIMAGE,
RELVERIFY,
BOOLGADGET,
(APTR)&col6i,
NULL,
NULL,
0,
NULL,
27,
NULL
};

struct Gadget colour5 =
{
NULL,
214,177,7,7,
GADGHCOMP | GADGIMAGE,
RELVERIFY,
BOOLGADGET,
(APTR)&col5i,
NULL,
NULL,
0,
NULL,
26,
NULL
};

struct Gadget colour4 =
{
NULL,
204,177,7,7,
GADGHCOMP | GADGIMAGE,
RELVERIFY,
BOOLGADGET,
(APTR)&col4i,
NULL,
NULL,
0,
NULL,
25,
NULL
};

struct Gadget colour3 =
{
NULL,
224,167,7,7,
GADGHCOMP | GADGIMAGE,
RELVERIFY,
BOOLGADGET,
(APTR)&col3i,
NULL,
NULL,
0,
NULL,
24,
NULL
};

struct Gadget colour2 =
{
NULL,
214,167,7,7,
GADGHCOMP | GADGIMAGE,
RELVERIFY,
BOOLGADGET,
(APTR)&col2i,
NULL,
NULL,
0,
NULL,
23,
NULL
};

struct Gadget colour1 =
{
NULL,
204,167,7,7,
GADGHCOMP | GADGIMAGE,
RELVERIFY,
BOOLGADGET,
(APTR)&col1i,
NULL,
NULL,
0,
NULL,
22,
NULL
};

struct Gadget blueminus = 
{
NULL,
301,186,8,9,
GADGHCOMP,
RELVERIFY,
BOOLGADGET,
NULL,
NULL,
&minustext,
0,
NULL,
21,
NULL
};

struct Gadget greenminus = 
{
NULL,
277,186,8,9,
GADGHCOMP,
RELVERIFY,
BOOLGADGET,
NULL,
NULL,
&minustext,
0,
NULL,
20,
NULL
};

struct Gadget redminus = 
{
NULL,
253,186,8,9,
GADGHCOMP,
RELVERIFY,
BOOLGADGET,
NULL,
NULL,
&minustext,
0,
NULL,
19,
NULL
};

struct Gadget blueplus = 
{
NULL,
301,168,8,9,
GADGHCOMP,
RELVERIFY,
BOOLGADGET,
NULL,
NULL,
&plustext,
0,
NULL,
18,
NULL
};

struct Gadget greenplus = 
{
NULL,
277,168,8,9,
GADGHCOMP,
RELVERIFY,
BOOLGADGET,
NULL,
NULL,
&plustext,
0,
NULL,
17,
NULL
};

struct Gadget redplus = 
{
NULL,
253,168,8,9,
GADGHCOMP,
RELVERIFY,
BOOLGADGET,
NULL,
NULL,
&plustext,
0,
NULL,
16,
NULL
};

/*	need this lot twice for AddGadget to work! (sigh!)	*/

struct Gadget colour6a =
{
NULL,
224,177,7,7,
GADGHCOMP | GADGIMAGE,
RELVERIFY,
BOOLGADGET,
(APTR)&col6i,
NULL,
NULL,
0,
NULL,
27,
NULL
};

struct Gadget colour5a =
{
NULL,
214,177,7,7,
GADGHCOMP | GADGIMAGE,
RELVERIFY,
BOOLGADGET,
(APTR)&col5i,
NULL,
NULL,
0,
NULL,
26,
NULL
};

struct Gadget colour4a =
{
NULL,
204,177,7,7,
GADGHCOMP | GADGIMAGE,
RELVERIFY,
BOOLGADGET,
(APTR)&col4i,
NULL,
NULL,
0,
NULL,
25,
NULL
};

struct Gadget colour3a =
{
NULL,
224,167,7,7,
GADGHCOMP | GADGIMAGE,
RELVERIFY,
BOOLGADGET,
(APTR)&col3i,
NULL,
NULL,
0,
NULL,
24,
NULL
};

struct Gadget colour2a =
{
NULL,
214,167,7,7,
GADGHCOMP | GADGIMAGE,
RELVERIFY,
BOOLGADGET,
(APTR)&col2i,
NULL,
NULL,
0,
NULL,
23,
NULL
};

struct Gadget colour1a =
{
NULL,
204,167,7,7,
GADGHCOMP | GADGIMAGE,
RELVERIFY,
BOOLGADGET,
(APTR)&col1i,
NULL,
NULL,
0,
NULL,
22,
NULL
};

struct Gadget blueminusa = 
{
NULL,
301,186,8,9,
GADGHCOMP,
RELVERIFY,
BOOLGADGET,
NULL,
NULL,
&minustext,
0,
NULL,
21,
NULL
};

struct Gadget greenminusa = 
{
NULL,
277,186,8,9,
GADGHCOMP,
RELVERIFY,
BOOLGADGET,
NULL,
NULL,
&minustext,
0,
NULL,
20,
NULL
};

struct Gadget redminusa = 
{
NULL,
253,186,8,9,
GADGHCOMP,
RELVERIFY,
BOOLGADGET,
NULL,
NULL,
&minustext,
0,
NULL,
19,
NULL
};

struct Gadget blueplusa = 
{
NULL,
301,168,8,9,
GADGHCOMP,
RELVERIFY,
BOOLGADGET,
NULL,
NULL,
&plustext,
0,
NULL,
18,
NULL
};

struct Gadget greenplusa = 
{
NULL,
277,168,8,9,
GADGHCOMP,
RELVERIFY,
BOOLGADGET,
NULL,
NULL,
&plustext,
0,
NULL,
17,
NULL
};

struct Gadget redplusa = 
{
NULL,
253,168,8,9,
GADGHCOMP,
RELVERIFY,
BOOLGADGET,
NULL,
NULL,
&plustext,
0,
NULL,
16,
NULL
};

struct Gadget setupgadget = 
{
NULL,
199,139,53,9,
GADGHBOX,
RELVERIFY,
BOOLGADGET,
NULL,
NULL,
NULL,
0,
NULL,
15,
NULL
};

struct Gadget solvegadget = 
{
&setupgadget,
199,127,53,9,
GADGHBOX,
RELVERIFY,
BOOLGADGET,
NULL,
NULL,
NULL,
0,
NULL,
14,
NULL
};

struct Gadget normalgadget = 
{
&solvegadget,
199,115,53,9,
GADGHBOX,
RELVERIFY,
BOOLGADGET,
NULL,
NULL,
NULL,
0,
NULL,
13,
NULL
};

struct Gadget aboutgadget = 
{
&normalgadget,
265,139,41,9,
GADGHCOMP,
RELVERIFY,
BOOLGADGET,
NULL,
NULL,
&abouttext,
0,
NULL,
12,
NULL
};

struct Gadget helpgadget = 
{
&aboutgadget,
270,115,32,9,
GADGHCOMP,
RELVERIFY,
BOOLGADGET,
NULL,
NULL,
&helptext,
0,
NULL,
11,
NULL
};

struct Gadget slowergadget = 
{
&helpgadget,
261,83,48,7,
GADGHCOMP,
RELVERIFY,
BOOLGADGET,
NULL,
NULL,
&slowertext,
0,
NULL,
10,
NULL
};

struct Gadget fastergadget = 
{
&slowergadget,
200,83,48,7,
GADGHCOMP,
RELVERIFY,
BOOLGADGET,
NULL,
NULL,
&fastertext,
0,
NULL,
9,
NULL
};

struct Gadget backgadget = 
{
&fastergadget,
276,23,33,9,
GADGHCOMP,
RELVERIFY,
BOOLGADGET,
NULL,
NULL,
&backtext,
0,
NULL,
8,
NULL
};

struct Gadget frontgadget = 
{
&backgadget,
197,23,41,9,
GADGHCOMP,
RELVERIFY,
BOOLGADGET,
NULL,
NULL,
&fronttext,
0,
NULL,
7,
NULL
};

struct Gadget rollrightgadget =
{
&frontgadget,
244,56,27,14,		/*	posn. and size	*/
GADGHCOMP |
GADGIMAGE,			/*	flags			*/
RELVERIFY,			/*	activation		*/
BOOLGADGET,			/*	gadget type		*/
(APTR)&rollright,	/*	image pointer	*/
NULL,				/*	alternate image	*/
NULL,				/*	text			*/
0,				/*	mutual exclude	*/
NULL,				/*	special info	*/
1,					/*	gadget ID		*/
NULL				/*	user data		*/
};

struct Gadget rollleftgadget =
{
&rollrightgadget,
244,42,27,14,		/*	posn. and size	*/
GADGHCOMP |
GADGIMAGE,			/*	flags			*/
RELVERIFY,			/*	activation		*/
BOOLGADGET,			/*	gadget type		*/
(APTR)&rollleft,	/*	image pointer	*/
NULL,				/*	alternate image	*/
NULL,				/*	text			*/
0,				/*	mutual exclude	*/
NULL,				/*	special info	*/
2,					/*	gadget ID		*/
NULL				/*	user data		*/
};

struct Gadget yawleftgadget =
{
&rollleftgadget,
242,20,15,14,		/*	posn. and size	*/
GADGHCOMP |
GADGIMAGE,			/*	flags			*/
RELVERIFY,			/*	activation		*/
BOOLGADGET,			/*	gadget type		*/
(APTR)&yawleft,		/*	image pointer	*/
NULL,				/*	alternate image	*/
NULL,				/*	text			*/
0,				/*	mutual exclude	*/
NULL,				/*	special info	*/
3,					/*	gadget ID		*/
NULL				/*	user data		*/
};

struct Gadget yawrightgadget =
{
&yawleftgadget,
257,20,15,14,		/*	posn. and size	*/
GADGHCOMP |
GADGIMAGE,			/*	flags			*/
RELVERIFY,			/*	activation		*/
BOOLGADGET,			/*	gadget type		*/
(APTR)&yawright,	/*	image pointer	*/
NULL,				/*	alternate image	*/
NULL,				/*	text			*/
0,				/*	mutual exclude	*/
NULL,				/*	special info	*/
4,					/*	gadget ID		*/
NULL				/*	user data		*/
};

struct Gadget climbgadget =
{
&yawrightgadget,
222,41,14,15,		/*	posn. and size	*/
GADGHCOMP |
GADGIMAGE,			/*	flags			*/
RELVERIFY,			/*	activation		*/
BOOLGADGET,			/*	gadget type		*/
(APTR)&climb,		/*	image pointer	*/
NULL,				/*	alternate image	*/
NULL,				/*	text			*/
0,				/*	mutual exclude	*/
NULL,				/*	special info	*/
5,					/*	gadget ID		*/
NULL				/*	user data		*/
};


struct Gadget divegadget =
{
&climbgadget,
222,56,14,15,		/*	posn. and size	*/
GADGHCOMP |
GADGIMAGE,			/*	flags			*/
RELVERIFY,			/*	activation		*/
BOOLGADGET,			/*	gadget type		*/
(APTR)&dive,		/*	image pointer	*/
NULL,				/*	alternate image	*/
NULL,				/*	text			*/
0,				/*	mutual exclude	*/
NULL,				/*	special info	*/
6,					/*	gadget ID		*/
NULL				/*	user data		*/
};
