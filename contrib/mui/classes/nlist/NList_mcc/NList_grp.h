#include "/common/mcc_common.h"


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
#else
extern ULONG ASM SAVEDS NGR_Dispatcher( REG(a0) struct IClass *cl GNUCREG(a0), REG(a2) Object *obj GNUCREG(a2), REG(a1) Msg msg GNUCREG(a1) );
#endif


extern struct MUI_CustomClass *NGR_Create(void);
extern void NGR_Delete(void);
