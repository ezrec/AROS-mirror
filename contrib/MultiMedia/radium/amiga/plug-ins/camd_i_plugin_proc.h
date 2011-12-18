

#ifndef TRACKER_INCLUDE

struct MidiLink;

void CAMDchangeTrackPan(int newpan,struct Tracks *track);
void CAMDchangevelocity(int velocity,struct Tracks *track,struct Notes *note);
void CAMDstopnote(int notenum,int velocity, struct Tracks *track,struct Notes *note);
void CAMDplaynote(int notenum, int velocity, struct Tracks *track,struct Notes *note);
void CAMDclosePatch(void);

extern void MyPutMidi(
	struct MidiLink *midilink,
	ULONG msg,
	int maxbuff,
	int skip
);

#endif




/***************** General ****************************/

#define PutMidi3(a,b,c,d,s) MyPutMidi(a,(ULONG)(((b)<<24)|((c)<<16)|((d)<<8)),s,0)
#define PutMidi2(a,b,c,s) MyPutMidi(a,(ULONG)(((b)<<24)|((c)<<16)),s,0)

#define PutMidi3_FX(a,b,c,d,s,skip) MyPutMidi(a,(ULONG)(((b)<<24)|((c)<<16)|((d)<<8)),s,skip)
#define PutMidi2_FX(a,b,c,s,skip) MyPutMidi(a,(ULONG)(((b)<<24)|((c)<<16)),s,skip)

#define D_PutMidi3(a,b,c,d) GoodPutMidi(a,(ULONG)(((b)<<24)|((c)<<16)|((d)<<8)),1000)
#define D_PutMidi2(a,b,c) GoodPutMidi(a,(ULONG)(((b)<<24)|((c)<<16)),1000)

#define R_PutMidi3(a,b,c,d) PutMidi(a,(ULONG)(((b)<<24)|((c)<<16)|((d)<<8)))
#define R_PutMidi2(a,b,c) PutMidi(a,(ULONG)(((b)<<24)|((c)<<16)))


