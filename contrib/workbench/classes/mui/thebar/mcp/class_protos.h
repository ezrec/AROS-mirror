/***************************************************************************

 TheBar.mcc - Next Generation Toolbar MUI Custom Class
 Copyright (C) 2003-2005 Alfonso Ranieri
 Copyright (C) 2005-2013 by TheBar.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 TheBar class Support Site:  http://www.sf.net/projects/thebar

 $Id$

***************************************************************************/

/* utils.c */
#if !defined(__MORPHOS__)
#if defined(__AROS__)
IPTR VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, ...);
#else
Object * VARARGS68K DoSuperNew(struct IClass *cl, Object *obj, ...);
#endif
#endif

#if !defined(__MORPHOS__) && !defined(__AROS__)
int stch_l(const char *chr_ptr, long *u_ptr);
#endif

Object *obutton(const void *text, const void *help);
Object *ocycle(STRPTR *array, const void *key, const void *help);
Object *ocheck(const void *key, const void *help);
Object *oslider(const void *key, const void *help, LONG min, LONG max);
Object *opop(ULONG type, const void *key);
Object *opoppen(const void *key, const void *title, const void *help);
Object *opopfri(const void *key, const void *title, const void *help);
Object *opopback(ULONG gradient, const void *key, const void *title, const void *help);
Object *opopframe(const void *key, const void *title, const void *help);
#if !defined(__amigaos4__) && !defined(__MORPHOS__) && !defined(__AROS__)
void drawGradient(Object *obj, struct MUIS_TheBar_Gradient *grad);
#endif
ULONG getKeyChar(const char *string);

#if !defined(__amigaos4__) && !defined(__MORPHOS__) && !defined(__AROS__)
/* coloradjust.c */
void freeColoradjust ( void );
ULONG initColoradjust ( void );

/* penadjust.c */
void freePenadjust ( void );
ULONG initPenadjust ( void );

/* backgroundadjust.c */
void freeBackgroundadjust ( void );
ULONG initBackgroundadjust ( void );

/* poppen.c */
void freePoppen ( void );
ULONG initPoppen ( void );

/* popbackground.c */
void freePopbackground ( void );
ULONG initPopbackground ( void );
#endif

