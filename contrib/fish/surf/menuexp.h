#ifndef MENUEXP_H_FILE
#define MENUEXP_H_FILE

#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif  /* INTUITION_INTUITION_H */

#include <aros/oldprograms.h>

extern USHORT *AbortDrawPtr;
extern USHORT *DebugOnPtr;

#define AbortDraw (*AbortDrawPtr & CHECKED)
#define ClrAbort() { *AbortDrawPtr &= ~CHECKED; }
#define DebugOn (*DebugOnPtr & CHECKED)

#endif /* MENUEXP_H_FILE */
