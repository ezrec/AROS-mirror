
#include "nsmtracker.h"
#include "disk.h"
#include "disk_patches_proc.h"

#include "disk_instrument_proc.h"



void SaveInstrument(struct Instruments *instrument){
DC_start("INSTRUMENT");

	DC_SSS("instrumentname",instrument->instrumentname);

	SavePatch(instrument->patches);

DC_end();
}

struct Instruments *LoadInstrument(void){
	static char *objs[1]={
		"PATCH"
	};
	static char *vars[1]={
		"instrumentname"
	};
	struct Instruments *instrument=DC_alloc(sizeof(struct Instruments));

	GENERAL_LOAD(1,1)


var0:
	instrument->instrumentname=DC_LoadS();
	goto start;

obj0:
	DC_ListAdd1(&instrument->patches,LoadPatch());
	goto start;

var1:
var2:
var3:
var4:
var5:
var6:
var7:
var8:
var9:
var10:
var11:
var12:
var13:
var14:
var15:
var16:
var17:
var18:

obj1:
obj2:
obj3:
obj4:
obj5:
obj6:

error:
end:
	return instrument;
}

int CAMDgetStandardVelocity(struct Tracks *track);
int CAMDgetMaxVelocity(struct Tracks *track);
void CAMDCloseInstrument(struct Instruments *instrument);
void CAMDSelectTrackInstrument(struct Tracks *track,struct Instruments *instrument);
void CAMDStopPlaying(struct Instruments *instrument);
int CAMDgetPatch(
	struct Tracker_Windows *window,
	ReqType reqtype,
	struct Tracks *track,
	struct Patch *patch
); 
extern int CAMDgetFX(struct Tracker_Windows *window,struct Tracks *track,struct FX *fx);
extern void CAMDPP_Update(struct Instruments *instrument,struct Patch *patch);
extern void *CAMD_CopyInstrumentData(struct Tracks *track);
extern void *CAMDLoadFX(struct FX *fx,struct Tracks *track);

#ifdef AMIAROS
#include "plug-ins/camd_playfromstart_proc.h"
#endif

void DLoadInstrument(struct Instruments *instrument){
#ifdef AMIAROS
	instrument->instrumentname="CAMD instrument";
	instrument->getStandardVelocity= &CAMDgetStandardVelocity;
	instrument->getMaxVelocity= &CAMDgetMaxVelocity;
	instrument->getFX= &CAMDgetFX;
	instrument->getPatch= &CAMDgetPatch;
	instrument->CloseInstrument=CAMDCloseInstrument;
	instrument->SelectTrackInstrument=CAMDSelectTrackInstrument;
	instrument->StopPlaying=CAMDStopPlaying;
	instrument->PP_Update=CAMDPP_Update;
	instrument->PlayFromStartHook=CAMDPlayFromStartHook;
	instrument->CopyInstrumentData=CAMD_CopyInstrumentData;
	instrument->LoadFX=CAMDLoadFX;
#endif
}


