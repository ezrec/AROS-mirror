#ifndef ABOUTBOX_MCC_H
#define ABOUTBOX_MCC_H

/*
 *  Aboutbox.mcc
 *
 *  Copyright © 2007 Christian Rosentreter <tokai@binaryriot.org>
 *  All rights reserved.
 */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef LIBRARIES_IFFPARSE_H
#include <libraries/iffparse.h>
#endif


#define MUIC_Aboutbox  "Aboutbox.mcc"
#define AboutboxObject MUI_NewObject(MUIC_Aboutbox


/*   attributes
 */
#define MUIA_Aboutbox_Credits            0xFED10001    /* [I..] STRPTR  v20.1  */
#define MUIA_Aboutbox_LogoData           0xFED10002    /* [I..] APTR    v20.2  */
#define MUIA_Aboutbox_LogoFallbackMode   0xFED10003    /* [I..] ULONG   v20.2  */
#define MUIA_Aboutbox_LogoFile           0xFED10004    /* [I..] STRPTR  v20.2  */
#define MUIA_Aboutbox_Build              0xFED1001E    /* [I..] STRPTR  v20.12 */


/*   methods
 */



/*   special values
 */

/*
 *   the fallback mode defines in which order Aboutbox.mcc tries to get valid image
 *   data for the logo:
 *
 *   D = PROGDIR:<executablefilename>.info
 *   E = file specified in MUIA_Aboutbox_LogoFile
 *   I = data specified with MUIA_Aboutbox_LogoData
 */
#define MUIV_Aboutbox_LogoFallbackMode_NoLogo     0
#define MUIV_Aboutbox_LogoFallbackMode_Auto       MAKE_ID('D' , 'E' , 'I' , '\0')


/*   messages
 */


#endif /* ABOUTBOX_MCC_H */

