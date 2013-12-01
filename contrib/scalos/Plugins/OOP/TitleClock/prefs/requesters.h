/*
 *  requesters.h    Protos for ASL requester and such like
 *
 * $Date$
 * $Revision$
 *
 *  0.1
 *  20010804    DM  + Created
 *
 */

#ifndef REQUESTERS_H
#define REQUESTERS_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif

STRPTR  SaveFileReq(struct Window *win);
STRPTR  OpenFileReq(struct Window *win);


#endif /* REQUESTERS_H */

