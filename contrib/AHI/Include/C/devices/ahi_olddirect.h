#ifndef DEVICES_AHI_H
#define DEVICES_AHI_H

/*
**	$VER: ahi.h 5.0 (14.1.98)
**	:ts=8 (TAB SIZE: 8)
**
**	ahi.device definitions
**
**	(C) Copyright 1994-1998 Martin Blom
**	All Rights Reserved.
**
*/

/*****************************************************************************/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef EXEC_IO_H
#include <exec/io.h>
#endif

#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif

#ifndef LIBRARIES_IFFPARSE_H
#include <libraries/iffparse.h>
#endif

/*****************************************************************************/


 /*** Fixed and sposition ***/

#ifndef EIGHTSVX_H

/* A fixed-point value, 16 bits	to the left	*/
/* of the point and 16 bits to the right.	*/

typedef LONG	Fixed;				
						
#endif /* EIGHTSVX_H */

typedef Fixed	sposition;



/****************************************************************************
** LOW-LEVEL INTERFACE DEFINITIONS FOLLOWS **********************************
****************************************************************************/


/*** STRUCTURES ************************************************************/

 /*** AHIAudioCtrl ***/

struct AHIAudioCtrl
{
	APTR	ahiac_UserData;
	/* Lots of private data follows! */
};

 /*** AHISoundMessage ***/

struct AHISoundMessage
{
	UWORD	ahism_Channel;
};

 /*** AHIRecordMessage ***/

struct AHIRecordMessage
{
	ULONG	ahirm_Type;			/* Format of buffer (object)	*/
	APTR	ahirm_Buffer;			/* Pointer to the sample array	*/
	ULONG	ahirm_Length;			/* # of samples in buffer	*/
};

 /*** AHISampleInfo ***/

struct AHISampleInfo
{
	ULONG	ahisi_Type;			/* Format of samples		*/
	APTR	ahisi_Address;			/* Address to sample array	*/
	ULONG	ahisi_Length;			/* # of samples in array	*/
};


 /*** AHIAudioModeRequester ***/

struct AHIAudioModeRequester
{
	ULONG	ahiam_AudioID;			/* Selected audio mode		*/
	ULONG	ahiam_MixFreq;			/* Selected mixing frequency	*/
	
	WORD	ahiam_LeftEdge;			/* Coords of requester on exit	*/
	WORD	ahiam_TopEdge;
	WORD	ahiam_Width;
	WORD	ahiam_Height;

	BOOL	ahiam_InfoOpened;		/* Info window opened on exit?	*/
	WORD	ahiam_InfoLeftEdge;		/* Last coords of Info window	*/
	WORD	ahiam_InfoTopEdge;
	WORD	ahiam_InfoWidth;
	WORD	ahiam_InfoHeight;

	APTR	ahiam_UserData;			/* Store whatever you want here	*/
	/* Lots of private data follows! */
};


 /*** Effects ***/

  /* AHIEffMasterVolume */

struct AHIEffMasterVolume
{
	ULONG	ahie_Effect;			/* Set to AHIET_MASTERVOLUME	*/
	Fixed	ahiemv_Volume;			/* See autodocs for range!	*/
};

  /* AHIEffOutputBuffer */

struct AHIEffOutputBuffer
{
	ULONG		 ahie_Effect;		/* Set to AHIET_OUTPUTBUFFER	*/
	struct Hook	*ahieob_Func;

 /* These fields are filled by AHI: */

	ULONG		 ahieob_Type;		/* Format of buffer		*/
	APTR		 ahieob_Buffer;		/* Pointer to the sample array	*/
	ULONG		 ahieob_Length;		/* # of sample frames in buffer	*/
};

  /* AHIEffDSPMask (V4) */

struct AHIEffDSPMask
{
	ULONG	ahie_Effect;			/* Set to AHIET_DSPMASK		*/
	UWORD	ahiedm_Channels;		/* Number of elements in array	*/
	UBYTE	ahiedm_Mask[0];			/* Here follows the array	*/
};

#define AHIEDM_WET		(0U)
#define AHIEDM_DRY		(1U)


  /* AHIEffDSPEcho (V4) */

