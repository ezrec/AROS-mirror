/*

File: pci.c
Author: Neil Cafferkey
Copyright (C) 2004,2005 Neil Cafferkey

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA.

*/


#include <exec/types.h>
#include <utility/tagitem.h>
#ifdef __amigaos4__
#include <expansion/pci.h>
#else
#include <hardware/intbits.h>
#include <libraries/prometheus.h>
#ifndef __AROS__
#include <pci/powerpci_pci.h>
#endif
#endif

#include <proto/exec.h>
#include <proto/expansion.h>
#include <proto/prometheus.h>
#ifndef __AROS__
#include <proto/powerpci.h>
#endif

#include "device.h"

#include "pci_protos.h"
#include "unit_protos.h"


struct BusContext
{
   struct DevUnit *unit;
   struct DevBase *device;
   VOID *card;
   UPINT io_base;
   UWORD generation;
   const struct TagItem *unit_tags;
};


/* Private prototypes */

static ULONG GetPrometheusCount(struct DevBase *base);
#ifndef __AROS__
static ULONG GetPowerPCICount(struct DevBase *base);
#endif
static struct DevUnit *FindPCIUnit(ULONG index, struct DevBase *base);
static struct DevUnit *CreatePCIUnit(ULONG index, struct DevBase *base);
static struct BusContext *AllocCard(ULONG index, struct DevBase *base);
static struct BusContext *AllocPrometheusCard(ULONG index,
   struct DevBase *base);
#ifndef __AROS__
static struct BusContext *AllocPowerPCICard(ULONG index,
   struct DevBase *base);
#endif
static VOID FreeCard(struct BusContext *context, struct DevBase *base);
static VOID FreePrometheusCard(struct BusContext *context,
   struct DevBase *base);
#ifndef __AROS__
static VOID FreePowerPCICard(struct BusContext *context,
   struct DevBase *base);
#endif
static BOOL AddPCIIntServer(APTR card, struct Interrupt *interrupt,
   struct DevBase *base);
static VOID RemPCIIntServer(APTR card, struct Interrupt *interrupt,
   struct DevBase *base);
static BOOL IsCardCompatible(UWORD vendor_id, UWORD product_id,
   struct DevBase *base);
static UWORD GetGeneration(UWORD product_id, struct DevBase *base);
static UBYTE ByteInHook(struct BusContext *context, ULONG offset);
static ULONG LongInHook(struct BusContext *context, ULONG offset);
static VOID ByteOutHook(struct BusContext *context, ULONG offset,
   UBYTE value);
static VOID WordOutHook(struct BusContext *context, ULONG offset,
   UWORD value);
static VOID LongOutHook(struct BusContext *context, ULONG offset,
   ULONG value);
static VOID LongsInHook(struct BusContext *context, ULONG offset,
   ULONG *buffer, ULONG count);
static VOID LongsOutHook(struct BusContext *context, ULONG offset,
   const ULONG *buffer, ULONG count);
static VOID BEWordOutHook(struct BusContext *context, ULONG offset,
   UWORD value);
static UWORD LEWordInHook(struct BusContext *context, ULONG offset);
static ULONG LELongInHook(struct BusContext *context, ULONG offset);
static VOID LEWordOutHook(struct BusContext *context, ULONG offset,
   UWORD value);
static VOID LELongOutHook(struct BusContext *context, ULONG offset,
   ULONG value);
static APTR AllocDMAMemHook(struct BusContext *context, UPINT size,
   UWORD alignment);
static VOID FreeDMAMemHook(struct BusContext *context, APTR mem);


static const UWORD product_codes[] =
{
   0x10b7, 0x1201,
   0x10b7, 0x1202,
   0x10b7, 0x4500,
   0x10b7, 0x5900,
   0x10b7, 0x5950,
   0x10b7, 0x5951,
   0x10b7, 0x5952,
   0x10b7, 0x9000,
   0x10b7, 0x9001,
   0x10b7, 0x9004,
   0x10b7, 0x9005,
   0x10b7, 0x9006,
   0x10b7, 0x900A,
   0x10b7, 0x9050,
   0x10b7, 0x9051,
   0x10b7, 0x9055,
   0x10b7, 0x9056,
   0x10b7, 0x9058,
   0x10b7, 0x905A,
   0x10b7, 0x9200,
   0x10b7, 0x9201,
   0x10b7, 0x9210,
   0x10b7, 0x9800,
   0x10b7, 0x9805,
   0xffff, 0xffff
};


