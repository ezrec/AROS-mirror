/*
     emu10kx.audio - AHI driver for SoundBlaster Live! series
     Copyright (C) 2002-2003 Martin Blom <martin@blom.org>

     This program is free software; you can redistribute it and/or
     modify it under the terms of the GNU General Public License
     as published by the Free Software Foundation; either version 2
     of the License, or (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include <config.h>

#include <exec/memory.h>
#include <proto/exec.h>

#ifdef __AMIGAOS4__
#include <proto/expansion.h>
#define __NOLIBBASE__
#include <proto/ahi_sub.h>
#include <proto/utility.h>
#else
#include <libraries/openpci.h>
#include <proto/openpci.h>
#include <clib/alib_protos.h>
#endif


#include "library.h"
#include "version.h"
#include "emu10kx-misc.h"


/* We use global library bases instead of storing them in DriverBase, since
   I don't want to modify the original sources more than necessary. */

struct ExecBase*   SysBase;
struct DosLibrary* DOSBase;
#ifndef __AMIGAOS4__
struct Library*    OpenPciBase;
#endif
struct DriverBase* AHIsubBase;

#ifdef __AMIGAOS4__
struct ExpansionBase*       ExpansionBase = NULL;
struct ExpansionIFace*      IExpansion    = NULL;
struct DOSIFace*            IDOS          = NULL;
struct AHIsubIFace*         IAHIsub       = NULL;
struct ExecIFace*           IExec         = NULL;
struct MMUIFace*            IMMU          = NULL;
struct UtilityIFace*        IUtility      = NULL;
struct PCIIFace*            IPCI          = NULL;
#endif


#include "8010.h"




/******************************************************************************
** Custom driver init *********************************************************
******************************************************************************/