struct AHIEffDSPEcho
{
	ULONG	ahie_Effect;			/* Set to AHIET_DSPECHO		*/
	ULONG	ahiede_Delay;			/* Delay (in samples)		*/
	Fixed	ahiede_Feedback;
	Fixed	ahiede_Mix;
	Fixed	ahiede_Cross;
};


  /* AHIEffChannelInfo (V4) */

struct AHIEffChannelInfo
{
	ULONG		 ahie_Effect;		/* Set to AHIET_CHANNELINFO	*/
	struct Hook	*ahieci_Func;
	UWORD		 ahieci_Channels;
	UWORD		 ahieci_Pad;

 /* The rest is filled by AHI: */

 	ULONG		 ahieci_Offset[0];	/* The array follows 		*/
};




/*** TAGS ******************************************************************/


 /*** Tag bases ***/

#define AHI_TagBase		(TAG_USER)
#define AHI_TagBaseR		(AHI_TagBase|0x8000)

  /* Just shorter names */

#define AHITB AHI_TagBase
#define AHITR AHI_TagBaseR


 /*** AHI_AllocAudioA tags ***/

#define AHIA_AudioID		(AHITB+1)	/* Desired audio mode		*/
#define AHIA_MixFreq		(AHITB+2)	/* Suggested mixing frequency	*/
#define AHIA_Channels		(AHITB+3)	/* Suggested number of channels */
#define AHIA_Sounds		(AHITB+4)	/* Number of sounds to use	*/
#define AHIA_SoundFunc		(AHITB+5)	/* End-of-Sound Hook		*/
#define AHIA_PlayerFunc		(AHITB+6)	/* Player Hook			*/
#define AHIA_PlayerFreq		(AHITB+7)	/* Frequency for player Hook	*/
#define AHIA_MinPlayerFreq	(AHITB+8)	/* Min. freq. for player Hook	*/
#define AHIA_MaxPlayerFreq	(AHITB+9)	/* Max. freq. for player Hook	*/
#define AHIA_RecordFunc		(AHITB+10)	/* Sample recording Hook	*/
#define AHIA_UserData		(AHITB+11)	/* Set ahiac_UserData		*/
#define AHIA_ErrorFunc		(AHITB+12)	/* Error Hook (V5)		*/
#define AHIA_AntiClickSamples	(AHITB+13)	/* # of samples to smooth (v5)	*/


 /*** AHI_PlayA tags (V4) ***/

#define AHIP_BeginChannel	(AHITB+40)	/* All command tags should be	*/
#define AHIP_EndChannel		(AHITB+41)	/* enclosed by these tags.	*/
#define AHIP_Freq		(AHITB+50)
#define AHIP_Vol		(AHITB+51)
#define AHIP_Pan		(AHITB+52)
#define AHIP_Sound		(AHITB+53)
#define AHIP_Offset		(AHITB+54)
#define AHIP_Length		(AHITB+55)
#define AHIP_LoopFreq		(AHITB+60)
#define AHIP_LoopVol		(AHITB+61)
#define AHIP_LoopPan		(AHITB+62)
#define AHIP_LoopSound		(AHITB+63)
#define AHIP_LoopOffset		(AHITB+64)
#define AHIP_LoopLength		(AHITB+65)


 /*** AHI_ControlAudioA tags ***/

#define AHIC_Play		(AHITB+80)	/* Control playback		*/
#define AHIC_Record		(AHITB+81)	/* Control recording		*/
#define AHIC_MixFreq_Query	(AHITB+84)	/* Get mixing frequency	(in Hz)	*/

    /* NOTE: The argument to or result from these tags is an index.		*/
    /* Use the AHI_GetAudioAttrsA() to find out the value.			*/

#define AHIC_Input			(AHITB+89)  /* Active input (V2)	*/
#define AHIC_Input_Query		(AHITB+90)
#define AHIC_Output			(AHITB+91)  /* Active output (V2)	*/
#define AHIC_Output_Query		(AHITB+92)
#define AHIC_MonitorVolumeLeft		(AHITB+300) /* Monitor volume (V5)	*/
#define AHIC_MonitorVolumeLeft_Query	(AHITB+301)
#define AHIC_MonitorVolumeRight		(AHITB+302)
#define AHIC_MonitorVolumeRight_Query	(AHITB+303)
#define AHIC_OutputVolumeLeft		(AHITB+304) /* Volume attenuation (V5)	*/
#define AHIC_OutputVolumeLeft_Query	(AHITB+305) 
#define AHIC_OutputVolumeRight		(AHITB+306)
#define AHIC_OutputVolumeRight_Query	(AHITB+307)
#define AHIC_InputGainLeft		(AHITB+308) /* Input gain (V5)		*/
#define AHIC_InputGainLeft_Query	(AHITB+309)
#define AHIC_InputGainRight		(AHITB+310)
#define AHIC_InputGainRight_Query	(AHITB+311)

    /* The following tags only have meaning in the direct modes */

