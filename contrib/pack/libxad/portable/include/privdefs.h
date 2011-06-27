#ifndef XADMASTER_PRIVDEFS_H
#define XADMASTER_PRIVDEFS_H

/*  $Id$
    private definitions and structures

    XAD library system for archive handling
    Copyright (C) 1998 and later by Dirk Stöcker <soft@dstoecker.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifdef UNIX
#include "xadmaster.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <emulation.h>
#define stricmp(a, b) strcasecmp((char *)(a), (char *)(b))
#define strnicmp(a, b, c) strncasecmp((char *)(a), (char *)(b), c)
#elif defined(AMIGA)
#include "xadmaster_intern.h"
#include <proto/utility.h>
#if !defined(NOXADPROTOS)
#include <proto/xadmaster.h>
#endif
#include <exec/semaphores.h>
#include "SDI_compiler.h"
#define SDI_TO_ANSI
#include <SDI_ASM_STD_protos.h>
#endif

#ifdef __amigaos4__
#include <exec/emulation.h>
#endif

/************************************************************************
*                                                                       *
*    language support                                                   *
*                                                                       *
************************************************************************/

#define MaxVal(a,b)     ((a) > (b) ? (a) : (b))
#define MinVal(a,b)     ((a) < (b) ? (a) : (b))

/************************************************************************
*                                                                       *
*    xadAllocObject/xadFreeObject internal structure                    *
*                                                                       *
************************************************************************/

struct xadObject {
  xadUINT32              xo_Size; /* size of complete object, incl. this header */
  xadUINT32              xo_Type; /* object type, see libraries/xadmaster.h     */
#ifdef DEBUGRESOURCE
  struct xadObject * xo_Next;
  struct xadObject * xo_Last;
#ifdef AMIGA
  struct Task *      xo_Task;
#else
  xadUINT32          xo_Unused;
#endif
  xadUINT32              xo_Cookie;
#endif
};

/************************************************************************
*                                                                       *
*    InHookSplitted private structure                                   *
*                                                                       *
************************************************************************/

struct xadSplitData {
  xadFileHandle          xsd_InFileHandle;
  xadSTRPTR              xsd_InFileName;
  xadUINT8 *             xsd_InMemory;
  xadSize                xsd_InSize;
  struct xadSplitFile *  xsd_InSplitted;
  xadTAGPTR              xsd_InDiskArc;
#ifdef AMIGA
  struct xadDeviceInfo * xsd_InDevice;
#endif
  xadTAGPTR              xsd_InArcInfo;
  xadSize *              xsd_MultiVolume;

  xadSize                xsd_Offset;
  struct Hook *          xsd_Hook;
  struct xadHookParam    xsd_HookParam;
};

/************************************************************************
*                                                                       *
*    private archive info structure                                     *
*                                                                       *
************************************************************************/

struct xadArchiveInfoP {
  struct xadArchiveInfo  xaip_ArchiveInfo;
  struct xadHookParam    xaip_InHookParam;
  struct xadHookParam    xaip_OutHookParam;
  struct Hook *          xaip_InHook;
  struct Hook *          xaip_OutHook;
  struct Hook *          xaip_ProgressHook;

  struct xadMasterBaseP *xaip_MasterBase;  /* for internal hooks */

  xadFileHandle          xaip_InFileHandle;
  xadSTRPTR              xaip_InFileName;
  xadUINT8 *             xaip_InMemory;
  xadSize                xaip_InSize;
  struct xadSplitFile *  xaip_InSplitted;
  xadTAGPTR              xaip_InDiskArc;
  xadTAGPTR              xaip_InArcInfo;
#ifdef AMIGA
  struct xadDeviceInfo * xaip_InDevice;
  struct xadDeviceInfo * xaip_OutDevice;
#endif

  xadFileHandle          xaip_OutFileHandle;
  xadSTRPTR              xaip_OutFileName;
  xadUINT8 *             xaip_OutMemory;
  xadSize                xaip_OutSize;
  xadTAGPTR              xaip_OutArcInfo;

  xadUINT32              xaip_LastEntryNumber;
  struct xadFileInfo *   xaip_LastFileEntry;
  struct xadDiskInfo *   xaip_LastDiskEntry;
};

/************************************************************************
*                                                                       *
*    client calls                                                       *
*                                                                       *
************************************************************************/

