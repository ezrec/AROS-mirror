// FontUtil.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include "debug.h"
#include <proto/scalos.h>

#include <defs.h>
#include <scalos/scalos.h>

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

#include "functions.h"

//----------------------------------------------------------------------------

// local functions

static void make_crc_table(void);

//----------------------------------------------------------------------------

// public data items

//----------------------------------------------------------------------------

// local data items

/* Table of CRCs of all 8-bit messages. */
static ULONG crc_table[256];

/* Flag: has the table been computed? Initially false. */
static BOOL crc_table_computed = FALSE;

//----------------------------------------------------------------------------

/* Make the table for a fast CRC. */
static void make_crc_table(void)
{
	ULONG n;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	for (n = 0; n < 256; n++)
		{
		ULONG c = (ULONG) n;
		ULONG k;

		for (k = 0; k < 8; k++)
			{
			if (c & 1)
				c = 0xedb88320L ^ (c >> 1);
			else
				c = c >> 1;
			}

		crc_table[n] = c;
		}

	crc_table_computed = TRUE;
}

//----------------------------------------------------------------------------

/* Update a running CRC with the bytes buf[0..len-1]--the CRC
	 should be initialized to all 1's, and the transmitted value
	 is the 1's complement of the final running CRC (see the
	 crc() routine below)). */

ULONG update_crc(ULONG crc, const unsigned char *buf, size_t len)
{
	if (!crc_table_computed)
		make_crc_table();

	crc = crc ^ 0xffffffffL;
	while (len--)
		{
		UBYTE ndx = (crc ^ *buf++) & 0xff;

		crc = crc_table[ndx] ^ (crc >> 8);
		}

	return crc ^ 0xffffffffL;
}

//----------------------------------------------------------------------------

