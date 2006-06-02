/*
 *		GUI.H											vi:ts=4
 *
 *      Copyright (c) Eddy Carroll, September 1994.
 *
 *		Constants used when creating the SnoopDos GUI
 */

#define IE_SHIFT			(IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT)
#define IE_ALT				(IEQUALIFIER_LALT   | IEQUALIFIER_RALT)
#define IE_CTRL				(IEQUALIFIER_CONTROL)
#define IE_ALL				(IE_SHIFT | IE_ALT | IE_CTRL)

#define HELPKEY				0x5F	/* Keyboard raw code for HELP key		*/
#define TABKEY				0x42	/* Keyboard raw code for TAB key		*/

#define DEF_WINDOW_WIDTH	724		/* Default window width					*/
#define DEF_WINDOW_HEIGHT	256		/* Default window height				*/

#define MAIN_MARGIN			10		/* Minimum margin for main window		*/
#define FUNC_MARGIN			20		/* Margin for functions window			*/
#define SET_MARGIN			15		/* Margin for settings window			*/
#define FORM_MARGIN			15		/* Margin for format window				*/

#define MAIN_NARROW			0		/* Indexes narrow button width			*/
#define MAIN_NARROW_TOGGLE	1		/* Narrow button that toggles			*/
#define MAIN_WIDE			2		/* Indexes wide button width			*/
#define MAIN_WIDE_INVIS		3		/* As above, but button is invisible	*/
#define MAIN_STATUS			4		/* Indexes status line width			*/
#define MAIN_NUMWIDTHS		5		/* Number of widths supported			*/

/*
 *		These next two pairs are for 1:2 and 1:1 aspect screens respectively
 */
#define LO_GADGET_HEIGHT	4		/* Additional height for each gadget	*/
#define LO_GADGET_SPACING	10		/* Vert spacing between most gadgets	*/

#define HI_GADGET_HEIGHT	6		/* Additional height for each gadget	*/
#define HI_GADGET_SPACING	12		/* Vert spacing between most gadgets	*/

#define HSCROLL_SHIFT_JUMP	5		/* # of chars to jump for shift lft/rgt	*/
#define VSCROLL_WIDTH		16		/* Width of vertical scroll gadget		*/
#define HSCROLL_HEIGHT1		11		/* Height of horz. scroll (1:1)			*/
#define HSCROLL_HEIGHT2		9		/* Height of horz. scroll (2:1)			*/
#define BOX_LEFT_MARGIN		2		/* Left margin of text inside box		*/

#define INVIS_LEFT_EDGE		-500	/* Position to make gadget invisible	*/

#define FBOX_NOSELECT		255		/* No entry selected in format window	*/
#define FBOX_SELECTLEFT		127		/* Left box selected in format window	*/
#define END_EDITLIST		(-1)	/* Signifies end of format edit list	*/

#define FORMAT_SELECTED		1		/* Highlight line when outputting it	*/
#define FORMAT_UNSELECTED	0		/* Don't highlight line when outputting	*/

#define FORMAT_LEFTBOX		0		/* Dragging from left box				*/
#define FORMAT_RIGHTBOX		1		/* Dragging from right box				*/

/*
 *		Delay used when briefly highlighting a gadget after a user
 *		presses a keyboard equivalent
 */
#define PRESSGAD_DELAY		5		/* Delay in 50ths of a second			*/

