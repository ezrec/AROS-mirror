/* Objectheader

	Name:		extheader.c
	Version:	$VER: extheader.c 1.1 (13.06.1998)
	Description:	C header for xad externals
	Author:		SDI
	Distribution:	PD

 1.0   13.06.98 : first version
*/

#include <libraries/xadmaster.h>

/* To make this a extern Object module it is necessary to force this
structure to be the really first stuff in the file. */

extern char version[];
extern struct xadClient FirstClient;

const static struct xadForeman __attribute__((used)) ForeMan =
{ XADFOREMAN_SECURITY, XADFOREMAN_ID, XADFOREMAN_VERSION, 0, version,
&FirstClient };

