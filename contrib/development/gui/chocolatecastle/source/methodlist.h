/*------------------------------------------*/
/* Code generated with ChocolateCastle 0.1  */
/* written by Grzegorz "Krashan" Kraszewski */
/* <krashan@teleinfo.pb.bialystok.pl>       */
/*------------------------------------------*/

#ifndef CHC_METHODLIST_H
#define CHC_METHODLIST_H

/* MethodListClass header. */

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/muimaster.h>
#include <clib/alib_protos.h>
#include <stdint.h>

extern struct MUI_CustomClass *MethodListClass;

struct MUI_CustomClass *CreateMethodListClass(void);
void DeleteMethodListClass(void);

struct MethodEntry
{
	STRPTR Name;
	STRPTR Function;
	STRPTR Structure;
	STRPTR Identifier;
	BOOL   Standard;
};

//------------------------------------------------------------------------------
// This method inserts a null-terminated table of MethodEntry structures into
// the list.

#define MTLM_InsertMethodTable         0x6EDA3466

struct MTLP_InsertMethodTable
{
	ULONG MethodID;
	struct MethodEntry *Table;
};

#endif
