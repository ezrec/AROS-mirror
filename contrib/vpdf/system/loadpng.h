#ifndef	LOADPNG_H
#define	LOADPNG_H

#include "chunky.h"

Texture	*PNGLoadARGB32(char	*fname);
Texture	*PNGLoadRGB24(char *fname);
int pngGetDimmensions(char *fname, int *width, int *height);

#endif