#define AHIC_Frequency			(AHITB+312) /* Sample freq. index (V5)	*/
#define AHIC_Frequency_Query		(AHITB+313)
#define AHIC_PlaySampleFormat		(AHITB+314) /* Sample format index (V5)	*/
#define AHIC_PlaySampleFormat_Query	(AHITB+315)
#define AHIC_RecordSampleFormat		(AHITB+316)
#define AHIC_RecordSampleFormat_Query	(AHITB+317)
#define AHIC_DirectLength		(AHITB+318) /* Length index (V5) 	*/
#define AHIC_DirectLength_Query		(AHITB+319)
#define AHIC_DirectPlayBuffer		(AHITB+320) /* Write buffer (V5) 	*/
#define AHIC_DirectPlayBuffer_Query	(AHITB+321)
#define AHIC_DirectRecordBuffer		(AHITB+322) /* Read buffer (V5)		*/
#define AHIC_DirectRecordBuffer_Query	(AHITB+323)

 /*** AHI_GetAudioAttrsA tags ***/

  /* General driver information tags */
#define AHIDB_AudioID		(AHITB+100)
#define AHIDB_BufferLen		(AHITB+122)	/* Set the string buffer size	*/
#define AHIDB_Driver		(AHITR+101)	/* Pointer to name of driver	*/
#define AHIDB_Author		(AHITB+118)	/* Driver author name		*/
#define AHIDB_Copyright		(AHITB+119)	/* Driver copyright notice	*/
#define AHIDB_Version		(AHITB+120)	/* Driver version string	*/
#define AHIDB_Annotation	(AHITB+121)	/* Driver annotation text	*/

#define AHIDB_Name		(AHITR+109)	/* Pointer to name of this mode */
#define AHIDB_Data		(AHITR+142)	/* Private! (V4)		*/
#define AHIDB_Flags		(AHITB+102)	/* Private!			*/

  /* Playback device, sampler or both? */
#define AHIDB_Play		(AHITB+183)	/* Can play sounds? (V5)	*/
#define AHIDB_Record		(AHITB+114)	/* Can record sounds?		*/

  /* Direct or channel based mode? */
#define AHIDB_Direct		(AHITB+178)	/* Direct mode? (V5)		*/

  /* Properties for direct modes */
#define AHIDB_DirectLengths		(AHITB+179) /* Supported "FIFO" lengths */
#define AHIDB_DirectLengthArg		(AHITB+180)
#define AHIDB_DirectLength		(AHITB+181)
#define AHIDB_DirectLengthArray		(AHITB+182)

  /* Properties for channel based modes */
#define AHIDB_Volume		(AHITB+103)	/* Volume supported?		*/
#define AHIDB_Stereo		(AHITB+105)	/* Stereo mode?			*/
#define AHIDB_Panning		(AHITB+104)	/* Panning supported?		*/
#define AHIDB_Surround		(AHITB+174)	/* Surround sound support? (V5)	*/
#define AHIDB_PingPong		(AHITB+107)	/* Can play backwards?		*/
#define AHIDB_MultTable		(AHITB+108)	/* Private!			*/
#define AHIDB_MaxChannels	(AHITB+111)	/* Max supported channels	*/

#define AHIDB_MaxPlaySamples	(AHITB+126)	/* Min playbuffer length	*/
#define AHIDB_MaxRecordSamples	(AHITB+127)	/* Min recordbuffer length	*/


  /* Properties for all modes follows... */

#define AHIDB_Bits		(AHITB+110)	/* Output bits			*/
#define AHIDB_HiFi		(AHITB+106)	/* HiFi mode?			*/
#define AHIDB_Realtime		(AHITB+125)	/* Realtime mode?		*/
#define AHIDB_FullDuplex	(AHITB+129)	/* Can play/record simult.?	*/
#define AHIDB_Accelerated	(AHITB+176)	/* Level of accel. (V5)		*/
#define AHIDB_Available		(AHITB+177)	/* Is mode available? (V5)	*/
#define AHIDB_Hidden		(AHITB+175)	/* Don't show in mode req. (V5)	*/

