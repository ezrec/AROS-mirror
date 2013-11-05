/*
    Copyright © 1995-2011, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Open a library.
    Lang: english
*/

#include <aros/debug.h>
#include <exec/execbase.h>
#include <exec/lists.h>
#include <exec/libraries.h>
#include <aros/libcall.h>
#include <proto/exec.h>

#include "exec_intern.h"
#include "exec_debug.h"


#if AROS_SMP
/* Use SpinLocks only for short-time locks. This lock is too big, as
 * the open vector might open other libraries *in other tasks*,
 * so we run into a deadlock for sure. 
 *
 * Maybe use a separate spinlock for every library and one for the
 * library list itself. TODO
 */
//#define USE_SPINLOCK 1
#endif
/*****************************************************************************

    NAME */

        AROS_LH2(struct Library *, OpenLibrary,

/*  SYNOPSIS */
        AROS_LHA(CONST_STRPTR,  libName, A1),
        AROS_LHA(ULONG,         version, D0),

/*  LOCATION */
        struct ExecBase *, SysBase, 92, Exec)

/*  FUNCTION
        Opens a library given by name and revision. If the library does not
        exist in the current system shared library list, the first the
        system ROMTag module list is tried. If this fails, and the DOS is
        running, then the library will be loaded from disk.

    INPUTS
        libName - Pointer to the library's name.
        version - the library's version number.

    RESULT
        Pointer to library structure or NULL.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
        CloseLibrary()

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct Library * library;

    DRAMLIB("OpenLibrary(\"%s\", %ld)", libName, version);

    /* Arbitrate for the library list */
    bug("[OPENLIBRARY] OpenLibrary(\"%s\", %ld) by task %p\n", libName, version, FindTask(NULL));
#if !USE_SPINLOCK
    Forbid();
#else
    bug("[OPENLIBRARY] trying to lock spin %lx\n", &(PrivExecBase(SysBase)->LibList_spinlock));
    LockSpin(&(PrivExecBase(SysBase)->LibList_spinlock));
#endif

    /* Look for the library in our list */
    library = (struct Library *) FindName (&SysBase->LibList, libName);

    /* Something found ? */
    if(library!=NULL)
    {
        /* Check version */
        if(library->lib_Version>=version)
        {
#if USE_SPINLOCK
            /* Does library code expect to be called within Forbid()?
             */
            Forbid();
#endif
            bug("[OPENLIBRARY] call Open vector for %s\n", libName);
            /* Call Open vector */
            library=AROS_LVO_CALL1(struct Library *,
                AROS_LCA(ULONG,version,D0),
                struct Library *,library,1,lib
            );
#if USE_SPINLOCK
            Permit();
#endif
        }
        else
        {
            DRAMLIB("Version mismatch (have %ld, wanted %ld)", library->lib_Version, version);
            library = NULL;
        }
    }

    /*
     *  We cannot handle loading libraries from disk. But this is taken
     *  care of by dos.library (well lddemon really) replacing this
     *  function with a SetFunction() call.
     */

    /* All done. */
#if !USE_SPINLOCK
    Permit();
#else
    UnlockSpin(&(PrivExecBase(SysBase)->LibList_spinlock));
    bug("[OPENLIBRARY] Library %s opened: %lx\n", libName, library);
#endif

    //DRAMLIB("OpenLibrary(\"%s\", %ld) = %p", libName, version, library);
    return library;

    AROS_LIBFUNC_EXIT
} /* OpenLibrary */
