#ifndef __MFSTRING_H
#define __MFSTRING_H

#define CATCOMP_NUMBERS
#undef  CATCOMP_STRINGS
#undef  CATCOMP_ARRAY
#include "MUIMineLocale.h"


/*
    function prototypes
*/
void OpenStrings(void);
void CloseStrings(void);
STRPTR GetStr(LONG id);



#endif // __MFSTRINGS_H

