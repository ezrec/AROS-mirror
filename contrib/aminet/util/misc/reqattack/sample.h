#ifndef SAMPLE_H
#define SAMPLE_H

#define FORM_HDR "FORM"
#define ESVX_HDR "8SVX"
#define SXSV_HDR "16SV"
#define AIFF_HDR "AIFF"
#define RIFF_HDR "RIFF"
#define WAVE_HDR "WAVE"
#define COMM_HDR "COMM"
#define SVX_DATA "BODY"
#define AIF_DATA "SSND"
#define WAV_DATA "data"
#define VHDR_HDR "VHDR"

struct SoundNode
{
	ULONG *sampledata;
	ULONG *samples;
	ULONG	samplesize;
	ULONG	samplefreq;
	ULONG 	sampleloopstart;
	ULONG 	samplelooplen;
	ULONG	samplehandle;
	WORD  sampletype;
	ULONG pan;
	ULONG vol;
};

#endif

