
	include	exec/types.i

	STRUCTURE filesave,0
	UBYTE	fs_Flags
	UBYTE	fs_Pad1
	UWORD	fs_DisableCount
	BYTE	fs_EnableSignal
	BYTE	fs_DisableSignal
	BYTE	fs_MasterSignal
	BYTE	fs_SlaveSignal
	APTR	fs_MasterTask
	APTR	fs_SlaveTask
	APTR	fs_FileReq
	APTR	fs_AHIsubBase
	ULONG	fs_Format
	APTR	fs_MixBuffer
	APTR	fs_SaveBuffer
	APTR	fs_SaveBuffer2
	ULONG	fs_SaveBufferSize;

	BYTE	fs_RecMasterSignal
	BYTE	fs_RecSlaveSignal
	APTR	fs_RecSlaveTask
	APTR	fs_RecFileReq
	APTR	fs_RecBuffer

	LABEL	filesave_SIZEOF

AHIDB_FileSaveFormat	EQU	AHIDB_UserBase+0	;Private tag
FORMAT_8SVX		EQU	0
FORMAT_AIFF		EQU	1
FORMAT_AIFC		EQU	2
FORMAT_S16		EQU	3
FORMAT_WAVE		EQU	4