BOOL
DriverInit( struct DriverBase* ahisubbase )
{
  struct EMU10kxBase* EMU10kxBase = (struct EMU10kxBase*) ahisubbase;
#ifdef __AMIGAOS4__
  struct PCIDevice   *dev;
#else
  struct pci_dev*     dev;
#endif
  int                 card_no;

#ifdef __AMIGAOS4__
  SysBase = AbsExecBase;
#endif

  /*** Libraries etc. ********************************************************/

  AHIsubBase = ahisubbase;
  
  DOSBase  = (struct DosLibrary*) OpenLibrary( DOSNAME, 37 );

  if( DOSBase == NULL )
  {
    Req( "Unable to open 'dos.library' version 37.\n" );
    return FALSE;
  }

#ifdef __AMIGAOS4__

  if ((IDOS = (struct DOSIFace *) GetInterface((struct Library *) DOSBase, "main", 1, NULL)) == NULL)
  {
       Req("Couldn't open IDOS interface!\n");
       return FALSE;
  }

  ExpansionBase = (struct ExpansionBase*) OpenLibrary( "expansion.library", 1 );
  if( ExpansionBase == NULL )
  {
    Req( "Unable to open 'expansion.library' version 1.\n" );
    return FALSE;
  }
  if ((IExpansion = (struct ExpansionIFace *) GetInterface((struct Library *) ExpansionBase, "main", 1, NULL)) == NULL)
  {
       Req("Couldn't open IExpansion interface!\n");
       return FALSE;
  }

  if ((IPCI = (struct PCIIFace *) GetInterface((struct Library *) ExpansionBase, "pci", 1, NULL)) == NULL)
  {
       Req("Couldn't open IPCI interface!\n");
       return FALSE;
  }
  
  if ((IAHIsub = (struct AHIsubIFace *) GetInterface((struct Library *) AHIsubBase, "main", 1, NULL)) == NULL)
  {
       Req("Couldn't open IAHIsub interface!\n");
       return FALSE;
  }
  
  if ((IMMU = (struct MMUIFace *) GetInterface((struct Library *) SysBase, "mmu", 1, NULL)) == NULL)
  {
       Req("Couldn't open IMMU interface!\n");
       return FALSE;
  }
  
  IUtility = SysBase->UtilityInterface;

#else

  OpenPciBase = OpenLibrary( "openpci.library", 1 );
  
  if( OpenPciBase == NULL )
  {
    Req( "Unable to open 'openpci.library' version 1.\n" );
    return FALSE;
  }

  EMU10kxBase->flush_caches = pci_bus() & ( GrexA1200Bus | GrexA4000Bus );

#endif

  InitSemaphore( &EMU10kxBase->semaphore );


  /*** Count cards ***********************************************************/

  EMU10kxBase->cards_found = 0;
  dev = NULL;

#ifdef __AMIGAOS4__
  // Search for Live! cards, tbd : while
  if ( (dev = IPCI->FindDeviceTags( FDT_VendorID, PCI_VENDOR_ID_CREATIVE,
				  FDT_DeviceID, PCI_DEVICE_ID_CREATIVE_EMU10K1,
				  TAG_DONE ) ) != NULL )
  {
    ++EMU10kxBase->cards_found;
    DebugPrintF("EMU10Kx found! :-)\n");
  }
  
  // Search for Audigy cards
  if ( (dev = IPCI->FindDeviceTags( FDT_VendorID, PCI_VENDOR_ID_CREATIVE,
				  FDT_DeviceID, PCI_DEVICE_ID_CREATIVE_AUDIGY,
				  TAG_DONE ) ) != NULL )
  {
    ++EMU10kxBase->cards_found;
    DebugPrintF("Audigy found! :-)\n");
  }
#else
  // Search for Live! cards
  while( ( dev = pci_find_device( PCI_VENDOR_ID_CREATIVE,
				  PCI_DEVICE_ID_CREATIVE_EMU10K1,
				  dev ) ) != NULL )
  {
    ++EMU10kxBase->cards_found;
  }
  
  // Search for Audigy cards
  while( ( dev = pci_find_device( PCI_VENDOR_ID_CREATIVE,
				  PCI_DEVICE_ID_CREATIVE_AUDIGY,
				  dev ) ) != NULL )
  {
    ++EMU10kxBase->cards_found;
  }
#endif
  
  // Fail if no hardware (prevents the audio modes form being added to
  // the database if the driver cannot be used).

  if( EMU10kxBase->cards_found == 0 )
  {
//    Req( "No SoundBlaster Live! or Audigy card present.\n" );
#ifdef __AMIGAOS4__
    KPrintF( ":No SoundBlaster Live! or Audigy card present.\n" );
#else
    KPrintF( DRIVER ":No SoundBlaster Live! or Audigy card present.\n" );
#endif
    return FALSE;
  }

  /*** CAMD ******************************************************************/
  
  InitSemaphore( &EMU10kxBase->camd.Semaphore );
  EMU10kxBase->camd.Semaphore.ss_Link.ln_Pri  = 0;
  EMU10kxBase->camd.Semaphore.ss_Link.ln_Name = EMU10KX_CAMD_SEMAPHORE;
  
  EMU10kxBase->camd.Cards    = EMU10kxBase->cards_found;
  EMU10kxBase->camd.Version  = VERSION;
  EMU10kxBase->camd.Revision = REVISION;

#ifdef __AMIGAOS4__
  EMU10kxBase->camd.OpenPortFunc.h_Entry    = OpenCAMDPort;
  EMU10kxBase->camd.OpenPortFunc.h_SubEntry = NULL;
  EMU10kxBase->camd.OpenPortFunc.h_Data     = NULL;

  EMU10kxBase->camd.ClosePortFunc.h_Entry    = (HOOKFUNC) CloseCAMDPort;
  EMU10kxBase->camd.ClosePortFunc.h_SubEntry = NULL;
  EMU10kxBase->camd.ClosePortFunc.h_Data     = NULL;

  EMU10kxBase->camd.ActivateXmitFunc.h_Entry    = (HOOKFUNC) ActivateCAMDXmit;
  EMU10kxBase->camd.ActivateXmitFunc.h_SubEntry = NULL;
  EMU10kxBase->camd.ActivateXmitFunc.h_Data     = NULL;
  
#else
  
  EMU10kxBase->camd.OpenPortFunc.h_Entry    = HookEntry;
  EMU10kxBase->camd.OpenPortFunc.h_SubEntry = OpenCAMDPort;
  EMU10kxBase->camd.OpenPortFunc.h_Data     = NULL;

  EMU10kxBase->camd.ClosePortFunc.h_Entry    = HookEntry;
  EMU10kxBase->camd.ClosePortFunc.h_SubEntry = (HOOKFUNC) CloseCAMDPort;
  EMU10kxBase->camd.ClosePortFunc.h_Data     = NULL;

  EMU10kxBase->camd.ActivateXmitFunc.h_Entry    = HookEntry;
  EMU10kxBase->camd.ActivateXmitFunc.h_SubEntry = (HOOKFUNC) ActivateCAMDXmit;
  EMU10kxBase->camd.ActivateXmitFunc.h_Data     = NULL;
#endif
  
  AddSemaphore( &EMU10kxBase->camd.Semaphore );

  /*** AC97 Mixer ************************************************************/
  
  InitSemaphore( &EMU10kxBase->ac97.Semaphore );
  EMU10kxBase->ac97.Semaphore.ss_Link.ln_Pri  = 0;
  EMU10kxBase->ac97.Semaphore.ss_Link.ln_Name = EMU10KX_AC97_SEMAPHORE;
  
  EMU10kxBase->ac97.Cards    = EMU10kxBase->cards_found;
  EMU10kxBase->ac97.Version  = VERSION;
  EMU10kxBase->ac97.Revision = REVISION;

#ifdef __AMIGAOS4__
  EMU10kxBase->ac97.GetFunc.h_Entry    = AC97GetFunc; //HookEntry;
  EMU10kxBase->ac97.GetFunc.h_SubEntry = NULL;
  EMU10kxBase->ac97.GetFunc.h_Data     = NULL;

  EMU10kxBase->ac97.SetFunc.h_Entry    = AC97SetFunc; //HookEntry;
  EMU10kxBase->ac97.SetFunc.h_SubEntry = NULL;
  EMU10kxBase->ac97.SetFunc.h_Data     = NULL;

#else

  EMU10kxBase->ac97.GetFunc.h_Entry    = HookEntry;
  EMU10kxBase->ac97.GetFunc.h_SubEntry = AC97GetFunc;
  EMU10kxBase->ac97.GetFunc.h_Data     = NULL;

  EMU10kxBase->ac97.SetFunc.h_Entry    = HookEntry;
  EMU10kxBase->ac97.SetFunc.h_SubEntry = (HOOKFUNC) AC97SetFunc;
  EMU10kxBase->ac97.SetFunc.h_Data     = NULL;
#endif

  AddSemaphore( &EMU10kxBase->ac97.Semaphore );
  
  /*** Allocate and init all cards *******************************************/

  EMU10kxBase->driverdatas = AllocVec( sizeof( *EMU10kxBase->driverdatas ) *
				       EMU10kxBase->cards_found,
				       MEMF_PUBLIC );

  if( EMU10kxBase->driverdatas == NULL )
  {
    Req( "Out of memory." );
    return FALSE;
  }

  card_no = 0;
  
#ifdef __AMIGAOS4__
  // Live! cards ... 
  if( ( dev = IPCI->FindDeviceTags( FDT_VendorID, PCI_VENDOR_ID_CREATIVE,
				  FDT_DeviceID, PCI_DEVICE_ID_CREATIVE_EMU10K1,
				  TAG_DONE ) ) != NULL )
  {
    //dev->Lock(PCI_LOCK_EXCLUSIVE); tbd
    EMU10kxBase->driverdatas[ card_no ] = AllocDriverData( dev, AHIsubBase );
    ++card_no;
  }
  
  // Audigy cards ...
  if( ( dev = IPCI->FindDeviceTags( FDT_VendorID, PCI_VENDOR_ID_CREATIVE,
				  FDT_DeviceID, PCI_DEVICE_ID_CREATIVE_AUDIGY,
				  TAG_DONE ) ) != NULL )
  {
    //dev->Lock(PCI_LOCK_EXCLUSIVE); tbd
    EMU10kxBase->driverdatas[ card_no ] = AllocDriverData( dev, AHIsubBase );
    ++card_no;
  }
#else
  // Live! cards ... 
  while( ( dev = pci_find_device( PCI_VENDOR_ID_CREATIVE,
				  PCI_DEVICE_ID_CREATIVE_EMU10K1,
				  dev ) ) != NULL )
  {
    EMU10kxBase->driverdatas[ card_no ] = AllocDriverData( dev, AHIsubBase );
    ++card_no;
  }
  
  // Audigy cards ...
  while( ( dev = pci_find_device( PCI_VENDOR_ID_CREATIVE,
				  PCI_DEVICE_ID_CREATIVE_AUDIGY,
				  dev ) ) != NULL )
  {
    EMU10kxBase->driverdatas[ card_no ] = AllocDriverData( dev, AHIsubBase );
    ++card_no;
  }
#endif

  return TRUE;
}


