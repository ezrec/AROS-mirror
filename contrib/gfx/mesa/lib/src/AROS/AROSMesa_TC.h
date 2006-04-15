#if !defined(AROSMESA_TC_H)
#define AROSMESA_TC_H

/* Private Includes and protos for TrueColor driver */

BOOL arosTC_Standard_init(struct arosmesa_context *c,struct TagItem *tagList);
BOOL arosTC_Standard_init_db(struct arosmesa_context *c,struct TagItem *tagList);

/*
	CyberGFX uses AROSMesa_8bit misc routines so ADISP_AGA have to be defined
*/

#ifndef ADISP_AGA
#define ADISP_AGA
#endif

#include "AROSMesa_8bit.h"

#endif /* AROSMESA_TC_H */
