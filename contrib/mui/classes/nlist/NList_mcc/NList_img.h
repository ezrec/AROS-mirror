#ifndef _AROS
#include "/common/mcc_common.h"
#else
#include "mcc_common.h"
#endif

extern struct MUI_CustomClass *NLI_Class;

#define NImageObject NewObject(NLI_Class->mcc_Class,NULL

struct NLIData
{
  LONG DoDraw;
};


#ifdef MORPHOS
struct EmulLibEntry NLI_Dispatcher;
#elif defined(_AROS)
AROS_UFP3(ULONG, NLI_Dispatcher,
    AROS_UFPA(struct IClass *, cl, A0),
    AROS_UFPA(Object *, obj, A2),
    AROS_UFPA(Msg, msg, A1));
#else
extern ULONG ASM SAVEDS NLI_Dispatcher( REG(a0) struct IClass *cl GNUCREG(a0), REG(a2) Object *obj GNUCREG(a2), REG(a1) Msg msg GNUCREG(a1) );
#endif

extern struct MUI_CustomClass *NLI_Create(void);
extern void NLI_Delete(void);
