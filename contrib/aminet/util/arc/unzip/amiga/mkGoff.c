/* Write out a fragment of assembly source giving offsets in globals.h: */

#define UNZIP_INTERNAL
#include "unzip.h"
#include "crypt.h"
#include <stdio.h>

/* Keep this in sync with the definition of redirSlide in unzpriv.h: */
#ifdef DLL
#  define pG_redirSlide pG->redirect_pointer
#else
#  define pG_redirSlide pG->area.Slide
#endif

void main(void)
{
    struct Globals *pG = (void *) 0L;

    printf("bb              EQU     %lu\n", &pG->bb);
    printf("bk              EQU     %lu\n", &pG->bk);
    printf("wp              EQU     %lu\n", &pG->wp);
#ifdef FUNZIP
    printf("in              EQU     %lu\n", &pG->in);
#else
    printf("incnt           EQU     %lu\n", &pG->incnt);
    printf("inptr           EQU     %lu\n", &pG->inptr);
    printf("csize           EQU     %lu\n", &pG->csize);
    printf("mem_mode        EQU     %lu\n", &pG->mem_mode);
#endif
    printf("slide           EQU     %lu\n", &pG_redirSlide);
    printf("SIZEOF_slide    EQU     %lu\n", sizeof(pG_redirSlide));
    printf("CRYPT           EQU     %d\n", CRYPT);
    exit(0);
}
