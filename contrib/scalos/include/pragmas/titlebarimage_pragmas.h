#ifndef PRAGMAS_TITLEBAR_PRAGMAS_H
#define PRAGMAS_TITLEBAR_PRAGMAS_H

/* "titlebar.image"*/
/*--- functions in V40 or higher ---*/
/**/

#if defined(LATTICE) || defined(__SASC) || defined(_DCC)
#pragma libcall TitlebarImageBase ObtainTBIClass 1e 0
#endif /* */

#endif /* PRAGMAS_TITLEBAR_PRAGMAS_H */