#define AHIDB_Frequencies	(AHITB+115)	/* Get # of frequencies		*/
#define AHIDB_FrequencyArg	(AHITB+116)
#define AHIDB_Frequency		(AHITB+117)	/* Get frequency		*/
#define AHIDB_FrequencyArray	(AHITB+143)	/* Get all frequencies (V5)	*/
#define AHIDB_IndexArg		(AHITB+123)
#define AHIDB_Index		(AHITB+124)	/* Get index of a frequency	*/

#define AHIDB_Inputs		(AHITB+136)	/* Input connectors (V2)	*/
#define AHIDB_InputArg		(AHITB+137)
#define AHIDB_Input		(AHITB+138)
#define AHIDB_InputArray	(AHITB+144)	/* (V5)				*/
#define AHIDB_Outputs		(AHITB+139)	/* Output connectors (V2)	*/
#define AHIDB_OutputArg		(AHITB+140)
#define AHIDB_Output		(AHITB+141)
#define AHIDB_OutputArray	(AHITB+145)	/* (V5)				*/


    /* NOTE: All the follwing tags speak dB stored as Fixed, unlike 		*/
    /* the previous (pre-5) tags which used linear stored as Fixed.		*/
    /* They are used to find the available settings of the monitor volume,	*/
    /* output volume and input gain on the left or right channel. (V5)		*/

#define AHIDB_MonitorVolumesLeft	(AHITB+146) /* Monitor volume (V5)	*/
#define AHIDB_MonitorVolumeLeftArg	(AHITB+147)
#define AHIDB_MonitorVolumeLeft		(AHITB+148)
#define AHIDB_MonitorVolumeLeftArray	(AHITB+149)
#define AHIDB_MonitorVolumesRight	(AHITB+150)
#define AHIDB_MonitorVolumeRightArg	(AHITB+151)
#define AHIDB_MonitorVolumeRight	(AHITB+152)
#define AHIDB_MonitorVolumeRightArray	(AHITB+153)
#define AHIDB_OutputVolumesLeft		(AHITB+154) /* Volume attenuation (V5)	*/
#define AHIDB_OutputVolumeLeftArg	(AHITB+155)
#define AHIDB_OutputVolumeLeft		(AHITB+156)
#define AHIDB_OutputVolumeLeftArray	(AHITB+157)
#define AHIDB_OutputVolumesRight	(AHITB+158)
#define AHIDB_OutputVolumeRightArg	(AHITB+159)
#define AHIDB_OutputVolumeRight		(AHITB+160)
#define AHIDB_OutputVolumeRightArray	(AHITB+161)
#define AHIDB_InputGainsLeft		(AHITB+162) /* Input gain (V5)		*/
#define AHIDB_InputGainLeftArg		(AHITB+163)
#define AHIDB_InputGainLeft		(AHITB+164)
#define AHIDB_InputGainLeftArray	(AHITB+165)
#define AHIDB_InputGainsRight		(AHITB+166)
#define AHIDB_InputGainRightArg		(AHITB+167)
#define AHIDB_InputGainRight		(AHITB+168)
#define AHIDB_InputGainRightArray	(AHITB+169)

     /* Use the following tags to find out which formats can be used for	*/
     /* playback and recording.	(V5)						*/

#define AHIDB_PlaySampleFormats		(AHITB+170) /* Playback formats (V5)	*/
#define AHIDB_PlaySampleFormatArg	(AHITB+167)
#define AHIDB_PlaySampleFormat		(AHITB+168)
#define AHIDB_PlaySampleFormatArray	(AHITB+169)
#define AHIDB_RecordSampleFormats	(AHITB+170) /* Recording formats (V5)	*/
#define AHIDB_RecordSampleFormatArg	(AHITB+171)
#define AHIDB_RecordSampleFormat	(AHITB+172)
#define AHIDB_RecordSampleFormatArray	(AHITB+173)

/* 					       183 is the last			*/


 /*** AHI_BestAudioIDA tags ***/

#define AHIB_Dizzy		(AHITB+190)	/* (V4)				*/


 /*** AHI_AudioRequestA tags ***/

  /* Window control */

