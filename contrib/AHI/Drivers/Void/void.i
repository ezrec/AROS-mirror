
	IFND	_VOID_I_
_VOID_I_	SET	1

	include exec/types.i

	STRUCTURE voiddata,0
	UBYTE	v_Flags
	UBYTE	v_Pad1
	BYTE	v_MasterSignal
	BYTE	v_SlaveSignal
	APTR	v_MasterTask
	APTR	v_SlaveTask
	APTR	v_AHIsubBase
	APTR	v_MixBuffer

	LABEL	voiddata_SIZEOF

	ENDC	* _VOID_I_
