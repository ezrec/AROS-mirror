
#ifndef  _VOID_H_
#define  _VOID_H_

#include <exec/types.h>
#include <exec/libraries.h>
#include <dos/dosextens.h>

struct voiddata {
	UBYTE			v_Flags;
	UBYTE			v_Pad1;
	BYTE			v_MasterSignal;
	BYTE			v_SlaveSignal;
	struct Process*		v_MasterTask;
	struct Process*		v_SlaveTask;
	struct Library*		v_AHIsubBase;
	APTR			v_MixBuffer;
};

#endif /* _VOID_H_ */
