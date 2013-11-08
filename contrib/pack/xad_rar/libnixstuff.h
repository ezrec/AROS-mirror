#ifndef libnixstuff_h
#define libnixstuff_h
#include <proto/exec.h>
#include <proto/wb.h>

struct ExecBase *SysBase;
struct Library *__DOSBase;
struct Library *__UtilityBase;

struct WBenchMsg *_WBenchMsg;

#endif
