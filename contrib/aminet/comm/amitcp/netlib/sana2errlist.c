/*
 * sana2errlist.c --- Errlist for Sana-II Errors
 *
 * Author: ppessi <Pekka.Pessi@hut.fi>
 *
 * Copyright © 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                  Helsinki University of Technology, Finland.
 *                  All rights reserved.
 *
 * Created      : Sun Apr 18 16:55:44 1993 ppessi
 * Last modified: Thu Mar 24 06:42:34 1994 ppessi
 *
 * $Log$
 * Revision 1.1  2001/12/25 22:28:13  henrik
 * amitcp
 *
 * Revision 1.4  1994/03/24  17:04:36  jraja
 * Added autodoc + small changes (ppessi).
 *
 * Revision 1.4  1994/03/24  16:33:42  ppessi
 * Added autodoc
 *
 * Revision 1.3  1993/06/03  23:27:19  ppessi
 * Cosmetic changes for version 1.0
 *
 * Revision 1.2  1993/05/16  23:31:43  ppessi
 * Added a new RCS ID definition. Moved this file to the sana2 support library.
 *
 * Revision 1.1  1993/04/19  02:15:57  ppessi
 * Initial revision
 */

/****** sana2.lib/sana2wire_errlist *******************************************

    NAME
	sana2wire_errlist - SANA-II device error messages

    SYNOPSIS
	#include <devices/sana2.h>

	const char * const sana2wire_errlist[];
	const short sana2wire_nerr;

	const char * const io_errlist[];
	const short io_nerr;

	const char * const sana2io_errlist[];
	const short sana2io_nerr;

    FUNCTION
	These arrays contain error messages corresponding to the error codes
	returned by SANA-II devices.  The list sana2wire_errlist contains
	error messages for error codes in the structure IOSana2Req field
	ios2_WireError.  The sana2wire_nerr contains number of error messages
	in the array sana2wire_errlist.

	The list io_errlist contains error messages for normal IO errors
	common for all IO devices.  The io_nerr contains number of error
	messages in the array io_errlist.

	The list sana2io_errlist contains error messages for SANA-II-specific
	IO errors.  The sana2io_nerr contains number of error messages in the
	array sana2io_errlist.

    SEE ALSO
	Sana2PrintFault(), sana2perror()

*******************************************************************************
*/

const char * const sana2io_errlist[] = 
{
  "No error",				/* S2ERR_NO_ERROR */
  "Resource allocation failure",	/* S2ERR_NO_RESOURCES */
  "Unknown error code (2)", 
  "Invalid argument",			/* S2ERR_BAD_ARGUMENT */
  "Inappropriate state",		/* S2ERR_BAD_STATE */
  "Invalid address",			/* S2ERR_BAD_ADDRESS */
  "Requested packet too large",		/* S2ERR_MTU_EXCEEDED */
  "Unknown error (7)",
  "Command is not supporter",		/* S2ERR_NOT_SUPPORTED */
  "Driver software error detected",	/* S2ERR_SOFTWARE */
  "Device driver is offline"		/* S2ERR_OUTOFSERVICE */
};

const short sana2io_nerr = 
{ sizeof sana2io_errlist/sizeof sana2io_errlist[0] };

const char * const io_errlist[] = 
{ 
  "Unknown error",			/* 0 */
  "Device or unit failed to open",	/* IOERR_OPENFAIL */
  "Request aborted",			/* IOERR_ABORTED */
  "Command not supported by device",	/* IOERR_NOCMD */
  "Invalid length",			/* IOERR_BADLENGTH */
  "Invalid address",			/* IOERR_BADADDRESS */
  "Requested unit is busy",		/* IOERR_UNITBUSY */
  "Hardware self-test failed"		/* IOERR_SELFTEST */
};

const short io_nerr = 
{ sizeof io_errlist/sizeof io_errlist[0] };

const char * const sana2wire_errlist[] = 
{
  "Generic error: 0",                   /* S2WERR_GENERIC_ERROR */
  "Unit not configured",		/* S2WERR_NOT_CONFIGURED */
  "Unit is currently online",		/* S2WERR_UNIT_ONLINE */
  "Unit is currently offline",		/* S2WERR_UNIT_OFFLINE */
  "Protocol already tracked",		/* S2WERR_ALREADY_TRACKED */
  "Protocol not tracked",		/* S2WERR_NOT_TRACKED */
  "Buff management function error",	/* S2WERR_BUFF_ERROR */
  "Source address error",		/* S2WERR_SRC_ADDRESS */
  "Destination address error",		/* S2WERR_DST_ADDRESS */
  "Broadcast address error",		/* S2WERR_BAD_BROADCAST */
  "Multicast address error",		/* S2WERR_BAD_MULTICAST */
  "Multicast address list full",	/* S2WERR_MULTICAST_FULL */
  "Unsupported event class",		/* S2WERR_BAD_EVENT */
  "Statdata failed sanity check",	/* S2WERR_BAD_STATDATA */
  "Unknown wire error (14)",
  "Attempt to config twice",		/* S2WERR_IS_CONFIGURED */
  "Null pointer detected"		/* S2WERR_NULL_POINTER */
};

const short sana2wire_nerr = 
{ sizeof sana2wire_errlist/sizeof sana2wire_errlist[0] };