#define AHIR_Window		(AHITB+200)	/* Parent window		*/
#define AHIR_Screen		(AHITB+201)	/* Screen to open on if no window */
#define AHIR_PubScreenName	(AHITB+202)	/* Name of public screen	*/
#define AHIR_PrivateIDCMP	(AHITB+203)	/* Allocate private IDCMP?	*/
#define AHIR_IntuiMsgFunc	(AHITB+204)	/* Function to handle IntuiMessages */
#define AHIR_SleepWindow	(AHITB+205)	/* Block input in AHIR_Window?	*/
#define AHIR_UserData		(AHITB+206)	/* Set ahiam_UserData		*/

  /* Text display */

#define AHIR_TextAttr		(AHITB+220)	/* Text font for gadget text	*/
#define AHIR_Locale		(AHITB+221)	/* Locale to use for text	*/
#define AHIR_TitleText		(AHITB+222)	/* Title of requester		*/
#define AHIR_PositiveText	(AHITB+223)	/* Positive gadget text		*/
#define AHIR_NegativeText	(AHITB+224)	/* Negative gadget text		*/

  /* Initial settings */

#define AHIR_InitialLeftEdge	(AHITB+240)	/* Initial requester coords	*/
#define AHIR_InitialTopEdge	(AHITB+241)
#define AHIR_InitialWidth	(AHITB+242)	/* Initial requester dimensions	*/
#define AHIR_InitialHeight	(AHITB+243)
#define AHIR_InitialAudioID	(AHITB+244)	/* Initial audio mode id	*/
#define AHIR_InitialMixFreq	(AHITB+245)	/* Initial mixingfrequency	*/
#define AHIR_InitialInfoOpened	(AHITB+246)	/* Info window initially opened?*/
#define AHIR_InitialInfoLeftEdge (AHITB+247)	/* Initial Info window coords.	*/
#define AHIR_InitialInfoTopEdge (AHITB+248)
#define AHIR_InitialInfoWidth	(AHITB+249)	/* Not used!			*/
#define AHIR_InitialInfoHeight	(AHITB+250)	/* Not used!			*/

  /* Options */

#define AHIR_DoMixFreq		(AHITB+260)	/* Show frequency slider?	*/
#define AHIR_DoDefaultMode	(AHITB+261)	/* Show default audio mode? (V4)*/
#define AHIR_DoChannels		(AHITB+262)	/* Allow channel slider? (V5)	*/
#define AHIR_DoHidden		(AHITB+263)	/* Show hidden modes? (V5)	*/
#define AHIR_DoDirectModes	(AHITB+264)	/* Show direct modes? (V5)	*/

  /* Filtering */

#define AHIR_FilterTags		(AHITB+270)	/* Pointer to filter taglist	*/
#define AHIR_FilterFunc		(AHITB+271)	/* Function to filter mode id's	*/



#undef AHITB
#undef AHITR

/*** Constants *************************************************************/


 /*** Audio mode ID ***/

#define AHI_INVALID_ID		(~0UL)		/* Invalid Audio ID		*/
#define AHI_DEFAULT_ID		(0x00000000UL)	/* Only for AHI_AllocAudioA()!	*/
#define AHI_LOOPBACK_ID		(0x00000001UL)	/* Unimplemented		*/

#define AHI_DEFAULT_FREQ	(0UL)		/* Only for AHI_AllocAudioA()!	*/
#define AHI_EXTERNAL_FREQ	(~0UL)		/* External mixfreq (V5)	*/
#define AHI_MIXFREQ		(~0UL)		/* Special AHI_SetFreq() freq.	*/
#define AHI_NOSOUND		(0xffffU)	/* Turns a channel off		*/


 /*** AHIDB_Accelerated flags (V5) ***/

#define AHIACCF_ACCELERATED	(1UL<<0)	/* General HW acceleration	*/
#define AHIACCF_POSTPROC	(1UL<<1)	/* Hardware postprocessing	*/
#define AHIACCF_SAMPLE		(1UL<<2)	/* Hardware AHIST_SAMPLE axl	*/
#define AHIACCF_DYNAMICSAMPLE	(1UL<<3)	/* HW AHIST_DYNAMICSAMPLE axl	*/

#define AHIACCF_NONE		(0UL)		/* No hardware acceleration	*/

