#ifndef _AROS
#include "/common/mcc_common.h"
#else
#include "mcc_common.h"
#endif

#ifndef INTUITION_CLASSES_H
#include <intuition/classes.h>
#endif

extern struct MUI_CustomClass *NGR_Class;

/*
#define NGroupObject NewObject(NGR_Class->mcc_Class,NULL
*/

struct NGRData
{
  LONG DoDraw;
};


#ifdef MORPHOS
struct EmulLibEntry NGR_Dispatcher;
#elif defined(_AROS)
AROS_UFP3(ULONG, NGR_Dispatcher,
    AROS_UFPA(struct IClass *, cl, A0),
    AROS_UFPA(Object *, obj, A2),
    AROS_UFPA(Msg, msg, A1));
#else
extern ULONG ASM SAVEDS NGR_Dispatcher( REG(a0) struct IClass *cl GNUCREG(a0), REG(a2) Object *obj GNUCREG(a2), REG(a1) Msg msg GNUCREG(a1) );
#endif


extern struct MUI_CustomClass *NGR_Create(void);
extern void NGR_Delete(void);