static const struct TagItem unit_tags[] =
{
   {IOTAG_ByteIn, (UPINT)ByteInHook},
   {IOTAG_LongIn, (UPINT)LongInHook},
   {IOTAG_ByteOut, (UPINT)ByteOutHook},
   {IOTAG_WordOut, (UPINT)WordOutHook},
   {IOTAG_LongOut, (UPINT)LongOutHook},
   {IOTAG_LongsIn, (UPINT)LongsInHook},
   {IOTAG_LongsOut, (UPINT)LongsOutHook},
   {IOTAG_BEWordOut, (UPINT)BEWordOutHook},
   {IOTAG_LEWordIn, (UPINT)LEWordInHook},
   {IOTAG_LELongIn, (UPINT)LELongInHook},
   {IOTAG_LEWordOut, (UPINT)LEWordOutHook},
   {IOTAG_LELongOut, (UPINT)LELongOutHook},
   {IOTAG_AllocDMAMem, (UPINT)AllocDMAMemHook},
   {IOTAG_FreeDMAMem, (UPINT)FreeDMAMemHook},
   {TAG_END, 0}
};


/****i* etherlink3.device/GetPCICount **************************************
*
*   NAME
*	GetPCICount -- Get the number of compatible PCI Cards.
*
*   SYNOPSIS
*	count = GetPCICount()
*
*	ULONG GetPCICount();
*
****************************************************************************
*
*/

ULONG GetPCICount(struct DevBase *base)
{
   ULONG count = 0;

   if(base->prometheus_base != NULL)
      count = GetPrometheusCount(base);
#ifndef __AROS__
   else if(base->powerpci_base != NULL)
      count = GetPowerPCICount(base);
#endif

   return count;
}



/****i* etherlink3.device/GetPrometheusCount *******************************
*
*   NAME
*	GetPrometheusCount
*
*   SYNOPSIS
*	count = GetPrometheusCount()
*
*	ULONG GetPrometheusCount();
*
****************************************************************************
*
*/

static ULONG GetPrometheusCount(struct DevBase *base)
{
   ULONG count = 0;
   PCIBoard *card = NULL;
   UPINT vendor_id, product_id;

   while((card = Prm_FindBoardTagList(card, NULL)) != NULL)
   {
      Prm_GetBoardAttrsTags(card, PRM_Vendor, (UPINT)&vendor_id,
         PRM_Device, (UPINT)&product_id, TAG_END);
      if(IsCardCompatible(vendor_id, product_id, base))
         count++;
   }

   return count;
}



/****i* etherlink3.device/GetPowerPCICount *********************************
*
*   NAME
*	GetPowerPCICount
*
*   SYNOPSIS
*	count = GetPowerPCICount()
*
*	ULONG GetPowerPCICount();
*
****************************************************************************
*
*/

#ifndef __AROS__
static ULONG GetPowerPCICount(struct DevBase *base)
{
   ULONG count = 0;
   ULONG card = 0;
   UWORD vendor_id, product_id;

   while((card = pci_find_device(0xffff, 0xffff, card)) != NULL)
   {
      product_id = pci_read_conf_word(card, PCI_DEVICE_ID);
      vendor_id = pci_read_conf_word(card, PCI_VENDOR_ID);
      if(IsCardCompatible(vendor_id, product_id, base))
         count++;
   }

   return count;
}
#endif



/****i* etherlink3.device/GetOS4Count **************************************
*
*   NAME
*	GetOS4Count
*
*   SYNOPSIS
*	count = GetOS4Count()
*
*	ULONG GetOS4Count();
*
****************************************************************************
*
*/

#ifdef __amigaos4__
static ULONG GetOS4Count(struct DevBase *base)
{
   ULONG count = 0;
   struct PCIDevice *card;

   if(base->i_pci != NULL)
   {
      while((card =
         base->i_pci->FindDeviceTags(FDT_CandidateList, product_codes,
         FDT_Index, count, TAG_END)) != NULL)
      {
         base->i_pci->FreeDevice(card);
         count++;
      }
   }

   return count;
}
#endif



