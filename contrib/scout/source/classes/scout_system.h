/**
 * Scout - The Amiga System Monitor
 *
 *------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * You must not use this source code to gain profit of any kind!
 *
 *------------------------------------------------------------------
 *
 * @author Andreas Gelhausen
 * @author Richard Körber <rkoerber@gmx.de>
 */


/* Prototypes for functions defined in
objects/scout_system.c
 */

struct SystemEntry {
    struct MinNode se_Node;
    TEXT se_System[128];
    TEXT se_CPU[128];
    TEXT se_FPU[128];
    TEXT se_MMU[128];
    TEXT se_PPC[128];
    TEXT se_Chipset[128];
    TEXT se_Ramsey[128];
    TEXT se_Gary[128];
    TEXT se_Agnus[128];
    TEXT se_Denise[128];
    TEXT se_ChunkyPlanar[128];
    TEXT se_BattClock[128];
    TEXT se_FBlit[128];
    TEXT se_RAMWidth[128];
    TEXT se_RAMAccess[128];
    TEXT se_RAMCAS[128];
    TEXT se_RAMBandWidth[128];
    TEXT se_VBlankFreq[128];
    TEXT se_PowerFreq[128];
    TEXT se_EClock[128];
    TEXT se_SSP[128];
    TEXT se_VBR[128];
    TEXT se_CACR[128];
    TEXT se_PCR[128];
    TEXT se_InstCache[128];
    TEXT se_DataCache[128];
    TEXT se_BranchCache[128];
    TEXT se_68060[128];
    TEXT se_CurrentTime[128];
    TEXT se_BootTime[128];
    TEXT se_UpTime[128];
    TEXT se_AmigaOS[128];
    TEXT se_BoingBag[128];
    TEXT se_OSVersion[128];
    TEXT se_SetPatch[128];
    TEXT se_ROMSize[128];
    TEXT se_Exec[128];
    TEXT se_Workbench[128];
    TEXT se_GraphicSystem[128];
    TEXT se_AudioSystem[128];
    TEXT se_TCPIP[128];
    TEXT se_PowerPCOS[128];
    TEXT se_PlainChipRAM[128];
    TEXT se_PlainFastRAM[128];
    TEXT se_PlainRAM[128];
    TEXT se_VirtualChipRAM[128];
    TEXT se_VirtualFastRAM[128];
    TEXT se_VirtualRAM[128];
    TEXT se_TotalChipRAM[128];
    TEXT se_TotalFastRAM[128];
    TEXT se_TotalRAM[128];
    TEXT se_SlowRAM[128];
    TEXT se_AlertCode[128];
    TEXT se_AlertTask[128];
    TEXT se_AlertDeadend[128];
    TEXT se_AlertSubsystem[128];
    TEXT se_AlertGeneral[128];
    TEXT se_AlertSpecific[128];
};

void PrintSystem( STRPTR filename );

void SendSystemList( STRPTR );

#define SystemWindowObject                   NewObject(SystemWinClass->mcc_Class, NULL

APTR MakeSystemWinClass( void );