#define AHIACCB_ACCELERATED	(0UL)
#define AHIACCB_POSTPROC	(1UL)
#define AHIACCB_SAMPLE		(2UL)
#define AHIACCB_DYNAMICSAMPLE	(3UL)



 /*** Set#? Flags ***/

#define AHISF_IMM		(1UL<<0)	/* Trigger action immediately	*/

#define AHISF_NONE		(0UL)		/* No flags (V5)		*/

#define AHISB_IMM		(0UL)



 /*** Effect Types ***/

#define AHIET_CANCEL		(1UL<<31)	/* OR with effect to disable	*/
#define AHIET_MASTERVOLUME	(1UL)		/* Software volume boost	*/
#define AHIET_OUTPUTBUFFER	(2UL)		/* Read-only access to mixbuf.	*/
#define AHIET_DSPMASK		(3UL)		/* Channels to perform on (V4)	*/
#define AHIET_DSPECHO		(4UL)		/* Echo/Delay (V4)		*/
#define AHIET_CHANNELINFO	(5UL)		/* Get offsets of channels (V4)	*/


 /*** Sound Types ***/

#define AHIST_NOTYPE		(~0UL)		/* Private!			*/
#define AHIST_SAMPLE		(0UL)		/* 8 or 16 bit sample		*/
#define AHIST_DYNAMICSAMPLE	(1UL)		/* Dynamic sample		*/
#define AHIST_INPUT		(1UL<<29)	/* The input from your sampler	*/


 /*** Sample types ***/

    /* Use AHIDB_PlaySampleFormat/AHIDB_RecordSampleFormat to find out which	*/
    /* formats can be used for playback or recording!				*/
    /* AHI_LoadSound() V2 supported AHIST_M8S, AHIST_M16S and AHIST_M8U.	*/
    /* AHI_LoadSound() V4 added AHIST_S8S and AHIST_S16S and dropped AHIST_M8U.	*/

  /* Normal PCM formats */

#define AHIST_M8S		(0UL)		/* Mono, 8 bit signed		*/
#define AHIST_M16S		(1UL)		/* Mono, 16 bit signed		*/
#define AHIST_S8S		(2UL)		/* Stereo, 8 bit signed		*/
#define AHIST_S16S		(3UL)		/* Stereo, 16 bit signed	*/
#define AHIST_M8U		(4UL)		/* Mono, 8 bit unsigned		*/
#define AHIST_M16U		(5UL)		/* Mono, 16 bit unsigned (V5)	*/
#define AHIST_S8U		(6UL)		/* Stereo, 8 bit unsigned (V5)	*/
#define AHIST_S16U		(7UL)		/* Stereo, 16 bit unsigned (V5)	*/
#define AHIST_M32S		(8UL)		/* Mono, 32 bit signed		*/
#define AHIST_S32S		(10UL)		/* Stereo, 32 bit signed	*/

  /* Compressed formats */

#define AHIST_MULAW		(32UL)		/* Mono, 8 bit µ-Law (V5)	*/
#define AHIST_MALAW		(33UL)		/* Mono, 8 bit A-Law (V5)	*/
#define AHIST_SULAW		(34UL)		/* Stereo, 8 bit µ-Law (V5)	*/
#define AHIST_SALAW		(35UL)		/* Stereo, 8 bit A-Law (V5)	*/
#define AHIST_MADPCM4		(36UL)		/* Mono, 4 bit ADPCM (IMA) (V5)	*/
#define AHIST_SADPCM4		(38UL)		/* Stereo, 4 bit ADPCM (V5)	*/

#define AHIST_BW		(1UL<<30)	/* Private!			*/



 /*** Error codes ***/

#define AHIE_OK			(0UL)		/* No error			*/
#define AHIE_NOMEM		(1UL)		/* Out of memory		*/
#define AHIE_BADSOUNDTYPE	(2UL)		/* Unknown sound type		*/
#define AHIE_BADSAMPLETYPE	(3UL)		/* Unsupported sample type	*/
#define AHIE_ABORTED		(4UL)		/* User-triggered abortion	*/
#define AHIE_UNKNOWN		(5UL)		/* Error, but unknown		*/
#define AHIE_HALFDUPLEX		(6UL)		/* Can't play/record simult.	*/
#define AHIE_OVERFLOW		(7UL)		/* Buffer overflow		*/
#define AHIE_UNDERFLOW		(8UL)		/* Buffer underflow		*/
#define AHIE_INPUTCLIPPED	(9UL)		/* Input signal out of range	*/
#define AHIE_OUTPUTCLIPPED	(10UL)		/* Output signal out of range	*/


