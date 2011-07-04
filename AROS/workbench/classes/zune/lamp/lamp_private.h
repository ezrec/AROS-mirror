#ifndef _LAMP_PRIVATE_H_
#define _LAMP_PRIVATE_H_

/*
    Copyright © 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <libraries/mui.h>

/*** Instance data **********************************************************/
struct Lamp_DATA
{
    /*- Private ------------------------------------------------------------*/

    /*- Protected ----------------------------------------------------------*/

    /*- Public -------------------------------------------------------------*/
    ULONG               lmp_Type;
    ULONG               lmp_ColorSpec;
    ULONG               lmp_Color[3];
    ULONG               lmp_ColorType;
    struct MUI_PenSpec  lmp_PenSpec;
};

#endif /* _LAMP_PRIVATE_H_ */
