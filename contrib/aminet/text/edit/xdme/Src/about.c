
/* $Id$

*/

#include "defs.h"

/* Version-Number :-) */
Prototype const UBYTE * version;
const UBYTE * version = "$VER: XDME " VERSION " (" VERDATE ") by "
			"digulla@fh-konstanz.de";

/*DEFHELP #cmd misc ABOUT - display information about XDME */

DEFUSERCMD("about", 0, CF_VWM|CF_ICO,void, do_about, (void),)
{
    static struct EasyStruct es =
    {
	sizeof (struct EasyStruct),
	NULL,
	"About XDME",
	"XDME " VERSION " compiled " __DATE__ "\n"
	"Author: Aaron \"Optimizer\" Digulla\n"
	"        (digulla@fh-konstanz.de)\n"
	"        Th.-Heuss-Str. 8\n"
	"        78467 Konstanz\n"
	"        Germany\n\n"
	"Co-Authors: Dirk Heckmann (Arexx),\n"
	"            Bernd \"0\" Noll (Keys,\n"
	"            Vars, Menus and more)\n"
	"            Todd M. Lewis (blocks)"
	, "Thanks"
    };

    EasyRequest ((Ep ? Ep->win : NULL), &es, NULL);

} /* do_about */