/****i* etherlink3.device/GetPCIUnit ***************************************
*
*   NAME
*	GetPCIUnit -- Get a unit by number.
*
*   SYNOPSIS
*	unit = GetPCIUnit(index)
*
*	struct DevUnit *GetPCIUnit(ULONG);
*
****************************************************************************
*
*/

struct DevUnit *GetPCIUnit(ULONG index, struct DevBase *base)
{
   struct DevUnit *unit;

   unit = FindPCIUnit(index, base);

   if(unit == NULL)
   {
      unit = CreatePCIUnit(index, base);
      if(unit != NULL)
      {
         AddTail((APTR)&base->pci_units, (APTR)unit);
      }
   }

   return unit;
}



/****i* etherlink3.device/FindPCIUnit **************************************
*
*   NAME
*	FindPCIUnit -- Find a unit by number.
*
*   SYNOPSIS
*	unit = FindPCIUnit(index)
*
*	struct DevUnit *FindPCIUnit(ULONG);
*
****************************************************************************
*
*/

static struct DevUnit *FindPCIUnit(ULONG index, struct DevBase *base)
{
   struct DevUnit *unit, *tail;
   BOOL found = FALSE;

   unit = (APTR)base->pci_units.mlh_Head;
   tail = (APTR)&base->pci_units.mlh_Tail;

   while(unit != tail && !found)
   {
      if(unit->index == index)
         found = TRUE;
      else
         unit = (APTR)unit->node.mln_Succ;
   }

   if(!found)
      unit = NULL;

   return unit;
}



/****i* etherlink3.device/CreatePCIUnit ************************************
*
*   NAME
*	CreatePCIUnit -- Create a unit.
*
*   SYNOPSIS
*	unit = CreatePCIUnit(index)
*
*	struct DevUnit *CreatePCIUnit(ULONG);
*
*   FUNCTION
*	Creates a PCI new unit.
*
****************************************************************************
*
*/

static struct DevUnit *CreatePCIUnit(ULONG index, struct DevBase *base)
{
   BOOL success = TRUE;
   struct BusContext *context;
   struct DevUnit *unit = NULL;

   context = AllocCard(index, base);
   if(context == NULL)
      success = FALSE;

   if(success)
   {
      context->device = base;
      context->unit = unit =
         CreateUnit(index, context, context->unit_tags, context->generation,
            PCI_BUS, base);
      if(unit == NULL)
         success = FALSE;
   }

   /* Add interrupt */

   if(success)
      AddPCIIntServer(context->card, &unit->status_int, base);

   if(!success)
   {
      if(context != NULL)
      {
         DeleteUnit(context->unit, base);
         FreeCard(context, base);
      }
      unit = NULL;
   }

   return unit;
}



/****i* etherlink3.device/DeletePCIUnit ************************************
*
*   NAME
*	DeletePCIUnit -- Delete a unit.
*
*   SYNOPSIS
*	DeletePCIUnit(unit)
*
*	VOID DeletePCIUnit(struct DevUnit *);
*
*   FUNCTION
*	Deletes a unit.
*
*   INPUTS
*	unit - Device unit (can be NULL).
*
*   RESULT
*	None.
*
****************************************************************************
*
*/

VOID DeletePCIUnit(struct DevUnit *unit, struct DevBase *base)
{
   struct BusContext *context;

   if(unit != NULL)
   {
      context = unit->card;
      RemPCIIntServer(context->card, &unit->status_int, base);
      DeleteUnit(unit, base);
      FreeCard(context, base);
   }

   return;
}



/****i* etherlink3.device/AllocCard ****************************************
*
*   NAME
*	AllocCard -- Get card from system.
*
*   SYNOPSIS
*	context = AllocCard(index)
*
*	struct BusContext *AllocCard(ULONG);
*
****************************************************************************
*
*/

static struct BusContext *AllocCard(ULONG index, struct DevBase *base)
{
   struct BusContext *context;

   if(base->prometheus_base != NULL)
      context = AllocPrometheusCard(index, base);
#ifndef __AROS__
   else if(base->powerpci_base != NULL)
      context = AllocPowerPCICard(index, base);
#endif

   return context;
}



/****i* etherlink3.device/AllocPrometheusCard ******************************
*
*   NAME
*	AllocPrometheusCard -- Create a unit.
*
*   SYNOPSIS
*	context = AllocPrometheusCard(index)
*
*	struct BusContext *AllocPrometheusCard(ULONG);
*
****************************************************************************
*
*/

