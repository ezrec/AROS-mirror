#ifndef APPLICATION_H
#define APPLICATION_H

extern ULONG applicationmask;

void InitApplication(void);
BOOL CleanupApplication(void);
#ifndef __AROS__
LONG ExtendedRequestArgs(REG(a0,struct Window *,Window),REG(a1,struct EasyStruct *,easyStruct),REG(a2,ULONG *,IDCMP_ptr),REG(d1,struct DeveloperAttack *,devAttack),REG(a3,APTR, ArgList));
struct Window *ExtendedBuildRequestArgs(REG(a0,struct Window *,RefWindow),REG(a1,struct EasyStruct *,easyStruct),REG(d0,ULONG ,IDCMP),REG(d1,struct DeveloperAttack *,devAttack),REG(a3,APTR, Args));
#endif
#endif

