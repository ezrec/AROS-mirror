/*********************************************************************
----------------------------------------------------------------------

	MysticView
	keyfiles

----------------------------------------------------------------------
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <utility/tagitem.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <libraries/cybergraphics.h>
#include <intuition/intuition.h>
#include <exec/memory.h>

#include <clib/macros.h>

#include <proto/asl.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/guigfx.h>
#include <proto/cybergraphics.h>
#include <proto/wb.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/icon.h>
#include <proto/render.h>

#include "Mystic_FileHandling.h"
#include "Mystic_Subtask.h"
#include "Mystic_Keyfile.h"

#include <assert.h>



void crcgen(unsigned long *table)
{
	unsigned long	crc, poly;
	int	i, j;

	poly = 0xEDB88320L;
	for (i=0; i<256; i++) {
		crc = i;
		for (j=8; j>0; j--) {
			if (crc&1) {
				crc = (crc >> 1) ^ poly;
			} else {
				crc >>= 1;
			}
		}
		table[i] = crc;
	}
}


unsigned long getcrc(unsigned long *crctable, char *buffer, int buffersize)
{
	register unsigned long crc;
	int i;

	crc = 0xFFFFFFFF;

	for (i = 0; i < buffersize; ++i)
	{
		crc = ((crc>>8) & 0x00FFFFFF) ^ crctable[ (crc^buffer[i]) & 0xFF ];
	}

	return( crc^0xFFFFFFFF );
}




BOOL getbit(char *buffer, int bitnr)
{
	unsigned char mask = 1 << (7 - (bitnr & 7));

	return (BOOL) !!(buffer[bitnr>>3] & mask);
}



void setbit(char *buffer, int bitnr, BOOL bit)
{
	unsigned char mask = 1 << (7 - (bitnr & 7));

	buffer[bitnr>>3] &= ~mask;
	if (bit)
	{
		buffer[bitnr>>3] |= mask;
	}
}


void decode(char *buffer, int buflen, unsigned long key)
{
	int i;
	int bitlen;
	BOOL bit1, bit2;
	int pos1, pos2;

	bitlen = buflen << 3;

	Forbid();

	srand(key^0x7f532c9a);

	for (i = 0; i < buflen; ++i)
	{
		buffer[i] ^= rand();
	}

	Permit();

	for (i = 0; i < bitlen / 2; ++i)
	{
		pos1 = i;
		pos2 = bitlen-i-1;
		bit1 = !getbit(buffer, pos1);
		bit2 = getbit(buffer, pos2);
		setbit(buffer, pos1, bit2);
		setbit(buffer, pos2, bit1);
	}
}



BOOL CheckKey(void)
{
	#ifndef REGISTERED

	struct keyfile *keyfile;
	BOOL keyokay = FALSE;

	if (keyfile = (struct keyfile *) FindSemaphore(mypersonalID))
	{
		char *buffer;

		if (buffer = Malloc(keyfile->len))
		{
			ULONG crc;

			memcpy(buffer, keyfile->key, keyfile->len);
			decode(buffer, keyfile->len, keyfile->len);
			crc = getcrc(keyfile->crctable, buffer + 4, keyfile->len - 4);

			if (crc == *((ULONG *) buffer))
			{
				keyokay = TRUE;
			}

			srand(time(NULL));

			Free(buffer);
		}
	}

	return keyokay;

	#else

	return TRUE;

	#endif
}


char *GetKeyLicensee(void)
{
	#ifndef REGISTERED

	struct keyfile *keyfile;
	char *name = NULL;

	if (keyfile = (struct keyfile *) FindSemaphore(mypersonalID))
	{
		char *buffer;

		if (buffer = Malloc(keyfile->len))
		{
			ULONG crc;

			memcpy(buffer, keyfile->key, keyfile->len);
			decode(buffer, keyfile->len, keyfile->len);
			crc = getcrc(keyfile->crctable, buffer + 4, keyfile->len - 4);

			if (crc == *((ULONG *) buffer))
			{
				name = _StrDup(&buffer[13]);
			}

			srand(time(NULL));

			Free(buffer);
		}
	}

	return name;

	#else

	return NULL;

	#endif
}







/*********************************************************************
----------------------------------------------------------------------

	RemoveKey(keyfile)

	remove keyfile from memory.

----------------------------------------------------------------------
*********************************************************************/

void RemoveKey(struct keyfile *keyfile)
{
	#ifndef REGISTERED

	if (keyfile)
	{
		RemSemaphore(&keyfile->semaphore);
		Free(keyfile->key);
		Free(keyfile);
	}

	#endif
}


/*********************************************************************
----------------------------------------------------------------------

	keyfile = InstallKey()

	install keyfile in memory.

----------------------------------------------------------------------
*********************************************************************/

struct keyfile *InstallKey(void)
{
	#ifndef REGISTERED

	struct keyfile *keyfile;

	BOOL success = FALSE;
	BPTR fp;
	struct FileInfoBlock ALIGNED fib;


	if (keyfile = Malloclear(sizeof(struct keyfile)))
	{
		if (fp = Open("L:MysticView.key", MODE_OLDFILE))
		{
			ExamineFH(fp, &fib);

			keyfile->len = fib.fib_Size;

			if (keyfile->key = Malloc(keyfile->len))
			{
				if (Read(fp, keyfile->key, keyfile->len) == keyfile->len)
				{
					keyfile->semaphore.ss_Link.ln_Name = mypersonalID;
					keyfile->semaphore.ss_Link.ln_Pri = 1;
					crcgen(keyfile->crctable);
					AddSemaphore(&keyfile->semaphore);
					success = TRUE;
				}
			}
			Close(fp);
		}
	}

	if (!success && keyfile)
	{
		Free(keyfile->key);
		Free(keyfile);
		keyfile = NULL;
	}

	return keyfile;

	#else

	return NULL;

	#endif
}