static struct BusContext *AllocPrometheusCard(ULONG index,
   struct DevBase *base)
{
   BOOL success = TRUE;
   struct BusContext *context;
   PCIBoard *card = NULL;
   UWORD i = 0;
   UPINT vendor_id, product_id;

   /* Find a compatible card */

   context = AllocMem(sizeof(struct BusContext), MEMF_PUBLIC | MEMF_CLEAR);
   if(context == NULL)
      success = FALSE;

   if(success)
   {
#ifdef __AROS__
      context->unit_tags = unit_tags;
#else
#error Prometheus hardware untested
#endif

      while(i <= index)
      {
         card = Prm_FindBoardTagList(card, NULL);
         Prm_GetBoardAttrsTags(card, PRM_Vendor, (UPINT)&vendor_id,
            PRM_Device, (UPINT)&product_id, TAG_END);
         if(IsCardCompatible(vendor_id, product_id, base))
            i++;
      }

      context->card = card;
      context->generation = GetGeneration(product_id, base);
      if(card == NULL)
         success = FALSE;
   }

   /* Get base address */

   if(success)
   {
      Prm_GetBoardAttrsTags(card, PRM_MemoryAddr0, (UPINT)&context->io_base,
         TAG_END);
      if(context->io_base == 0)
         success = FALSE;
   }

   /* Lock card */

   if(success)
   {
      if(!Prm_SetBoardAttrsTags(card, PRM_BoardOwner, (UPINT)base, TAG_END))
         success = FALSE;
   }

   if(!success)
   {
      FreeCard(context, base);
      context = NULL;
   }

   return context;
}



/****i* etherlink3.device/AllocPowerPCICard ********************************
*
*   NAME
*	AllocPowerPCICard -- Create a unit.
*
*   SYNOPSIS
*	context = AllocPowerPCICard(index)
*
*	struct BusContext *AllocPowerPCICard(ULONG);
*
****************************************************************************
*
*/

#ifndef __AROS__
static struct BusContext *AllocPowerPCICard(ULONG index,
   struct DevBase *base)
{
   BOOL success = TRUE;
   struct BusContext *context;
   ULONG card = 0;
   UWORD i = 0, vendor_id, product_id;

   /* Find a compatible card */

   context = AllocMem(sizeof(struct BusContext), MEMF_PUBLIC | MEMF_CLEAR);
   if(context == NULL)
      success = FALSE;

   if(success)
   {
      context->unit_tags = unit_tags;

      while(i <= index)
      {
         card = pci_find_device(0xffff, 0xffff, card);
         product_id = pci_read_conf_word(card, PCI_DEVICE_ID);
         vendor_id = pci_read_conf_word(card, PCI_VENDOR_ID);
         if(IsCardCompatible(vendor_id, product_id, base))
            i++;
      }

      context->card = (APTR)card;
      context->generation = GetGeneration(product_id, base);
      if(card == NULL) /* ??? */
         success = FALSE;
   }

   /* Get base address */

   if(success)
   {
      context->io_base = (UPINT)pci_get_base_start(card, 0);
      if(context->io_base == NULL)
         success = FALSE;
context->io_base += 0x10000;
   }

   if(!success)
   {
      FreeCard(context, base);
      context = NULL;
   }

   return context;
}
#endif



/****i* etherlink3.device/AllocOS4Card *************************************
*
*   NAME
*	AllocOS4Card -- Create a unit.
*
*   SYNOPSIS
*	context = AllocOS4Card(index)
*
*	struct BusContext *AllocOS4Card(ULONG);
*
****************************************************************************
*
*/

#ifdef __amigaos4__
static struct BusContext *AllocOS4Card(ULONG index, struct DevBase *base)
{
   BOOL success = TRUE;
   struct BusContext *context;
   struct PCIDevice *card = NULL;
   struct PCIResourceRange *io_range = NULL;

   /* Find a compatible card */

   context = AllocMem(sizeof(struct BusContext), MEMF_PUBLIC | MEMF_CLEAR);
   if(context == NULL)
      success = FALSE;

   context->card = card =
      base->i_pci->FindDeviceTags(FDT_CandidateList, product_codes,
         FDT_Index, index, TAG_END);
   if(card == NULL)
      success = FALSE;

   if(success)
   {
      if(!card->Lock(PCI_LOCK_EXCLUSIVE))
         success = FALSE;
   }

