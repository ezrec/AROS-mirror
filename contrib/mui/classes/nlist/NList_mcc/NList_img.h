#include "/common/mcc_common.h"


extern struct MUI_CustomClass *NLI_Class;

#define NImageObject NewObject(NLI_Class->mcc_Class,NULL

struct NLIData
{
  LONG DoDraw;
};


#ifdef MORPHOS
struct EmulLibEntry NLI_Dispatcher;
#else
extern ULONG ASM SAVEDS NLI_Dispatcher( REG(a0) struct IClass *cl GNUCREG(a0), REG(a2) Object *obj GNUCREG(a2), REG(a1) Msg msg GNUCREG(a1) );
#endif

extern struct MUI_CustomClass *NLI_Create(void);
extern void NLI_Delete(void);
