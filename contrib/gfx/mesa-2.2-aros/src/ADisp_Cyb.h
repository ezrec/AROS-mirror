
BOOL Cyb_Standard_init(struct amigamesa_context *c,struct TagItem *tagList);
BOOL Cyb_Standard_init_db(struct amigamesa_context *c,struct TagItem *tagList);

/*
	CyberGFX uses Amiga_AGA misc rutines so ADISP_AGA have to be defined
*/

#ifndef ADISP_AGA
#define ADISP_AGA
#endif

#include "ADisp_AGA.h"