   if(success)
   {
      card->SetEndian(PCI_MODE_REVERSE_ENDIAN);
      io_range = card->GetResourceRange(0);
      if(io_range == NULL)
         success = FALSE;
   }

   if(success)
   {
      context->io_base = io_range->BaseAddress;
      context->unit_tags = unit_tags;
   }

   if(!success)
   {
      FreeCard(context, base);
      context = NULL;
   }

   return context;
}
#endif



/****i* etherlink3.device/FreeCard *****************************************
*
*   NAME
*	FreeCard
*
*   SYNOPSIS
*	FreeCard(context)
*
*	VOID FreeCard(struct BusContext *);
*
****************************************************************************
*
*/

static VOID FreeCard(struct BusContext *context, struct DevBase *base)
{

   if(context != NULL)
   {
      if(base->prometheus_base != NULL)
         FreePrometheusCard(context, base);
#ifndef __AROS__
      else if(base->powerpci_base != NULL)
         FreePowerPCICard(context, base);
#endif
   }

   return;
}



/****i* etherlink3.device/FreePrometheusCard *******************************
*
*   NAME
*	FreePrometheusCard
*
*   SYNOPSIS
*	FreePrometheusCard(context)
*
*	VOID FreePrometheusCard(struct BusContext *);
*
****************************************************************************
*
*/

static VOID FreePrometheusCard(struct BusContext *context,
   struct DevBase *base)
{
   PCIBoard *card;
   APTR owner;

   if(context != NULL)
   {
      card = context->card;
      if(card != NULL)
      {
         /* Unlock board */

         Prm_GetBoardAttrsTags(card, PRM_BoardOwner, (UPINT)&owner,
            TAG_END);
         if(owner == base)
            Prm_SetBoardAttrsTags(card, PRM_BoardOwner, NULL, TAG_END);
      }

      FreeMem(context, sizeof(struct BusContext));
   }

   return;
}



/****i* etherlink3.device/FreePowerPCICard *********************************
*
*   NAME
*	FreePowerPCICard
*
*   SYNOPSIS
*	FreePowerPCICard(context)
*
*	VOID FreePowerPCICard(struct BusContext *);
*
****************************************************************************
*
*/

#ifndef __AROS__
static VOID FreePowerPCICard(struct BusContext *context,
   struct DevBase *base)
{
   if(context != NULL)
   {
      FreeMem(context, sizeof(struct BusContext));
   }

   return;
}
#endif



/****i* etherlink3.device/AddPCIIntServer **********************************
*
*   NAME
*	AddPCIIntServer
*
*   SYNOPSIS
*	context = AddPCIIntServer(index)
*
*	struct BusContext *AddPCIIntServer(ULONG);
*
****************************************************************************
*
*/

static BOOL AddPCIIntServer(APTR card, struct Interrupt *interrupt,
   struct DevBase *base)
{
   BOOL success;

   if(base->prometheus_base != NULL)
      success = Prm_AddIntServer(card, interrupt);
#ifndef __AROS__
   else if(base->powerpci_base != NULL)
      success = pci_add_irq((ULONG)card, interrupt);
#endif

   return success;
}



/****i* etherlink3.device/RemPCIIntServer **********************************
*
*   NAME
*	RemPCIIntServer
*
*   SYNOPSIS
*	RemPCIIntServer()
*
*	VOID RemPCIIntServer(ULONG);
*
****************************************************************************
*
*/

static VOID RemPCIIntServer(APTR card, struct Interrupt *interrupt,
   struct DevBase *base)
{
   if(base->prometheus_base != NULL)
      Prm_RemIntServer(card, interrupt);
#ifndef __AROS__
   else if(base->powerpci_base != NULL)
      pci_rem_irq((ULONG)card, interrupt);
#endif

   return;
}



/****i* etherlink3.device/IsCardCompatible *********************************
*
*   NAME
*	IsCardCompatible
*
*   SYNOPSIS
*	compatible = IsCardCompatible(context)
*
*	BOOL IsCardCompatible(struct BusContext *);
*
****************************************************************************
*
*/

static BOOL IsCardCompatible(UWORD vendor_id, UWORD product_id,
   struct DevBase *base)
{
   BOOL compatible = FALSE;
   const UWORD *p;

