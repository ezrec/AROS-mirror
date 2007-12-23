#ifndef SDI_HOOK_H
#define SDI_HOOK_H

#define ENTRY(func) (APTR)func

#define MakeStaticHook(hookname, funcname) static struct Hook hookname =   \
    {{NULL, NULL}, (HOOKFUNC)funcname, NULL, NULL}

#endif
