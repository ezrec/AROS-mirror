#include "dopuslib.h"


/*****************************************************************************

    NAME */

	AROS_LH2(void, ActivateStrGad,

/*  SYNOPSIS */
	AROS_LHA(struct Gadget *, gadget, A0),
	AROS_LHA(struct Window *, window, A1),

/*  LOCATION */
	struct Library *, DOpusBase, 62, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	if (!(gadget->Flags & GFLG_DISABLED)) {
	
		struct StringInfo *si;
		STRPTR str;
		int len;
		
		si = (struct StringInfo *)gadget->SpecialInfo;
		str = si->Buffer;
		
		/* Compute the length of chars in the buffer */
		
		for (len = 0; *str ++; len++ )
			;
			
		if (gadget->Activation & GACT_STRINGRIGHT) {
			si->BufferPos = 0;
		
		} else {
			si->BufferPos = len;
		}
		
		RefreshGList(gadget, window, NULL, 1);
		
		ActivateGadget(gadget, window, NULL);
	}
	

	return;
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH2(void, RefreshStrGad,

/*  SYNOPSIS */
	AROS_LHA(struct Gadget *, gadget, A0),
	AROS_LHA(struct Window *, window, A1),

/*  LOCATION */
	struct Library *, DOpusBase, 63, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)
	
	struct StringInfo *si;
	
	si = (struct StringInfo *)gadget->SpecialInfo;
	
	si->BufferPos = 0;
	
	Forbid();
	gadget->Flags |= GFLG_GADGHNONE;
	Permit();
	
	RefreshGList(gadget, window, NULL, 1);

	Forbid();	
	gadget->Flags &= ~GFLG_GADGHNONE;
	Permit();

	AROS_LIBFUNC_EXIT
}
