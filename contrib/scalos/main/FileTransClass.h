// FileTransClass.h
// $Date$
// $Revision$


#ifndef	FILETRANSCLASS_H
#define	FILETRANSCLASS_H

#include "int64.h"

//----------------------------------------------------------------------------

// class instance data for "FileTransfer.sca"

struct FileTransClassInstance
	{
	ULONG	ftci_Number;

	UWORD	ftci_WindowWidth;		// Initial dimensions of Window
	UWORD	ftci_WindowHeight;

	ULONG	ftci_ReplaceMode;		// +jl+ 20010713
	ULONG	ftci_MostCurrentReplaceMode;	// effective ReplaceMode for last copy/move operation

	ULONG	ftci_OverwriteMode;

	ULONG	ftci_LinksMode; 		// user-defined preference: create soft links or hard links

	struct	Screen *ftci_Screen;
	struct	Window *ftci_Window;

	struct Gadget *ftci_GadgetList;		// Start of Gadget list for window

	struct	TextAttr ftci_TextAttr;		// TextAttr for Gadget texts

	T_TIMEVAL ftci_ProcessStartTime;	// time when processing of FTOps started (start counting)
	T_TIMEVAL ftci_CopyStartTime;		// time when processing of FTOps started (start copy/move)
	T_TIMEVAL ftci_LastRemainTimeDisplay;	// last time when remaining time was updated

	Object  *ftci_CancelButtonFrame;	// Frame image for cancel button
	Object  *ftci_CancelButtonImage;	// Theme Image for cancel button

	struct	Gadget *ftci_TextLineGadgets[6];
	struct	Gadget *ftci_GaugeGadget;
	struct	Gadget *ftci_CancelButtonGadget;

	STRPTR	ftci_Line1Buffer;		// Text Buffer for ftci_TextLineGadgets[0]
	STRPTR	ftci_Line2Buffer;		// Text Buffer for ftci_TextLineGadgets[1]
	char	ftci_Line3Buffer[60];
	char	ftci_Line4Buffer[60];
	char	ftci_Line5Buffer[60];
	char	ftci_Line6Buffer[60];

	ULONG	ftci_CopyCount;
	ULONG	ftci_HookResult;

	struct 	List ftci_OpList;
	SCALOSSEMAPHORE	ftci_OpListSemaphore;

	ULONG64	ftci_TotalBytes;
	ULONG64	ftci_CurrentBytes;

	ULONG	ftci_TotalItems;
	ULONG	ftci_CurrentItems;

	ULONG	ftci_TotalFiles;
	ULONG	ftci_TotalDirs;
	ULONG	ftci_TotalLinks;

	ULONG	ftci_CurrentOperation;		// type of current operation (FTOPCODE_Copy/FTOPCODE_Move)

	BOOL	ftci_CountValid;		// TRUE if ftci_TotalItems and ftci_TotalBytes are valid

	struct	DatatypesImage *ftci_Background;
	struct	Hook ftci_BackFillHook;

	enum FileTransTypeAction ftci_LastAction;	// Action performed when the error occurred
	enum FileTransOperation ftci_LastOp;	// Operation performed (Lock, Examine etc.) when error occurred
	LONG	ftci_LastErrorCode;		// code of last error
	STRPTR	ftci_LastErrorFileName;		// name of object on which last error occured
	};

// Gadget IDs
#define	GID_CancelButton	1013

#endif	/* FILETRANSCLASS_H */