/******************************************************************************
** Custom driver clean-up *****************************************************
******************************************************************************/

VOID
DriverCleanup( struct DriverBase* AHIsubBase )
{
  struct EMU10kxBase* EMU10kxBase = (struct EMU10kxBase*) AHIsubBase;
  int i;

  if( EMU10kxBase->camd.Semaphore.ss_Link.ln_Name != NULL )
  {
    ObtainSemaphore( &EMU10kxBase->camd.Semaphore );
    RemSemaphore( &EMU10kxBase->camd.Semaphore );
    ReleaseSemaphore( &EMU10kxBase->camd.Semaphore );
  }

  if( EMU10kxBase->ac97.Semaphore.ss_Link.ln_Name != NULL )
  {
    ObtainSemaphore( &EMU10kxBase->ac97.Semaphore );
    RemSemaphore( &EMU10kxBase->ac97.Semaphore );
    ReleaseSemaphore( &EMU10kxBase->ac97.Semaphore );
  }

  for( i = 0; i < EMU10kxBase->cards_found; ++i )
  {
    // Kill'em all
    emu10k1_irq_disable( &EMU10kxBase->driverdatas[ i ]->card, ~0UL );
//			 INTE_MIDIRXENABLE | INTE_MIDITXENABLE);
    
    FreeDriverData( EMU10kxBase->driverdatas[ i ], AHIsubBase );
  }

  FreeVec( EMU10kxBase->driverdatas ); 
  
#ifdef __AMIGAOS4__
  DebugPrintF("CLEANING UP!\n");
  DropInterface( (struct Interface *) IExpansion);
  DropInterface( (struct Interface *) IPCI);
  DropInterface( (struct Interface *) IAHIsub);
  CloseLibrary( (struct Library*) ExpansionBase );    
#else
  CloseLibrary( OpenPciBase );
#endif
  CloseLibrary( (struct Library*) DOSBase );
}
