/*
 * $Id$
 *
 * $Log$
 * Revision 1.1  2001/10/06 20:18:47  digulla
 * Initial revision
 *
 * Revision 1.2  1994/09/09  12:29:10  digulla
 * adjusted keymappings to changes in src/search.c
 *
 * Revision 1.1  1994/08/14  14:26:15  digulla
 * Initial revision
 *
 */

CONST TWOSTRINGS defmap[] =
{
	"esc",      "esc",              /* Map some special keys */
	"c-esc",    "recall",
	"enter",    "return",
	"up",       "up",
	"down",     "down",
	"right",    "right",
	"left",     "left",
	"bs",       "bs",
	"del",      "del",
	"tab",      "indent . .ct tab",
	"s-tab",    "indent . -.ct backtab",
	"return",   "return",
	"s-return", "firstnb down",
	"s-spc",    "( )",              /* shift space to space */
	"s-del",    "deline",
	"c-c",      "null",             /* break.. map to a nop */
	"help",     "ref",

	/* Follow Styleguide */
	"A-n",      "newwindow arpload",
	"A-o",      "arpload",
	"A-s",      "saveold",
	"A-a",      "arpsave",
	"A-q",      "quit",
	"A-Q",      "quitall",
	"A-z",      "undo",
	"A-x",      "bsave t:xdme_clip bdelete",
	"A-c",      "bsave t:xdme_clip unblock",
	"A-v",      "insfile t:xdme_clip",
	"A-f",      "escimm (findstr \`) next",
	"As-f",     "escimm (findstr \`) prev",
	"A-r",      "escimm (findstr \`) escimm (repstr \`) next",
	"As-r",     "escimm (findstr \`) escimm (repstr \`) prev",
	"A-e",      "escimm (execute \`)",
	"A-b",      "block",
	"A-i",      "iconify",

	"a-up",     "scrollup",
	"a-down",   "scrolldown",
	"a-left",   "wleft",
	"a-right",  "wright",

	"s-up",     "pageup",
	"s-down",   "pagedown",
	"s-right",  "pageright",
	"s-left",   "pageleft",

	"c-up",     "top",
	"c-down",   "bottom",
	"c-right",  "last",
	"c-left",   "set tmp $colno firstnb if x=$tmp first",

	"c-j",      "join",
	"c-i",      "insertmode toggle",
	"c-del",    "remeol",
	"c-w",      "wordwrap toggle",
	"c-g",      "escimm (goto )",
	"c-l",      "(^L)",                 /* FF */
	"c-e",      "(^[)",                 /* ESC */
	"c-m",      "match",

	"a-l",      "wleft",
	"a-r",      "wright",
	"a-n",      "next",
	"as-n",     "replace next",
	"a-p",      "prev",
	"as-p",     "replace prev",
	"a-f",      "reformat",
	"a-i",      "indent b .t",
	"a-I",      "indent b -.t",
	"a-b",      "block",
	"a-c",      "bcopy",
	"a-d",      "bdelete",
	"a-m",      "bmove",
	"a-s",      "barpsave",

	"L-lmb",    "tomouse",      /*  left button                 */
	"L-mmo",    "tomouse",      /*  mouse move w/left held down */
	"M-mmb",    "clipins",      /*  insert from clipboard       */
	"R-rmb",    "iconify",      /*  right button                */
	NULL, NULL
};

