/*
**	JobHandler.c
**
**	Handler routines for job nodes
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

BOOL
HandleXEMJob(JobNode *UnusedJob)
{
	HandleExternalEmulation();

	return(FALSE);
}