#ifdef AMIGA
#define XADM_PRIV
#define XADM_PUB
#define XADM_AI(ai)   ((struct xadArchiveInfo *) ai)
#else
/* A special version of XADM that casts to private */
#define XADM_PRIV (struct xadMasterBaseP *) xadMasterBase,

/* A special version of XADM that casts to public */
#define XADM_PUB (struct xadMasterBase *) xadMasterBase,

/* Internal ai-structure passing */
#define XADM_AI(ai)   ((struct xadArchiveInfoP *) ai)
#endif

#if defined(AMIGA) && !defined(__amigaos4__) && !defined(__AROS__)
typedef ASM(xadBOOL) (*XCRECOGTYPE)
  (REG(d0, xadSize size), REG(a0, const xadUINT8 *data),
   REG(a6, struct xadMasterBaseP *xadMasterBase));
typedef ASM(xadERROR) (*XCGETINFO)
  (REG(a0, struct xadArchiveInfoP *ai),
   REG(a6, struct xadMasterBaseP *xadMasterBase));
typedef ASM(xadERROR) (*XCUNARCHIVE)
  (REG(a0, struct xadArchiveInfoP *ai),
   REG(a6, struct xadMasterBaseP *xadMasterBase));
typedef ASM(void) (*XCFREE)
  (REG(a0, struct xadArchiveInfoP *ai),
   REG(a6, struct xadMasterBaseP *xadMasterBase));
#elif defined(__amigaos4__)
typedef xadBOOL (*XCRECOGTYPE)
  (xadSize size, const xadUINT8 *data,
   struct xadMasterIFace *IxadMaster);
typedef xadERROR (*XCGETINFO)
  (struct xadArchiveInfoP *ai,
   struct xadMasterIFace *IxadMaster);
typedef xadERROR (*XCUNARCHIVE)
  (struct xadArchiveInfoP *ai,
   struct xadMasterIFace *IxadMaster);
typedef void (*XCFREE)
  (struct xadArchiveInfoP *ai,
   struct xadMasterIFace *IxadMaster);
#else
typedef xadBOOL (*XCRECOGTYPE)
  (xadSize size, const xadUINT8 *data,
   struct xadMasterBaseP *xadMasterBase);
typedef xadERROR (*XCGETINFO)
  (struct xadArchiveInfoP *ai,
   struct xadMasterBaseP *xadMasterBase);
typedef xadERROR (*XCUNARCHIVE)
  (struct xadArchiveInfoP *ai,
   struct xadMasterBaseP *xadMasterBase);
typedef void (*XCFREE)
  (struct xadArchiveInfoP *ai,
   struct xadMasterBaseP *xadMasterBase);
#endif