/*** Obsolete stuff ********************************************************/

    /* Obsolete tags and definitions. Define AHI_V5_NAMES_ONLY to exclude	*/
    /* these definitions and use only the new ones.				*/

#ifndef AHI_V5_NAMES_ONLY

 /*** AHI_ControlAudioA tags ****/
#define AHIC_MonitorVolume	(AHITB+82)
#define AHIC_MonitorVolume_Query (AHITB+83)
#define AHIC_InputGain		(AHITB+85)
#define AHIC_InputGain_Query	(AHITB+86)
#define AHIC_OutputVolume	(AHITB+87)
#define AHIC_OutputVolume_Query	(AHITB+88)


 /*** AHI_GetAudioAttrsA tags ***/
#define AHIDB_MinMixFreq	(AHITB+112)
#define AHIDB_MaxMixFreq	(AHITB+113)
#define AHIDB_MinMonitorVolume	(AHITB+130)
#define AHIDB_MaxMonitorVolume	(AHITB+131)
#define AHIDB_MinInputGain	(AHITB+132)
#define AHIDB_MaxInputGain	(AHITB+133)
#define AHIDB_MinOutputVolume	(AHITB+134)
#define AHIDB_MaxOutputVolume	(AHITB+135)

 /*** Effects ****/
#define AHIDSPEcho AHIEffDSPEcho		/* Fix for error in V4 includes	*/

#endif /* AHI_V5_NAMES_ONLY */



/****************************************************************************
** DEVICE INTERFACE DEFINITIONS FOLLOWS *************************************
****************************************************************************/

/*** Device units **********************************************************/

#define AHINAME			"ahi.device"

#define AHI_DEFAULT_UNIT	(0U)
#define AHI_NO_UNIT		(255U)


 /**** Flags for OpenDevice() ****/

#define	AHIDF_NOMODESCAN	(1UL<<0)	/* Don't build mode database...	*/
#define	AHIDB_NOMODESCAN	(0UL)		/* on startup if not existing.	*/


 /*** AHIRequest ****/

struct AHIRequest
{
	struct	IOStdReq   ahir_Std;		/* Standard IO request		*/
	UWORD		   ahir_Version;	/* Required version		*/
/* --- New for V4, they will be ignored by V2 and earlier --- */
	UWORD		   ahir_Pad1;
	ULONG		   ahir_Private[2];	/* Hands off!			*/
	ULONG		   ahir_Type;		/* Sample format		*/
	ULONG		   ahir_Frequency;	/* Sample/Record frequency	*/
	Fixed		   ahir_Volume;		/* Sample volume		*/
	Fixed		   ahir_Position;	/* Stereo position		*/
	struct AHIRequest *ahir_Link;		/* For double buffering		*/
};



/*** The preference file ***************************************************/

#define ID_AHIU MAKE_ID('A','H','I','U')
#define ID_AHIG MAKE_ID('A','H','I','G')

struct AHIUnitPrefs
{
	UBYTE	ahiup_Unit;
	UBYTE	ahiup_Pad;
	UWORD	ahiup_Channels;
	ULONG	ahiup_AudioMode;
	ULONG	ahiup_Frequency;
	Fixed	ahiup_MonitorVolume;
	Fixed	ahiup_InputGain;
	Fixed	ahiup_OutputVolume;
	ULONG	ahiup_Input;
	ULONG	ahiup_Output;
};

struct AHIGlobalPrefs
{
	UWORD	ahigp_DebugLevel;		/* See below.			*/
	BOOL	ahigp_DisableSurround;
	BOOL	ahigp_DisableEcho;
	BOOL	ahigp_FastEcho;
	Fixed	ahigp_MaxCPU;
	BOOL	ahigp_ClipMasterVolume;
};

 /*** Debug levels ****/

#define AHI_DEBUG_NONE		(0U)
#define AHI_DEBUG_LOW		(1U)
#define AHI_DEBUG_HIGH		(2U)
#define AHI_DEBUG_ALL		(3U)

#endif /* DEVICES_AHI_H */