   for(p = product_codes; p[0] != 0xffff; p += 2)
   {
      if(p[0] == vendor_id && p[1] == product_id)
         compatible = TRUE;
   }

   return compatible;
}



/****i* etherlink3.device/GetGeneration ************************************
*
*   NAME
*	GetGeneration
*
*   SYNOPSIS
*	generation = GetGeneration(product_id)
*
*	UWORD GetGeneration(UWORD);
*
****************************************************************************
*
*/

static UWORD GetGeneration(UWORD product_id, struct DevBase *base)
{
   UWORD generation;

   switch(product_id)
   {
   case 0x5900:
   case 0x5950:
   case 0x5951:
   case 0x5952:
      generation = VORTEX_GEN;
      break;
   case 0x9000:
   case 0x9050:
   case 0x9051:
      generation = BOOMERANG_GEN;
      break;
   case 0x9004:
   case 0x9005:
   case 0x9006:
   case 0x900a:
   case 0x9055:
   case 0x9056:
   case 0x9058:
   case 0x905a:
   case 0x9800:
   case 0x9805:
      generation = CYCLONE_GEN;
      break;
   default:
      generation = TORNADO_GEN;
   }

   return generation;
}



/****i* etherlink3.device/ByteInHook ***************************************
*
*   NAME
*	ByteInHook
*
*   SYNOPSIS
*	value = ByteInHook(context, offset)
*
*	UBYTE ByteInHook(struct BusContext *, ULONG);
*
****************************************************************************
*
*/

static UBYTE ByteInHook(struct BusContext *context, ULONG offset)
{
   return BYTEIN(context->io_base + offset);
}



/****i* etherlink3.device/LongInHook ***************************************
*
*   NAME
*	LongInHook
*
*   SYNOPSIS
*	value = LongInHook(context, offset)
*
*	ULONG LongInHook(struct BusContext *, ULONG);
*
****************************************************************************
*
*/

static ULONG LongInHook(struct BusContext *context, ULONG offset)
{
   return LONGIN(context->io_base + offset);
}



/****i* etherlink3.device/ByteOutHook **************************************
*
*   NAME
*	ByteOutHook
*
*   SYNOPSIS
*	ByteOutHook(context, offset, value)
*
*	VOID ByteOutHook(struct BusContext *, ULONG, UBYTE);
*
****************************************************************************
*
*/

static VOID ByteOutHook(struct BusContext *context, ULONG offset,
   UBYTE value)
{
   BYTEOUT(context->io_base + offset, value);

   return;
}



/****i* etherlink3.device/WordOutHook **************************************
*
*   NAME
*	WordOutHook
*
*   SYNOPSIS
*	WordOutHook(context, offset, value)
*
*	VOID WordOutHook(struct BusContext *, ULONG, UWORD);
*
****************************************************************************
*
*/

static VOID WordOutHook(struct BusContext *context, ULONG offset,
   UWORD value)
{
   WORDOUT(context->io_base + offset, value);

   return;
}



/****i* etherlink3.device/LongOutHook **************************************
*
*   NAME
*	LongOutHook
*
*   SYNOPSIS
*	LongOutHook(context, offset, value)
*
*	VOID LongOutHook(struct BusContext *, ULONG, ULONG);
*
****************************************************************************
*
*/

static VOID LongOutHook(struct BusContext *context, ULONG offset,
   ULONG value)
{
   LONGOUT(context->io_base + offset, value);

   return;
}



/****i* prism2.device/LongsInHook ******************************************
*
*   NAME
*	LongsInHook
*
*   SYNOPSIS
*	LongsInHook(context, offset, buffer, count)
*
*	VOID LongsInHook(struct BusContext *, ULONG, ULONG *, ULONG);
*
****************************************************************************
*
*/

static VOID LongsInHook(struct BusContext *context, ULONG offset,
   ULONG *buffer, ULONG count)
{
   LONGSIN(context->io_base + offset, buffer, count);

   return;
}



/****i* etherlink3.device/LongsOutHook *************************************
*
*   NAME
*	LongsOutHook
*
*   SYNOPSIS
*	LongsOutHook(context, offset, buffer, count)
*
*	VOID LongsOutHook(struct BusContext *, ULONG, const ULONG *, ULONG);
*
****************************************************************************
*
*/

