#ifndef DOS_DOS_PRIVATE_H
#define DOS_DOS_PRIVATE_H 1

#if !defined(__amigaos4__)

#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/ports.h>
#include <exec/semaphores.h>
#include <devices/timer.h>

struct PrivateErrorString {
	LONG  *estr_Nums;
	UBYTE *estr_Strings;
};

#include "amiga-align.h"

struct PrivateDosLibrary {
    struct Library dl_lib;
    struct PrivateRootNode *dl_Root; /* Pointer to RootNode, described below */
    APTR    dl_GV;	      /* Pointer to BCPL global vector	      */
    LONG    dl_A2;	      /* BCPL standard register values	      */
    LONG    dl_A5;
    LONG    dl_A6;
    struct PrivateErrorString *dl_Errors;    /* PRIVATE pointer to array of error msgs */
    struct timerequest *dl_TimeReq;	  /* PRIVATE pointer to timer request */
    struct Library     *dl_UtilityBase;   /* PRIVATE ptr to utility library */
    struct Library     *dl_IntuitionBase; /* PRIVATE ptr to intuition library */
};  /*	DosLibrary */

struct PrivateRootNode {
    BPTR    rn_TaskArray;	     /* [0] is max number of CLI's
				      * [1] is APTR to process id of CLI 1
				      * [n] is APTR to process id of CLI n */
    BPTR    rn_ConsoleSegment; /* SegList for the CLI			   */
    struct  DateStamp rn_Time; /* Current time				   */
    LONG    rn_RestartSeg;     /* SegList for the disk validator process   */
    BPTR    rn_Info;	       /* Pointer to the Info structure		   */
    BPTR    rn_FileHandlerSegment; /* segment for a file handler	   */
    struct MinList rn_CliList; /* new list of all CLI processes */
			       /* the first cpl_Array is also rn_TaskArray */
    struct MsgPort *rn_BootProc; /* private ptr to msgport of boot fs	   */
    BPTR    rn_ShellSegment;   /* seglist for Shell (for NewShell)	   */
    LONG    rn_Flags;	       /* dos flags */
};  /* RootNode */

struct PrivateDosInfo {
    BPTR    di_McName;	       /* PRIVATE: system resident module list	    */
#define di_ResList di_McName
    BPTR    di_DevInfo;	       /* Device List				    */
    BPTR    di_Devices;	       /* Currently zero			    */
    BPTR    di_Handlers;       /* Currently zero			    */
    APTR    di_NetHand;	       /* Network handler processid; currently zero */
    struct  SignalSemaphore di_DevLock;	   /* do NOT access directly! */
    struct  SignalSemaphore di_EntryLock;  /* do NOT access directly! */
    struct  SignalSemaphore di_DeleteLock; /* do NOT access directly! */
};  /* DosInfo */

#include "default-align.h"

#endif /* __amigaos4__ */

#endif /* DOS_DOS_PRIVATE_H */

