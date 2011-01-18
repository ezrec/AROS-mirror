/* Internal tiny implementation of necessary libc functions. */

#include <exec/tasks.h>
#include <proto/exec.h>
#include <sys/types.h>
#include <setjmp.h>
#include <string.h>

#include "libc.h"

/*
 * Note that we use tc_UserData in order to store a pointer to our
 * data. Since applications may use tc_UserData for their own purpose,
 * we save old value upon function entry and restore it upon exit.
 * libc_setcontext() and libc_unsetcontext() are responsible for
 * doing this.
 * I hope this will not clash with some weird stuff (like using tc_UserData
 * inside exceptions). In theory it is possible to keep own list of contexts
 * and associated tasks, but this would be slow and overcomplicated.
 */

static inline struct LibcData *GetLibcData(void)
{
    struct Task *me;

    me = FindTask(NULL);
    return (struct LibcData *)me->tc_UserData;
}

void _abort(char *file, unsigned int line)
{
    struct LibcData *ctx = GetLibcData();

    ctx->exception_arg1 = file;
    ctx->exception_arg2 = line;
    longjmp(ctx->abort_buf, EXCEPTION_ABORT);
}

void *calloc(size_t nmemb, size_t size)
{
    struct LibcData *data = GetLibcData();

    /* Note that this implies that the pool was created with MEMF_CLEAR flag */
    return AllocVecPooled(data->pool, nmemb * size);
}

void* xmalloc (size_t size)
{
    struct LibcData *data = GetLibcData();
    void *mem;
    
    mem = AllocVecPooled(data->pool, size);
    if (!mem)
        longjmp(data->abort_buf, EXCEPTION_NO_MEMORY);

    return mem;
}
    
    

void libc_setcontext(struct LibcData *data)
{
    struct Task *me = FindTask(NULL);
    data->userdata = me->tc_UserData;

    me->tc_UserData = data;
}

void libc_unsetcontext(void)
{
    struct Task *me = FindTask(NULL);
    struct LibcData *data = (struct LibcData *)me->tc_UserData;
    
    me->tc_UserData = data->userdata;
}