static VOID LongsOutHook(struct BusContext *context, ULONG offset,
   const ULONG *buffer, ULONG count)
{
   LONGSOUT(context->io_base + offset, buffer, count);

   return;
}



/****i* etherlink3.device/BEWordOutHook ************************************
*
*   NAME
*	BEWordOutHook
*
*   SYNOPSIS
*	BEWordOutHook(context, offset, value)
*
*	VOID BEWordOutHook(struct BusContext *, ULONG, UWORD);
*
****************************************************************************
*
*/

static VOID BEWordOutHook(struct BusContext *context, ULONG offset,
   UWORD value)
{
   BEWORDOUT(context->io_base + offset, value);

   return;
}



/****i* etherlink3.device/LEWordInHook *************************************
*
*   NAME
*	LEWordInHook
*
*   SYNOPSIS
*	value = LEWordInHook(context, offset)
*
*	UWORD LEWordInHook(struct BusContext *, ULONG);
*
****************************************************************************
*
*/

static UWORD LEWordInHook(struct BusContext *context, ULONG offset)
{
   return LEWORDIN(context->io_base + offset);
}



/****i* etherlink3.device/LELongInHook ***************************************
*
*   NAME
*	LELongInHook
*
*   SYNOPSIS
*	value = LELongInHook(context, offset)
*
*	ULONG LELongInHook(struct BusContext *, ULONG);
*
****************************************************************************
*
*/

static ULONG LELongInHook(struct BusContext *context, ULONG offset)
{
   return LELONGIN(context->io_base + offset);
}



/****i* etherlink3.device/LEWordOutHook ************************************
*
*   NAME
*	LEWordOutHook
*
*   SYNOPSIS
*	LEWordOutHook(context, offset, value)
*
*	VOID LEWordOutHook(struct BusContext *, ULONG, UWORD);
*
****************************************************************************
*
*/

static VOID LEWordOutHook(struct BusContext *context, ULONG offset,
   UWORD value)
{
   LEWORDOUT(context->io_base + offset, value);

   return;
}



/****i* etherlink3.device/LELongOutHook ************************************
*
*   NAME
*	LELongOutHook
*
*   SYNOPSIS
*	LELongOutHook(context, offset, value)
*
*	VOID LELongOutHook(struct BusContext *, ULONG, ULONG);
*
****************************************************************************
*
*/

static VOID LELongOutHook(struct BusContext *context, ULONG offset,
   ULONG value)
{
   LELONGOUT(context->io_base + offset, value);

   return;
}



/****i* etherlink3.device/AllocDMAMemHook **********************************
*
*   NAME
*	AllocDMAMemHook
*
*   SYNOPSIS
*	mem = AllocDMAMemHook(context, size, alignment)
*
*	APTR AllocDMAMemHook(struct BusContext *, UPINT, UWORD);
*
****************************************************************************
*
* Alignment currently must be minimum of 8 bytes.
*
*/

static APTR AllocDMAMemHook(struct BusContext *context, UPINT size,
   UWORD alignment)
{
   struct DevBase *base;
   APTR mem = NULL, original_mem;

   base = context->device;
   size += 2 * sizeof(APTR) + alignment;
   if(base->prometheus_base != NULL)
      original_mem = Prm_AllocDMABuffer(size);
   else
      original_mem = AllocMem(size, MEMF_PUBLIC);
   if(original_mem != NULL)
   {
      mem = (APTR)((UPINT)(original_mem + 2 * sizeof(APTR) + alignment - 1)
         & ~(alignment - 1));
      *((APTR *)mem - 1) = original_mem;
      *((UPINT *)mem - 2) = size;
   }

   return mem;
}



/****i* etherlink3.device/FreeDMAMemHook ***********************************
*
*   NAME
*	FreeDMAMemHook
*
*   SYNOPSIS
*	FreeDMAMemHook(context, mem)
*
*	VOID FreeDMAMemHook(struct BusContext *, APTR);
*
****************************************************************************
*
*/

static VOID FreeDMAMemHook(struct BusContext *context, APTR mem)
{
   struct DevBase *base;

   base = context->device;
   if(mem != NULL)
   {
      if(base->prometheus_base != NULL)
         Prm_FreeDMABuffer(*((APTR *)mem - 1), *((UPINT *)mem - 2));
      else
         FreeMem(*((APTR *)mem - 1), *((UPINT *)mem - 2));
   }

   return;
}