#if defined(__GNUC__) && defined (__MORPHOS__)
#  define Callback_RecogData(_xc, _size, _data, _base) ({               \
  xadBOOL _cb_re;                                                       \
  if (_xc->xc_Flags & XADCF_EXTERN) {                                   \
    xadUINT32 _cb_v1 = _size;                                           \
    const xadUINT8 *_cb_v2 = _data;                                     \
    REG_D0 = (xadUINT32)_cb_v1;                                         \
    REG_A0 = (xadUINT32)_cb_v2;                                         \
    REG_A6 = (xadUINT32)_base;                                          \
    _cb_re = (xadBOOL)(*MyEmulHandle->EmulCallDirect68k)(_xc->xc_RecogData); \
  }                                                                     \
  else {                                                                \
      _cb_re = ((XCRECOGTYPE)_xc->xc_RecogData)(_size, _data, _base);   \
  }                                                                     \
  _cb_re;                                                               \
})
#  define Callback_GetInfo(_xc, _info, _base) ({                        \
  xadERROR _cb_re;                                                      \
  if (_xc->xc_Flags & XADCF_EXTERN) {                                   \
    struct xadArchiveInfoP *_cb_v1 = _info;                             \
    REG_A0 = (xadUINT32)_cb_v1;                                         \
    REG_A6 = (xadUINT32)_base;                                          \
    _cb_re = (xadERROR)(*MyEmulHandle->EmulCallDirect68k)(_xc->xc_GetInfo); \
  }                                                                     \
  else {                                                                \
    _cb_re = ((XCGETINFO)_xc->xc_GetInfo)(_info, _base);                \
  }                                                                     \
  _cb_re;                                                               \
})
#  define Callback_UnArchive(_xc, _info, _base) ({                      \
  xadERROR _cb_re;                                                      \
  if (_xc->xc_Flags & XADCF_EXTERN) {                                   \
    struct xadArchiveInfoP *_cb_v1 = _info;                             \
    REG_A0 = (xadUINT32)_cb_v1;                                         \
    REG_A6 = (xadUINT32)_base;                                          \
    _cb_re = (xadERROR)(*MyEmulHandle->EmulCallDirect68k)(_xc->xc_UnArchive); \
  }                                                                     \
  else {                                                                \
    _cb_re = ((XCUNARCHIVE)_xc->xc_UnArchive)(_info, _base);            \
  }                                                                     \
  _cb_re;                                                               \
})
#  define Callback_Free(_xc, _info, _base) ({                           \
  if (_xc->xc_Flags & XADCF_EXTERN) {                                   \
    struct xadArchiveInfoP *_cb_v1 = _info;                             \
    REG_A0 = (xadUINT32)_cb_v1;                                         \
    REG_A6 = (xadUINT32)_base;                                          \
    (*MyEmulHandle->EmulCallDirect68k)(_xc->xc_Free);                   \
  }                                                                     \
  else {                                                                \
    ((XCFREE)_xc->xc_Free)(_info, _base);                               \
  }                                                                     \
})
#elif defined(__amigaos4__)
#  define Callback_RecogData(_xc, _size, _data, _base) ({               \
  xadBOOL _cb_re;                                                       \
  if (IsNative(_xc->xc_RecogData)) {                                    \
    _cb_re = ((XCRECOGTYPE)_xc->xc_RecogData)(_size, _data, IxadMaster);\
  } else {                                                              \
    _cb_re = EmulateTags(_xc->xc_RecogData,                             \
      ET_RegisterD0, _size,                                             \
      ET_RegisterA0, _data,                                             \
      ET_RegisterA6, _base,                                             \
      ET_SaveRegs, TRUE,                                                \
      TAG_DONE);                                                        \
  }                                                                     \
  _cb_re;                                                               \
})
#  define Callback_GetInfo(_xc, _info, _base) ({                        \
  xadERROR _cb_re;                                                      \
  if (IsNative(_xc->xc_GetInfo)) {                                      \
    _cb_re = ((XCGETINFO)_xc->xc_GetInfo)(_info, IxadMaster);           \
  } else {                                                              \
    _cb_re = EmulateTags(_xc->xc_GetInfo,                               \
      ET_RegisterA0, _info,                                             \
      ET_RegisterA6, _base,                                             \
      ET_SaveRegs, TRUE,                                                \
      TAG_DONE);                                                        \
  }                                                                     \
  _cb_re;                                                               \
})
#  define Callback_UnArchive(_xc, _info, _base) ({                      \
  xadERROR _cb_re;                                                      \
  if (IsNative(_xc->xc_UnArchive)) {                                    \
    _cb_re = ((XCUNARCHIVE)_xc->xc_UnArchive)(_info, IxadMaster);       \
  } else {                                                              \
    _cb_re = EmulateTags(_xc->xc_UnArchive,                             \
      ET_RegisterA0, _info,                                             \
      ET_RegisterA6, _base,                                             \
      ET_SaveRegs, TRUE,                                                \
      TAG_DONE);                                                        \
  }                                                                     \
  _cb_re;                                                               \
})
#  define Callback_Free(_xc, _info, _base) ({                           \
  if (IsNative(_xc->xc_Free)) {                                         \
    ((XCFREE)_xc->xc_Free)(_info, IxadMaster);                          \
  } else {                                                              \
    EmulateTags(_xc->xc_Free,                                           \
      ET_RegisterA0, _info,                                             \
      ET_RegisterA6, _base,                                             \
      ET_SaveRegs, TRUE,                                                \
      TAG_DONE);                                                        \
  }                                                                     \
})
#else
#  define Callback_RecogData(_xc, _size, _data, _base) \
  ((XCRECOGTYPE)_xc->xc_RecogData)(_size, _data, _base)
#  define Callback_GetInfo(_xc, _info, _base) \
  ((XCGETINFO)_xc->xc_GetInfo)(_info, _base)
#  define Callback_UnArchive(_xc, _info, _base) \
  ((XCUNARCHIVE)_xc->xc_UnArchive)(_info, _base)
#  define Callback_Free(_xc, _info, _base) \
  ((XCFREE)_xc->xc_Free)(_info, _base)
#endif

#endif /* XADMASTER_PRIVDEFS_H */
