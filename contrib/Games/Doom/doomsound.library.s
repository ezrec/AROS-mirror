;------------------------------------------------------------------------
; Sound Library for DOOM - by Joseph Fenton
;
; Functions:
;	Sfx_SetVol - pass in the new sound effects volume in d0.
;	Sfx_Start - pass in new sound effect to play. a0 = wave,
;		d0 = cnum, d1 = pitch, d2 = vol, d3 = sep, d4 =
;		length.
;	Sfx_Update - pass in new settings for a sfx. a0 = data,
;		d0 = cnum, d1 = pitch, d2 = vol, d3 = sep.
;	Sfx_Stop - stop a playing sound effect. d0 = cnum.
;	Sfx_Done - check if sound effect playing. d0 = cnum; returns
;		d0 = 0 if not playing/current index if playing.
;	Mus_SetVol - pass in the new music volume in d0.
;	Mus_Register - pass in pointer to MUS data to register in
;		a0. Returns handle in d0.
;	Mus_Unregister - pass in handle to music to unregister in d0.
;	Mus_Play - pass in handle to music to start playing in d0,
;		pass the looping flag in d1.
;	Mus_Stop - pass in handle to music to stop playing in d0.
;	Mus_Pause - pass in handle to music to pause in d0.
;	Mus_Resume - pass in handle to music to resume in d0.
;	Mus_Done - return d0 = 0 if music not playing
;
;------------------------------------------------------------------------


_custom		equ	$DFF000

;------------------------------------------------------------------------

		include	devpac:system.gs	; all includes/LVOs

;		include	"exec/exec.i"
;		include	"exec/funcdef.i"
;		include	"exec/exec_lib.i"
;		include	"dos/dos.i"
;		include	"dos/dos_lib.i"
;		include	"graphics/gfxbase.i"
;		include	"devices/audio.i"
;		include	"hardware/custom.i"
;		include	"hardware/intbits.i"
;		include	"hardware/dmabits.i"
;
;CALLSYS		macro
;		jsr	(_LVO\1,a6)
;		endm
;
;CALLDOS		macro
;		movea.l	(_DOSBase,pc),a6
;		jsr	(_LVO\1,a6)
;		endm

		output	libs:doomsound.library

;------------------------------------------------------------------------

VERSION		equ 37			;Version #
REVISION	equ 0			;revision of the version

_VER		equ (VERSION/10)
VERHIGH		equ _VER|$30
VERLOW		equ (VERSION-(_VER*10))|$30
_REV		equ (REVISION/10)
REVHIGH		equ _REV|$30
REVLOW		equ (REVISION-(_REV*10))|$30

OUTPUT_VER	MACRO
		ifge VERHIGH-$31
		dc.b VERHIGH
		endc
		dc.b VERLOW
		dc.b '.'
		ifge REVHIGH-$31
		dc.b REVHIGH
		endc
		dc.b REVLOW
		ENDM

OUTPUT_DATE	MACRO
		dc.b ' (29.3.98)'
		ENDM


;--------------------------------------------------------------------
; in case of idiots

		moveq	#0,d0
		rts

;--------------------------------------------------------------------
; standard library header


InitDescript	dc.w	$4AFC		;this is the magic cookie
		dc.l	InitDescript
		dc.l	EndCode		;end of code to checksum
		dc.b	RTF_AUTOINIT
		dc.b	VERSION		;version #
		dc.b	NT_LIBRARY	;node type (library)
		dc.b	0		;node priority
		dc.l	Name		;pointer to the library name
		dc.l	IDString	;pointer to ID string
		dc.l	Init		;pointer to initialization data


Name		dc.b	'doomsound.library',0
		even

IDString	dc.b	'doomsound.library '
		OUTPUT_VER
		OUTPUT_DATE
		dc.b	' ©1998 by Joe Fenton.',0
		even


		CNOP	0,4

Init		dc.l	LIB_SIZE
		dc.l	FuncTable
		dc.l	DataTable
		dc.l	InitRoutine


FuncTable	dc.l	OpenLib		; -6
		dc.l	CloseLib	; -12
		dc.l	Expunge		; -18
		dc.l	Reserved	; -24

		dc.l	Sfx_SetVol	; -30
		dc.l	Sfx_Start	; -36
		dc.l	Sfx_Update	; -42
		dc.l	Sfx_Stop	; -48
		dc.l	Sfx_Done	; -54

		dc.l	Mus_SetVol	; -60
		dc.l	Mus_Register	; -66
		dc.l	Mus_Unregister	; -72
		dc.l	Mus_Play	; -78
		dc.l	Mus_Stop	; -84
		dc.l	Mus_Pause	; -90
		dc.l	Mus_Resume	; -96
		dc.l	Mus_Done	; -102

		dc.l	-1


DataTable	INITBYTE LN_TYPE,NT_LIBRARY
		INITLONG LN_NAME,Name
		INITBYTE LIB_FLAGS,LIBF_SUMUSED+LIBF_CHANGED
		INITWORD LIB_VERSION,VERSION
		INITWORD LIB_REVISION,REVISION
		INITLONG LIB_IDSTRING,IDString
		dc.l	0


;--------------------------------------
; A0 = library segment list
; A6 = ExecBase
; D0 = library pointer
;
; D0 -> library base or NULL

		CNOP	0,16

InitRoutine	move.l	a6,_ExecBase
		move.l	a5,-(a7)		;save a5
		movea.l	d0,a5			;library base pointer
		move.l	a0,SegList		;save library seglist

		bsr	InitServer		;setup sound server
		bne.b	.err

		move.l	a5,d0			;success ...
		movea.l	(a7)+,a5
		rts

.err		moveq	#0,d0			;error
		movea.l	(a7)+,a5
		rts


;--------------------------------------

		CNOP	0,16

OpenLib		addq.w	#1,LIB_OPENCNT(a6)
		move.l	a6,d0
		rts


;--------------------------------------
; this library is auto-expunging

		CNOP	0,16

CloseLib	subq.w	#1,LIB_OPENCNT(a6)
		beq.b	Expunge
;still open
		moveq	#0,d0			;no seglist
		rts


;--------------------------------------
; A6 = library base
;
; D0 -> seglist or NULL

		CNOP	0,16

Expunge		tst.w	LIB_OPENCNT(a6)		;library still open?
		bne	.LibStillOpen		;yes

		bsr	ExitServer		;cleanup sound server

		IFND	TESTING
		move.l	SegList,d0		;get the seglist
		movem.l	d0/a5-a6,-(a7)		;delayed save
		movea.l	a6,a5			;library base => A5
		movea.l a5,a1			;library node
		movea.l	_ExecBase,a6
		CALLSYS	Remove			;unlink msmmu.library

		movea.l	a5,a1			;calculate lib size
		moveq	#0,d0			;and free memory
		move.w	LIB_NEGSIZE(a5),d0	;backward extent
		suba.w	d0,a1			;low memory extent
		add.w	LIB_POSSIZE(a5),d0	;forward extent
		CALLSYS	FreeMem			;give it back
		movem.l	(a7)+,d0/a5-a6		;seglist=>D0
		ENDC
		rts


.LibStillOpen	moveq	#0,d0			;no seglist
		rts


;--------------------------------------

		CNOP	0,4

Reserved	moveq.l #0,d0
		rts


EndCode:

;--------------------------------------------------------------------

		dc.b	'This sound library uses the Amiga audio.device and',0
		dc.b	'11KHz samples; it supports full stereo panning on',0
		dc.b	'sound effects and music.  Up to 16 sound effects and',0
		dc.b	'16 channels of music can be palying at any time.',0

;--------------------------------------------------------------------

		CNOP	0,16

InitServer	movem.l	d1-d7/a0-a6,-(a7)
		movea.l	_ExecBase,a6

		moveq	#0,d0
		lea	DOSName,a1
		CALLSYS	OpenLibrary
		move.l	d0,_DOSBase
		beq	.err			; couldn't open dos.library

		moveq	#39,d0
		lea	GfxName,a1
		CALLSYS	OpenLibrary
		tst.l	d0
		beq.b	.2			; leave at default (NTSC)
		move.l	d0,a1
		btst	#REALLY_PALn,gb_DisplayFlags(a1)
		beq.b	.1			; crystal is NTSC
		move.w	#322,period		; PAL sample rate
.1		CALLSYS	CloseLibrary

.2		move.l	#128*256,d0
		move.l	#MEMF_CLEAR+MEMF_FAST+MEMF_PUBLIC,d1
		CALLSYS	AllocVec
		move.l	d0,LookupMem
		beq	.err0			; no memory for vol_lookup
		move.l	d0,vol_lookup
		movea.l	d0,a0

		moveq	#0,d2
		moveq	#0,d1
.3		move.b	d1,d3
		ext.w	d3
		muls.w	d2,d3
		divs.w	#127,d3			; i*j/127
		move.b	d3,(a0)+
		addq.b	#1,d1
		bne.b	.3
		addq.b	#1,d2
		cmpi.b	#128,d2
		bne.b	.3

		CALLSYS	CreateMsgPort
		move.l	d0,AudioPort
		beq	.err1			; couldn't create port
		movea.l	d0,a0
		move.l	#ioa_SIZEOF,d0
		CALLSYS	CreateIORequest
		move.l	d0,AudioIO
		beq	.err2			; couldn't create io

		movea.l	d0,a1
		moveq	#0,d0
		moveq	#0,d1
		lea	AudioName,a0
		CALLSYS	OpenDevice
		tst.l	d0
		bne	.err3			; couldn't open audio.device

		movea.l	AudioIO,a1
		move.w	#ADCMD_ALLOCATE,IO_COMMAND(a1)
		move.b	#ADIOF_NOWAIT+IOF_QUICK,IO_FLAGS(a1)
		move.l	#AudioAlloc,ioa_Data(a1)
		move.l	#1,ioa_Length(a1)
		movea.l	IO_DEVICE(a1),a6
		jsr	DEV_BEGINIO(a6)
		movea.l	_ExecBase,a6
		tst.l	d0
		bne	.err4			; couldn't allocate channels

		movea.l	AudioIO,a1
		move.w	#ADCMD_LOCK,IO_COMMAND(a1)
		CALLSYS	SendIO

		lea	_custom,a0
		move.w	#$0780,intena(a0)	; kill int enable
		move.w	#$0780,intreq(a0)	; kill request
		move.w	#$00FF,adkcon(a0)	; kill modulation
		move.w	#$000F,dmacon(a0)	; disable dma

		move.l	#ClearBuf,aud0(a0)
		move.w	#40,aud0+ac_len(a0)
		move.w	period,aud0+ac_per(a0)
		move.w	#0,aud0+ac_vol(a0)

		move.l	#ClearBuf,aud1(a0)
		move.w	#40,aud1+ac_len(a0)
		move.w	period,aud1+ac_per(a0)
		move.w	#0,aud1+ac_vol(a0)

		move.l	#ClearBuf,aud2(a0)
		move.w	#40,aud2+ac_len(a0)
		move.w	period,aud2+ac_per(a0)
		move.w	#0,aud2+ac_vol(a0)

		move.l	#ClearBuf,aud3(a0)
		move.w	#40,aud3+ac_len(a0)
		move.w	period,aud3+ac_per(a0)
		move.w	#0,aud3+ac_vol(a0)

		moveq	#INTB_AUD0,d0
		lea	AInt0,a1
		CALLSYS	SetIntVector
		move.l	d0,OldAInt0

		move.l	#QuietInst,Channel0
		move.l	#QuietInst,Channel1
		move.l	#QuietInst,Channel2
		move.l	#QuietInst,Channel3
		move.l	#QuietInst,Channel4
		move.l	#QuietInst,Channel5
		move.l	#QuietInst,Channel6
		move.l	#QuietInst,Channel7
		move.l	#QuietInst,Channel8
		move.l	#QuietInst,Channel9
		move.l	#QuietInst,Channel10
		move.l	#QuietInst,Channel11
		move.l	#QuietInst,Channel12
		move.l	#QuietInst,Channel13
		move.l	#QuietInst,Channel14
		move.l	#QuietInst,Channel15

		clr.b	mus_playing
		clr.b	mus_looping

		clr.l	MusDelay		; MusDelay = 0
		clr.l	VoiceAvail		; all voices available

		CALLSYS	Disable
		lea	_custom,a0
		move.w	#$8080,intena(a0)	; int enable
		move.w	#$800F,dmacon(a0)	; enable dma
		bsr	AudioINT0		; start audio
		CALLSYS	Enable

		movem.l	(a7)+,d1-d7/a0-a6
		moveq	#0,d0		; okay
		rts

.err4		movea.l	AudioIO,a1
		CALLSYS	CloseDevice

.err3		movea.l	AudioIO,a0
		CALLSYS	DeleteIORequest

.err2		movea.l	AudioPort,a0
		CALLSYS	DeleteMsgPort

.err1		movea.l	LookupMem,a1
		CALLSYS	FreeVec

.err0		movea.l	_DOSBase,a1
		CALLSYS	CloseLibrary

.err		movem.l	(a7)+,d1-d7/a0-a6
		moveq	#-1,d0		; error
		rts

;--------------------------------------

		CNOP	0,16

ExitServer	movem.l	d1-d7/a0-a6,-(a7)

		lea	_custom,a0
		move.w	#$0780,intena(a0)	; kill int enable
		move.w	#$0780,intreq(a0)	; kill request
		move.w	#$000F,dmacon(a0)	; dma off

		moveq	#INTB_AUD0,d0
		movea.l	OldAInt0,a1
		movea.l	_ExecBase,a6
		CALLSYS	SetIntVector

		movea.l	AudioIO,a1
		move.w	#ADCMD_FREE,IO_COMMAND(a1)
		move.b	#IOF_QUICK,IO_FLAGS(a1)
		movea.l	IO_DEVICE(a1),a6
		jsr	DEV_BEGINIO(a6)

		movea.l	_ExecBase,a6

		movea.l	AudioIO,a1
		CALLSYS	CloseDevice

		movea.l	AudioIO,a0
		CALLSYS	DeleteIORequest

		movea.l	AudioPort,a0
		CALLSYS	DeleteMsgPort

		movea.l	LookupMem,a1
		CALLSYS	FreeVec

		movea.l	_DOSBase,a1
		CALLSYS	CloseLibrary

		movem.l	(a7)+,d1-d7/a0-a6
		moveq	#0,d0
		rts

;--------------------------------------

		CNOP	0,16

Sfx_SetVol	move.w	d0,sfx_volume
		rts

;--------------------------------------

		CNOP	0,16

Sfx_Start	movem.l	d0-d4/a0-a2,-(a7)
		lea	sfxVoiceTbl,a1
		movea.l	(a1,d0.w*4),a1
		lea	8(a0),a0
		subq.l	#8,d4
		move.l	a0,vc_Wave(a1)
		clr.l	vc_Index(a1)
		swap	d1
		clr.w	d1
		divu.l	#11025,d1
		move.l	d1,vc_Step(a1)
		clr.l	vc_Loop(a1)
		lsl.l	#8,d4
		move.l	d4,vc_Length(a1)

		addq.w	#1,d3		; sep += 1
		move.w	d2,d4
		muls.w	d3,d4
		muls.w	d3,d4
		clr.w	d4
		swap	d4
		neg.w	d4
		add.w	d2,d4		; ltvol = vol - vol * (sep+1)^2 / 256^2
		lsl.l	#8,d4
		move.l	d4,vc_LtVol(a1)

		subi.l	#257,d3		; sep -= 257
		move.w	d2,d4
		muls.w	d3,d4
		muls.w	d3,d4
		clr.w	d4
		swap	d4
		neg.w	d4
		add.w	d2,d4		; rtvol = vol - vol * (sep-257)^2 / 256^2
		lsl.l	#8,d4
		move.l	d4,vc_RtVol(a1)

		move.b	#$81,vc_Flags(a1)
		movem.l	(a7)+,d0-d4/a0-a2
		rts

;--------------------------------------

		CNOP	0,16

Sfx_Update	movem.l	d0-d4/a0-a2,-(a7)
		lea	sfxVoiceTbl,a1
		movea.l	(a1,d0.w*4),a1
		swap	d1
		clr.w	d1
		divu.l	#11025,d1
		move.l	d1,vc_Step(a1)

		addq.w	#1,d3		; sep += 1
		move.w	d2,d4
		muls.w	d3,d4
		muls.w	d3,d4
		clr.w	d4
		swap	d4
		neg.w	d4
		add.w	d2,d4		; ltvol = vol - vol * (sep+1)^2 / 256^2
		lsl.l	#8,d4
		move.l	d4,vc_LtVol(a1)

		subi.l	#257,d3		; sep -= 257
		move.w	d2,d4
		muls.w	d3,d4
		muls.w	d3,d4
		clr.w	d4
		swap	d4
		neg.w	d4
		add.w	d2,d4		; rtvol = vol - vol * (sep-257)^2 / 256^2
		lsl.l	#8,d4
		move.l	d4,vc_RtVol(a1)

		movem.l	(a7)+,d0-d4/a0-a2
		rts

;--------------------------------------

		CNOP	0,16

Sfx_Stop	move.l	a0,-(a7)
		lea	sfxVoiceTbl,a0
		movea.l	(a0,d0.w*4),a0
		bclr	#0,vc_Flags(a0)
		movea.l	(a7)+,a0
		rts

;--------------------------------------

		CNOP	0,16

Sfx_Done	move.l	a0,-(a7)
		lea	sfxVoiceTbl,a0
		movea.l	(a0,d0.w*4),a0
		moveq	#0,d0
		btst	#0,vc_Flags(a0)
		beq.b	.1		; not playing
		move.l	vc_Index(a0),d0
		addq.l	#1,d0		; for safety
.1		movea.l	(a7)+,a0
		rts

;--------------------------------------

		CNOP	0,16

Mus_SetVol	move.w	d0,mus_volume
		rts

;--------------------------------------

		CNOP	0,16

Mus_Register	movem.l	d1-d7/a0-a6,-(sp)
		bsr	Mus_Unregister

		move.l	a0,MUSMemPtr

		cmpi.l	#$4D55531A,(a0)		; "MUS",26
		bne	.err0			; not a mus file

		move.l	MUSMemPtr,MusPtr

		bsr	TestMUSFile
		beq	.err0

		move.l	InstrFile,d1
		move.l	#MODE_OLDFILE,d2
		CALLDOS	Open
		move.l	d0,InstrHandle
		beq	.err0

		move.l	#MEMF_CLEAR,d0		; any memory
		move.l	#65536,d1		; puddle size
		move.l	#32768,d2		; threshold size
		bsr	CreatePool
		move.l	a0,InstrPool
		beq	.err1

		movea.l	a0,a2
		movea.l	MusPtr,a3

		move.w	#255,d0
		lea	Instruments,a0
.setinstr	move.l	#QuietInst,(a0)+
		dbra	d0,.setinstr

		move.w	$C(a3),d4		; instrCnt
		ror.w	#8,d4
		subq.w	#1,d4			; for dbra

		lea	$10(a3),a3		; instruments[]

.instrloop	moveq	#14,d0
		movea.l	a2,a0
		bsr	AllocPooled

		moveq	#0,d2
		move.b	(a3)+,d2		; instrument #
		moveq	#0,d1
		move.b	(a3)+,d1		; offset to next instr. #
		adda.l	d1,a3			; skip it (whatever it is?)

		lea	Instruments,a0
		move.l	d0,(a0,d2.w*4)
		beq	.err2

		movea.l	d0,a4			; instrument record

		bftst	(validInstr){d2:1}
		beq	.next			; no instrument

		move.l	InstrHandle,d1
		lsl.l	#2,d2
		moveq	#OFFSET_BEGINNING,d3
		CALLDOS	Seek

		move.l	InstrHandle,d1
		move.l	a4,d2
		moveq	#4,d3
		CALLSYS	Read			; get instrument offset
		addq.l	#1,d0
		beq	.err2			; can't read file

		move.l	InstrHandle,d1
		move.l	(a4),d2
		moveq	#OFFSET_BEGINNING,d3
		CALLSYS	Seek

		move.l	InstrHandle,d1
		move.l	a4,d2
		moveq	#14,d3
		CALLSYS	Read			; get instrument header
		addq.l	#1,d0
		beq	.err2			; can't read file

		move.l	in_Length(a4),d0
		swap	d0
		movea.l	a2,a0
		bsr	AllocPooled
		move.l	d0,in_Wave(a4)		; wave data buffer
		beq	.err2

		move.l	InstrHandle,d1
		move.l	d0,d2
		move.l	in_Length(a4),d3
		swap	d3
		CALLDOS	Read			; get instrument samples
		addq.l	#1,d0
		beq	.err2			; can't read file

		move.b	#1,in_Flags(a4)
.next		dbra	d4,.instrloop

		move.l	InstrHandle,d1
		CALLDOS	Close
		clr.l	InstrHandle

		moveq	#1,d0			; return handle=1
		movem.l	(sp)+,d1-d7/a0-a6
		rts

.err2		movea.l	InstrPool,a0
		bsr	DeletePool
		clr.l	InstrPool

.err1		move.l	InstrHandle,d1
		CALLDOS	Close
		clr.l	InstrHandle

.err0		moveq	#0,d0			; return handle=0
		movem.l	(sp)+,d1-d7/a0-a6
		rts

;--------------------------------------

		CNOP	0,16

Mus_Unregister	movem.l	d0-d7/a0-a6,-(a7)
		bsr	Mus_Stop

		clr.l	MUSMemPtr
		clr.l	MUSMemSize

		tst.l	InstrPool
		beq.b	.1
		movea.l	InstrPool,a0
		bsr	DeletePool
		clr.l	InstrPool

.1		movem.l	(a7)+,d0-d7/a0-a6
		rts

;--------------------------------------

		CNOP	0,16

Mus_Play	move.b	d1,mus_looping
		move.b	#2,mus_playing		; 2 = play from start
		rts

;--------------------------------------

		CNOP	0,16

Mus_Stop	tst.b	mus_playing
		beq.b	.2
		st	mus_playing		; -1 = stop playing
.1		tst.b	mus_playing
		bne.b	.1

.2		move.l	#QuietInst,Channel0
		move.l	#QuietInst,Channel1
		move.l	#QuietInst,Channel2
		move.l	#QuietInst,Channel3
		move.l	#QuietInst,Channel4
		move.l	#QuietInst,Channel5
		move.l	#QuietInst,Channel6
		move.l	#QuietInst,Channel7
		move.l	#QuietInst,Channel8
		move.l	#QuietInst,Channel9
		move.l	#QuietInst,Channel10
		move.l	#QuietInst,Channel11
		move.l	#QuietInst,Channel12
		move.l	#QuietInst,Channel13
		move.l	#QuietInst,Channel14
		move.l	#QuietInst,Channel15

		clr.b	Voice0+vc_Flags		; disable voices
		clr.b	Voice1+vc_Flags
		clr.b	Voice2+vc_Flags
		clr.b	Voice3+vc_Flags
		clr.b	Voice4+vc_Flags
		clr.b	Voice5+vc_Flags
		clr.b	Voice6+vc_Flags
		clr.b	Voice7+vc_Flags
		clr.b	Voice8+vc_Flags
		clr.b	Voice9+vc_Flags
		clr.b	Voice10+vc_Flags
		clr.b	Voice11+vc_Flags
		clr.b	Voice12+vc_Flags
		clr.b	Voice13+vc_Flags
		clr.b	Voice14+vc_Flags
		clr.b	Voice15+vc_Flags

		clr.b	mus_looping
		clr.l	MusDelay		; MusDelay = 0
		clr.l	VoiceAvail		; all voices available
		rts

;--------------------------------------

		CNOP	0,16

Mus_Pause	clr.b	mus_playing		; 0 = not playing
		rts

;--------------------------------------

		CNOP	0,16

Mus_Resume	move.b	#1,mus_playing		; 1 = play
		rts

;--------------------------------------

		CNOP	0,16

Mus_Done	moveq	#0,d0
		move.b	mus_playing,d0
		rts

;------------------------------------------------------------------------

		CNOP	0,16

TestMUSFile	movea.l	MusPtr,a0
		move.l	MUSMemSize,d3		; d3 = total file size
		moveq	#0,d0
		move.w	4(a0),d0
		beq	.fail
		ror.w	#8,d0			; score length
		moveq	#0,d1
		move.w	6(a0),d1
		ror.w	#8,d1			; score start
		cmpi.w	#18,d1			; start < 18? (1 instr.)
		blt	.fail
		add.l	d1,d0			; d0 = total size

		move.l	d0,d3
		move.l	d3,MUSMemSize

		move.w	12(a0),d2
		beq.b	.fail
		ror.w	#8,d2			; d2 = instr. count
		subq.w	#1,d2
		lea	16(a0),a1		; a1 = * instr. list
.loop		addq.l	#1,a1			; skip instr. value
		moveq	#0,d0
		move.b	(a1)+,d0		; d0 = offset to next instr.
		adda.l	d0,a1			; skip info (?)
		dbra	d2,.loop		; next
		move.l	a1,d0			; d0 = * data following list
		sub.l	a0,d0			; - file start
		cmp.l	d0,d1			; = start?
		bne.b	.fail
		move.b	-1(a0,d3.l),d0		; get last byte
		lsr.b	#4,d0
		cmpi.b	#6,d0			; last byte = $6x? (end)
		bne.b	.fail
		moveq	#1,d0			; file okay
		rts

.fail		moveq	#0,d0			; yikes!
		rts

;------------------------------------------------------------------------

		cnop	0,16

AudioINT0	movem.l	d2-d7/a2-a6,-(a7)
		move.l	chip_offset,d0
		add.l	d0,chip_buffer		; switch buffers
		neg.l	d0
		move.l	d0,chip_offset

		lea	_custom,a0
		move.w	#$0780,intreq(a0)	; clear int requests

		tst.b	mus_playing
		bgt.b	.domus
		beq.b	.musoff
		addq.b	#1,mus_playing		; now off

.musoff		move.l	#ClearBuf,aud3(a0)
		move.w	#40,aud3+ac_len(a0)
		move.w	period,aud3+ac_per(a0)
		move.w	#0,aud3+ac_vol(a0)

		move.l	#ClearBuf,aud2(a0)
		move.w	#40,aud2+ac_len(a0)
		move.w	period,aud2+ac_per(a0)
		move.w	#0,aud2+ac_vol(a0)

		bra	.dosfx

.domus		cmpi.b	#2,mus_playing
		bne.b	.musnxt
		subq.b	#1,mus_playing

		movea.l	MusPtr,a1
		moveq	#0,d1
		move.w	6(a1),d1		; score start
		ror.w	#8,d1
		adda.l	d1,a1			; a1 = start
		move.l	a1,MusIndex		; store index

.musnxt		subq.l	#1,MusDelay
		bpl.b	.fillmus

		bsr	NextEvent

.fillmus	lea	Voice0,a0		; music voices
		movea.l	chip_buffer,a2
		bsr	FillBuffer

		lea	_custom,a0

		move.l	chip_buffer,d0
		move.l	d0,aud3(a0)		; left mus
		move.w	#40,aud3+ac_len(a0)	; 80 samples
		move.w	period,aud3+ac_per(a0)
		move.w	mus_volume,aud3+ac_vol(a0)

		addi.l	#128,d0
		move.l	d0,aud2(a0)		; right mus
		move.w	#40,aud2+ac_len(a0)	; 80 samples
		move.w	period,aud2+ac_per(a0)
		move.w	mus_volume,aud2+ac_vol(a0)

.dosfx		lea	Voice16,a0		; sound effect voices
		movea.l	chip_buffer,a2
		lea	256(a2),a2
		bsr	FillBuffer

		lea	_custom,a0

		move.l	chip_buffer,d0
		addi.l	#256,d0
		move.l	d0,aud0(a0)		; left sfx
		move.w	#40,aud0+ac_len(a0)	; 80 samples
		move.w	period,aud0+ac_per(a0)
		move.w	sfx_volume,aud0+ac_vol(a0)

		addi.l	#128,d0
		move.l	d0,aud1(a0)		; right sfx
		move.w	#40,aud1+ac_len(a0)	; 80 samples
		move.w	period,aud1+ac_per(a0)
		move.w	sfx_volume,aud1+ac_vol(a0)

.exit		movem.l	(a7)+,d2-d7/a2-a6
		moveq	#0,d0
		rts

;------------------------------------------------------------------------

		CNOP	0,16

FillBuffer	movea.l	a2,a4
		lea	tempAudio,a1
		moveq	#4,d0
.lcloop		clr.l	(a1)+
		clr.l	(a1)+
		clr.l	(a1)+
		clr.l	(a1)+
		dbra	d0,.lcloop

		lea	tempAudio+128,a1
		moveq	#4,d0
.rcloop		clr.l	(a1)+
		clr.l	(a1)+
		clr.l	(a1)+
		clr.l	(a1)+
		dbra	d0,.rcloop
		bra.b	.1stvoice

.next		move.l	vc_Next(a0),d0		; next voice
		bne.b	.chkvoice

		lea	128(a4),a2
		lea	tempAudio,a1
		moveq	#4,d0
.lmloop		move.l	(a1)+,(a4)+
		move.l	(a1)+,(a4)+
		move.l	(a1)+,(a4)+
		move.l	(a1)+,(a4)+
		dbra	d0,.lmloop

		lea	tempAudio+128,a1
		moveq	#4,d0
.rmloop		move.l	(a1)+,(a2)+
		move.l	(a1)+,(a2)+
		move.l	(a1)+,(a2)+
		move.l	(a1)+,(a2)+
		dbra	d0,.rmloop
		rts

.chkvoice	movea.l	d0,a0
.1stvoice	btst	#0,vc_Flags(a0)
		beq.b	.next			; not enabled

;------------------
; do voice
		btst	#7,vc_Flags(a0)
		bne	.5			; sfx

		btst	#1,vc_Flags(a0)
		beq.b	.1			; not releasing

		tst.l	vc_LtVol(a0)
		beq.b	.0
		subi.l	#256,vc_LtVol(a0)

.0		tst.l	vc_RtVol(a0)
		beq.b	.1
		subi.l	#256,vc_RtVol(a0)

.1		move.l	vc_LtVol(a0),d6
		move.l	vc_RtVol(a0),d7
		tst.l	d6
		bne.b	.2			; not off yet
		tst.l	d7
		bne.b	.2			; not off yet

		clr.b	vc_Flags(a0)		; voice off
		bra	.next

.2		lea	tempAudio,a1		; left buffer
		movea.l	vol_lookup,a5

		movem.l	vc_Index(a0),d1-d4	; index,step,loop,length

		movea.l	vc_Channel(a0),a3
		move.l	ch_Pitch(a3),d0
		muls.l	d0,d5:d2
		move.w	d5,d2
		swap	d2
		add.l	vc_Step(a0),d2		; final sample rate

		movea.l	vc_Wave(a0),a3		; sample data

		moveq	#79,d5			; 80 samples
.floop		move.l	d1,d0
		swap	d0
		move.b	(a3,d0.w),d6		; sample
		move.b	d6,d7
		move.b	(a5,d6.l),d6		; convert
		move.b	(a5,d7.l),d7		; convert
		asr.b	#2,d6
		asr.b	#2,d7
		add.b	d6,(a1)+		; left sample
		add.b	d7,127(a1)		; right sample

		add.l	d2,d1
		cmp.l	d4,d1
		blo.b	.3
		sub.l	d4,d1
		add.l	d3,d1
		tst.l	d3
		beq.b	.4			; no looping
.3		dbra	d5,.floop
		bra.b	.done			; done with voice

; ran out of data
.4		clr.b	vc_Flags(a0)		; voice off

.done		move.l	d1,vc_Index(a0)
		bra	.next

; do sfx voice

.5		move.l	vc_LtVol(a0),d6
		move.l	vc_RtVol(a0),d7

		lea	tempAudio,a1		; left buffer
		movea.l	vol_lookup,a5

		movem.l	vc_Index(a0),d1-d4	; index,step,loop,length

		move.l	vc_Step(a0),d2		; sample rate
		lsr.l	#8,d2

		movea.l	vc_Wave(a0),a3		; sample data

		moveq	#79,d5			; 80 samples
.sfloop		move.l	d1,d0
		lsr.l	#8,d0
		move.b	(a3,d0.l),d6		; sample
		move.b	d6,d7
		move.b	(a5,d6.l),d6		; convert
		move.b	(a5,d7.l),d7		; convert
		asr.b	#2,d6
		asr.b	#2,d7
		add.b	d6,(a1)+		; left sample
		add.b	d7,127(a1)		; right sample

		add.l	d2,d1
		cmp.l	d4,d1
		bhs.b	.4			; ran out of data
		dbra	d5,.sfloop
		bra	.done			; done with voice

;------------------------------------------------------------------------

		CNOP	0,16

NextEvent	movea.l	MusIndex,a1

.0		move.b	(a1)+,d0		; get next event
		move.b	d0,d1
		lsr.b	#3,d1
		andi.w	#$E,d1			; d1 = event type * 2
		lea	EventTable,a0
		move.w	(a0,d1.w),d1
		jsr	(a0,d1.w)		; do event
		tst.b	d0
		bpl.b	.0			; more events

		moveq	#0,d1			; time = 0
.1		move.b	(a1)+,d0		; get byte
		bpl.b	.2

		andi.w	#$7F,d0			; kill sign bit
		or.b	d0,d1			; time = time + 7 bits
		lsl.l	#7,d1			; * 128
		bra.b	.1			; get next 7 bits

.2		or.b	d0,d1			; time = time + last 7 bits
		subq.l	#1,d1			; delay = time - 1
		bmi.b	.0			; (no delay)

		move.l	d1,MusDelay		; store delay
		move.l	a1,MusIndex		; store index
		rts

;------------------------------------------------------------------------

Release		moveq	#15,d1
		and.b	d0,d1			; d1 = channel

		lea	Channels,a0
		movea.l	(a0,d1.w*4),a0		; channel record
		movea.l	ch_Map(a0),a0		; channel map

		move.b	(a1)+,d1		; note #
		moveq	#0,d2
		move.b	(a0,d1.w),d2		; voice #
		beq.b	.exit			; no mapping

		clr.b	(a0,d1.w)		; clear mapping
		move.l	VoiceAvail,d3
		bclr	d2,d3			; voice free for use
		move.l	d3,VoiceAvail

		lea	Voices,a0
		movea.l	(a0,d2.w*4),a0		; voice
		bset	#1,vc_Flags(a0)		; do release

.exit		rts

;------------------------------------------------------------------------

PlayNote	moveq	#15,d1
		and.b	d0,d1			; d1 = channel

		lea	Channels,a0
		movea.l	(a0,d1.w*4),a2		; channel record
		movea.l	ch_Map(a2),a0		; channel map

		moveq	#-1,d2			; no volume change
		move.b	(a1)+,d1		; note #
		bclr	#7,d1
		beq.b	.getvc			; no volume

		moveq	#0,d2
		move.b	(a1)+,d2		; volume

.getvc		moveq	#0,d3
		move.l	VoiceAvail,d4
.vloop		bset	d3,d4
		beq.b	.foundfree
		addq.b	#1,d3
		cmpi.b	#16,d3
		bne.b	.vloop
; no free voices
		rts


.foundfree	move.b	d3,(a0,d1.w)		; voice mapping
		move.l	d4,VoiceAvail

		lea	Voices,a0
		movea.l	(a0,d3.w*4),a3		; voice

		tst.b	d2
		bmi.b	.skip

; new channel volume

		move.b	d2,ch_Vol(a2)
		moveq	#0,d3
		move.b	ch_Pan(a2),d3
		addq.w	#1,d3		; sep += 1
		move.w	d2,d4
		muls.w	d3,d4
		muls.w	d3,d4
		clr.w	d4
		swap	d4
		neg.w	d4
		add.w	d2,d4		; ltvol = vol - vol * (sep+1)^2 / 256^2
		lsl.l	#8,d4
		move.l	d4,ch_LtVol(a2)

		subi.l	#257,d3		; sep -= 257
		move.w	d2,d4
		muls.w	d3,d4
		muls.w	d3,d4
		clr.w	d4
		swap	d4
		neg.w	d4
		add.w	d2,d4		; rtvol = vol - vol * (sep-257)^2 / 256^2
		lsl.l	#8,d4
		move.l	d4,ch_RtVol(a2)

.skip		move.l	ch_LtVol(a2),vc_LtVol(a3)
		move.l	ch_RtVol(a2),vc_RtVol(a3)

		moveq	#15,d2
		and.b	d0,d2
		cmpi.b	#15,d2
		beq.b	.percussion

		move.l	ch_Instr(a2),a4		; instrument record

		lea	NoteTable,a0
		moveq	#72,d2			; one octave above middle c
		sub.b	in_Base(a4),d2
		add.b	d1,d2
		move.l	(a0,d2.w*4),vc_Step(a3)	; step value for note

		clr.l	vc_Index(a3)

		move.l	a2,vc_Channel(a3)	; back link (for pitch wheel)

		move.l	in_Wave(a4),vc_Wave(a3)
		move.l	in_Loop(a4),vc_Loop(a3)
		move.l	in_Length(a4),vc_Length(a3)
		move.b	in_Flags(a4),vc_Flags(a3)
.exit		rts

; for the percussion channel, the note played sets the percussion instrument

.percussion	move.l	#65536,vc_Step(a3)	; sample rate always 1.0

		clr.l	vc_Index(a3)

		move.l	a2,vc_Channel(a3)	; back link

		addi.b	#100,d1			; percussion instruments

		lea	Instruments,a0
		move.l	(a0,d1.w*4),a0		; instrument record
		move.l	in_Wave(a0),vc_Wave(a3)
		move.l	in_Loop(a0),vc_Loop(a3)
		move.l	in_Length(a0),vc_Length(a3)
		move.b	in_Flags(a0),vc_Flags(a3)
		rts

;------------------------------------------------------------------------

Pitch		moveq	#15,d1
		and.b	d0,d1			; d1 = channel

		lea	Channels,a0
		movea.l	(a0,d1.w*4),a2		; channel record

		moveq	#0,d1
		move.b	(a1)+,d1		; pitch wheel setting
		lea	PitchTable,a0
		move.l	(a0,d1.w*4),ch_Pitch(a2)
		rts

;------------------------------------------------------------------------

Tempo		addq.l	#1,a1			; skip value
		rts

;------------------------------------------------------------------------

ChangeCtrl	moveq	#15,d1
		and.b	d0,d1			; d1 = channel

		lea	Channels,a0
		movea.l	(a0,d1.w*4),a2		; channel

		move.b	(a1)+,d1		; get controller

		moveq	#0,d2
		move.b	(a1)+,d2		; value

		tst.b	d1
		bne.b	.1

; set channel instrument

		lea	Instruments,a0
		move.l	(a0,d2.w*4),ch_Instr(a2)
		bne.b	.0
		move.l	#QuietInst,ch_Instr(a2)
.0		rts

.1		cmpi.b	#3,d1			; volume?
		bne.b	.2

; set channel volume

		move.b	d2,ch_Vol(a2)
		moveq	#0,d3
		move.b	ch_Pan(a2),d3
		addq.w	#1,d3		; sep += 1
		move.w	d2,d4
		muls.w	d3,d4
		muls.w	d3,d4
		clr.w	d4
		swap	d4
		neg.w	d4
		add.w	d2,d4		; ltvol = vol - vol * (sep+1)^2 / 256^2
		lsl.l	#8,d4
		move.l	d4,ch_LtVol(a2)

		subi.w	#257,d3		; sep -= 257
		move.w	d2,d4
		muls.w	d3,d4
		muls.w	d3,d4
		clr.w	d4
		swap	d4
		neg.w	d4
		add.w	d2,d4		; rtvol = vol - vol * (sep-257)^2 / 256^2
		lsl.l	#8,d4
		move.l	d4,ch_RtVol(a2)
		rts

.2		cmpi.b	#4,d1			; pan?
		bne.b	.exit

; set channel pan

		add.b	d2,d2		; pan -> sep
		move.b	d2,ch_Pan(a2)
		move.b	ch_Vol(a2),d2
		moveq	#0,d3
		move.b	ch_Pan(a2),d3
		addq.w	#1,d3		; sep += 1
		move.w	d2,d4
		muls.w	d3,d4
		muls.w	d3,d4
		clr.w	d4
		swap	d4
		neg.w	d4
		add.w	d2,d4		; ltvol = vol - vol * (sep+1)^2 / 256^2
		lsl.l	#8,d4
		move.l	d4,ch_LtVol(a2)

		subi.w	#257,d3		; sep -= 257
		move.w	d2,d4
		muls.w	d3,d4
		muls.w	d3,d4
		clr.w	d4
		swap	d4
		neg.w	d4
		add.w	d2,d4		; rtvol = vol - vol * (sep-257)^2 / 256^2
		lsl.l	#8,d4
		move.l	d4,ch_RtVol(a2)
.exit		rts

;------------------------------------------------------------------------

NoEvent		rts

;------------------------------------------------------------------------

EndScore	tst.b	mus_looping		; loop?
		bne	.loop

		move.l	#QuietInst,Channel0
		move.l	#QuietInst,Channel1
		move.l	#QuietInst,Channel2
		move.l	#QuietInst,Channel3
		move.l	#QuietInst,Channel4
		move.l	#QuietInst,Channel5
		move.l	#QuietInst,Channel6
		move.l	#QuietInst,Channel7
		move.l	#QuietInst,Channel8
		move.l	#QuietInst,Channel9
		move.l	#QuietInst,Channel10
		move.l	#QuietInst,Channel11
		move.l	#QuietInst,Channel12
		move.l	#QuietInst,Channel13
		move.l	#QuietInst,Channel14
		move.l	#QuietInst,Channel15

		clr.b	Voice0+vc_Flags		; disable voices
		clr.b	Voice1+vc_Flags
		clr.b	Voice2+vc_Flags
		clr.b	Voice3+vc_Flags
		clr.b	Voice4+vc_Flags
		clr.b	Voice5+vc_Flags
		clr.b	Voice6+vc_Flags
		clr.b	Voice7+vc_Flags
		clr.b	Voice8+vc_Flags
		clr.b	Voice9+vc_Flags
		clr.b	Voice10+vc_Flags
		clr.b	Voice11+vc_Flags
		clr.b	Voice12+vc_Flags
		clr.b	Voice13+vc_Flags
		clr.b	Voice14+vc_Flags
		clr.b	Voice15+vc_Flags

		clr.l	MusDelay		; MusDelay = 0
		clr.l	VoiceAvail		; all voices available
		clr.b	mus_playing
		addq.l	#4,a7			; pop NextEvent
		rts

.loop		movea.l	MusPtr,a1
		moveq	#0,d1
		move.w	6(a1),d1		; score start
		ror.w	#8,d1
		adda.l	d1,a1			; a1 = start
		rts

;------------------------------------------------------------------------
		cnop	0,4

EventTable	dc.w	Release-EventTable
		dc.w	PlayNote-EventTable
		dc.w	Pitch-EventTable
		dc.w	Tempo-EventTable
		dc.w	ChangeCtrl-EventTable
		dc.w	NoEvent-EventTable
		dc.w	EndScore-EventTable
		dc.w	NoEvent-EventTable

;------------------------------------------------------------------------
;--------------------------------------------------------------------

		cnop	0,4

CreatePool	movea.l	_ExecBase,a1
		cmpi.w	#39,LIB_VERSION(a1)
		blt.b	.nopools		; change to bra for debugging

		move.l	a6,-(sp)
		movea.l	a1,a6
		CALLSYS	CreatePool
		movea.l	(sp)+,a6
		rts

.nopools	movem.l	d2-d7/a2-a6,-(sp)
		move.l	d0,d4			; memory attributes
		move.l	d1,d3			; amount to allocate when low
		move.l	d2,d5			; size of when not to use pool

		exg.l	d0,d1			; swap flags and size
		movea.l	a1,a6
		CALLSYS	AllocMem		; get first block
		movea.l	d0,a0
		tst.l	d0
		beq.b	.exit			; no memory!

		movem.l	d3-d5,(a0)		; puddleSize, Flags,Threshold
		clr.l	12(a0)			; no next block
		lea	24(a0),a1		; first free location here
		move.l	a1,16(a0)
		subi.l	#24,d3			; for header info
		move.l	d3,20(a0)		; amount free in this block

.exit		movem.l	(sp)+,d2-d7/a2-a6
		rts

		cnop	0,4

DeletePool	movea.l	_ExecBase,a1
		cmpi.w	#39,LIB_VERSION(a1)
		blt.b	.nopools		; change to bra for debugging

		move.l	a6,-(sp)
		movea.l	a1,a6
		CALLSYS	DeletePool
		movea.l	(sp)+,a6
		rts

.nopools	movem.l	d2-d7/a2-a6,-(sp)
		move.l	a0,d2			; first block
		beq.b	.exit			; safety check

		movea.l	a1,a6

.loop		movea.l	d2,a1			; pointer to block
		move.l	(a1),d0			; size of block
		move.l	12(a1),d2		; next block
		CALLSYS	FreeMem
		tst.l	d2
		bne.b	.loop

.exit		movem.l	(sp)+,d2-d7/a2-a6
		rts

		cnop	0,4

AllocPooled	movea.l	_ExecBase,a1
		cmpi.w	#39,LIB_VERSION(a1)
		blt.b	.nopools		; change to bra for debugging

		move.l	a6,-(sp)
		movea.l	a1,a6
		CALLSYS	AllocPooled
		movea.l	(sp)+,a6
		rts

.nopools	movem.l	d2-d7/a2-a6,-(sp)
		move.l	a0,d2
		beq.b	.exit			; safety check

		addq.l	#3,d0
		andi.b	#$FC,d0			; long align size

		movea.l	a1,a6

		cmp.l	8(a0),d0		; check threshold
		blt.b	.chkpuddles		; allocate from puddles

		addi.l	#24,d0			; for header
		move.l	d0,d3			; save size
		move.l	4(a0),d1		; mem attrs
		CALLSYS	AllocMem
		movea.l	d0,a0
		tst.l	d0
		beq.b	.exit			; no memory

		move.l	d3,(a0)			; size of block
		clr.l	20(a0)			; no free space in here

		movea.l	d2,a1			; pool header
		move.l	12(a1),d1
		move.l	a0,12(a1)		; splice in block
		move.l	d1,12(a0)		; relink next block
		lea	24(a0),a0		; skip over header

.exit		move.l	a0,d0
		movem.l	(sp)+,d2-d7/a2-a6
		rts

		cnop	0,4

.chkpuddles	cmp.l	20(a0),d0		; check free space
		blt.b	.gotspace

		movea.l	12(a0),a0		; next block
		move.l	a0,d1
		bne.b	.chkpuddles

; not enough free space in existing puddles, create another

		move.l	d0,d6			; save size

		movea.l	d2,a0			; pool header
		movem.l	(a0),d3-d5
		movem.l	(a0),d0-d1
		CALLSYS	AllocMem		; get block
		movea.l	d0,a0
		tst.l	d0
		beq.b	.out			; no memory!

		movea.l	d2,a1			; pool header
		movem.l	d3-d5,(a0)		; puddleSize, Flags,Threshold
		move.l	12(a1),12(a0)		; next block
		move.l	a0,12(a1)		; splice in block
		lea	24(a0),a1		; first free location here
		move.l	a1,16(a0)
		subi.l	#24,d3			; for header info
		move.l	d3,20(a0)		; amount free in this block

		move.l	d6,d0			; restore size

.gotspace	sub.l	d0,20(a0)		; sub from amount free
		bmi.b	.err			; threshold >= puddlesize!

		move.l	16(a0),a1		; free space
		add.l	d0,16(a0)		; next free space

		movea.l	a1,a0
		bra.b	.out

.err		add.l	d0,20(a0)		; restore free space
		moveq	#0,d0
		suba.l	a0,a0			; no memory

.out		move.l	a0,d0
		movem.l	(sp)+,d2-d7/a2-a6
		rts

;------------------------------------------------------------------------
;------------------------------------------------------------------------

		cnop	0,4

MUSMemPtr	dc.l	0
MUSMemSize	dc.l	0

SegList		dc.l	0
_ExecBase	dc.l	0
_DOSBase	dc.l	0
LookupMem	dc.l	0
vol_lookup	dc.l	0

;------------------------------------------------------------------------

period		dc.w	325	; NTSC 11025 KHz
sfx_volume	dc.w	64
mus_volume	dc.w	64
mus_playing	dc.b	0
mus_looping	dc.b	0

;------------------------------------------------------------------------

		cnop	0,4

AudioPort	dc.l	0
AudioIO		dc.l	0

AInt0		dc.l	0,0
		dc.b	NT_INTERRUPT,0		; LN_TYPE, LN_PRI
		dc.l	AIntName		; LN_NAME
		dc.l	0			; IS_DATA
		dc.l	AudioINT0		; IS_CODE

MusPtr		dc.l	0
MusIndex	dc.l	0
MusDelay	dc.l	0
OldAInt0	dc.l	0

AudioAlloc	dc.b	$0F			; Amiga channels to allocate

GfxName		dc.b	'graphics.library',0
DOSName		dc.b	'dos.library',0
AudioName	dc.b	'audio.device',0
AIntName	dc.b	'DOOM Sound Server',0

;--------------------------------------

		CNOP	0,4

; bit set if voice is in use (0-15=music voices,16-31=sfx voices)

VoiceAvail	dc.l	0

chip_buffer	dc.l	chipBuffer
chip_offset	dc.l	512

;--------------------------------------------------------------------

		CNOP	0,4

NoteTable	dc.l	65536/64,69433/64,73562/64,77936/64,82570/64,87480/64,92682/64,98193/64,104032/64,110218/64,116772/64,123715/64
		dc.l	65536/32,69433/32,73562/32,77936/32,82570/32,87480/32,92682/32,98193/32,104032/32,110218/32,116772/32,123715/32
		dc.l	65536/16,69433/16,73562/16,77936/16,82570/16,87480/16,92682/16,98193/16,104032/16,110218/16,116772/16,123715/16
		dc.l	65536/8,69433/8,73562/8,77936/8,82570/8,87480/8,92682/8,98193/8,104032/8,110218/8,116772/8,123715/8
		dc.l	65536/4,69433/4,73562/4,77936/4,82570/4,87480/4,92682/4,98193/4,104032/4,110218/4,116772/4,123715/4
		dc.l	65536/2,69433/2,73562/2,77936/2,82570/2,87480/2,92682/2,98193/2,104032/2,110218/2,116772/2,123715/2
		dc.l	65536,69433,73562,77936,82570,87480,92682,98193,104032,110218,116772,123715
		dc.l	65536*2,69433*2,73562*2,77936*2,82570*2,87480*2,92682*2,98193*2,104032*2,110218*2,116772*2,123715*2
		dc.l	65536*4,69433*4,73562*4,77936*4,82570*4,87480*4,92682*4,98193*4,104032*4,110218*4,116772*4,123715*4
		dc.l	65536*8,69433*8,73562*8,77936*8,82570*8,87480*8,92682*8,98193*8,104032*8,110218*8,116772*8,123715*8
		dc.l	65536*16,69433*16,73562*16,77936*16,82570*16,87480*16,92682*16,98193*16

;------------------------------------------------------------------------

PitchTable:

pitch_ix	SET	128

		REPT	128
		dc.l	-3678*pitch_ix/64
pitch_ix	SET	pitch_ix-1
		ENDR

		REPT	128
		dc.l	3897*pitch_ix/64
pitch_ix	SET	pitch_ix+1
		ENDR

;------------------------------------------------------------------------

		STRUCTURE MusChannel,0
		APTR	ch_Instr
		APTR	ch_Map
		ULONG	ch_Pitch
		APTR	ch_LtVol
		APTR	ch_RtVol
		BYTE	ch_Vol
		BYTE	ch_Pan


		CNOP	0,4

Channels	dc.l	Channel0,Channel1,Channel2,Channel3
		dc.l	Channel4,Channel5,Channel6,Channel7
		dc.l	Channel8,Channel9,Channel10,Channel11
		dc.l	Channel12,Channel13,Channel14,Channel15


		CNOP	0,4

Channel0	dc.l	0		; instrument
		dc.l	Channel0Map	; note to voice map
		dc.l	0		; pitch wheel setting
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; volume
		dc.b	0		; pan setting

		CNOP	0,4

Channel1	dc.l	0		; instrument
		dc.l	Channel1Map	; note to voice map
		dc.l	0		; pitch wheel setting
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; volume
		dc.b	0		; pan setting

		CNOP	0,4

Channel2	dc.l	0		; instrument
		dc.l	Channel2Map	; note to voice map
		dc.l	0		; pitch wheel setting
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; volume
		dc.b	0		; pan setting

		CNOP	0,4

Channel3	dc.l	0		; instrument
		dc.l	Channel3Map	; note to voice map
		dc.l	0		; pitch wheel setting
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; volume
		dc.b	0		; pan setting

		CNOP	0,4

Channel4	dc.l	0		; instrument
		dc.l	Channel4Map	; note to voice map
		dc.l	0		; pitch wheel setting
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; volume
		dc.b	0		; pan setting

		CNOP	0,4

Channel5	dc.l	0		; instrument
		dc.l	Channel5Map	; note to voice map
		dc.l	0		; pitch wheel setting
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; volume
		dc.b	0		; pan setting

		CNOP	0,4

Channel6	dc.l	0		; instrument
		dc.l	Channel6Map	; note to voice map
		dc.l	0		; pitch wheel setting
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; volume
		dc.b	0		; pan setting

		CNOP	0,4

Channel7	dc.l	0		; instrument
		dc.l	Channel7Map	; note to voice map
		dc.l	0		; pitch wheel setting
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; volume
		dc.b	0		; pan setting

		CNOP	0,4

Channel8	dc.l	0		; instrument
		dc.l	Channel8Map	; note to voice map
		dc.l	0		; pitch wheel setting
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; volume
		dc.b	0		; pan setting

		CNOP	0,4

Channel9	dc.l	0		; instrument
		dc.l	Channel9Map	; note to voice map
		dc.l	0		; pitch wheel setting
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; volume
		dc.b	0		; pan setting

		CNOP	0,4

Channel10	dc.l	0		; instrument
		dc.l	Channel10Map	; note to voice map
		dc.l	0		; pitch wheel setting
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; volume
		dc.b	0		; pan setting

		CNOP	0,4

Channel11	dc.l	0		; instrument
		dc.l	Channel11Map	; note to voice map
		dc.l	0		; pitch wheel setting
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; volume
		dc.b	0		; pan setting

		CNOP	0,4

Channel12	dc.l	0		; instrument
		dc.l	Channel12Map	; note to voice map
		dc.l	0		; pitch wheel setting
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; volume
		dc.b	0		; pan setting

		CNOP	0,4

Channel13	dc.l	0		; instrument
		dc.l	Channel13Map	; note to voice map
		dc.l	0		; pitch wheel setting
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; volume
		dc.b	0		; pan setting

		CNOP	0,4

Channel14	dc.l	0		; instrument
		dc.l	Channel14Map	; note to voice map
		dc.l	0		; pitch wheel setting
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; volume
		dc.b	0		; pan setting

		CNOP	0,4

Channel15	dc.l	0		; instrument
		dc.l	Channel15Map	; note to voice map
		dc.l	0		; pitch wheel setting
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; volume
		dc.b	0		; pan setting


		CNOP	0,4

Channel0Map	dcb.b	128,0
Channel1Map	dcb.b	128,0
Channel2Map	dcb.b	128,0
Channel3Map	dcb.b	128,0
Channel4Map	dcb.b	128,0
Channel5Map	dcb.b	128,0
Channel6Map	dcb.b	128,0
Channel7Map	dcb.b	128,0
Channel8Map	dcb.b	128,0
Channel9Map	dcb.b	128,0
Channel10Map	dcb.b	128,0
Channel11Map	dcb.b	128,0
Channel12Map	dcb.b	128,0
Channel13Map	dcb.b	128,0
Channel14Map	dcb.b	128,0
Channel15Map	dcb.b	128,0

;--------------------------------------

		STRUCTURE AudioVoice,0
		APTR	vc_Next
		APTR	vc_Channel
		APTR	vc_Wave
		ULONG	vc_Index
		ULONG	vc_Step
		ULONG	vc_Loop
		ULONG	vc_Length
		APTR	vc_LtVol
		APTR	vc_RtVol
		BYTE	vc_Flags	; b7 = SFX, b1 = RLS, b0 = EN

		CNOP	0,4

Voices		dc.l	Voice0,Voice1,Voice2,Voice3
		dc.l	Voice4,Voice5,Voice6,Voice7
		dc.l	Voice8,Voice9,Voice10,Voice11
		dc.l	Voice12,Voice13,Voice14,Voice15

sfxVoiceTbl	dc.l	Voice16,Voice17,Voice18,Voice19
		dc.l	Voice20,Voice21,Voice22,Voice23
		dc.l	Voice24,Voice25,Voice26,Voice27
		dc.l	Voice28,Voice29,Voice30,Voice31

; Music Voices

		CNOP	0,4

Voice0		dc.l	Voice1
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice1		dc.l	Voice2
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice2		dc.l	Voice3
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice3		dc.l	Voice4
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice4		dc.l	Voice5
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice5		dc.l	Voice6
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice6		dc.l	Voice7
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice7		dc.l	Voice8
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice8		dc.l	Voice9
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice9		dc.l	Voice10
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice10		dc.l	Voice11
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice11		dc.l	Voice12
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice12		dc.l	Voice13
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice13		dc.l	Voice14
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice14		dc.l	Voice15
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice15		dc.l	0
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

; Sound Effect Voices

		CNOP	0,4

Voice16		dc.l	Voice17
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice17		dc.l	Voice18
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice18		dc.l	Voice19
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice19		dc.l	Voice20
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice20		dc.l	Voice21
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice21		dc.l	Voice22
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice22		dc.l	Voice23
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice23		dc.l	Voice24
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice24		dc.l	Voice25
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice25		dc.l	Voice26
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice26		dc.l	Voice27
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice27		dc.l	Voice28
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice28		dc.l	Voice29
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice29		dc.l	Voice30
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice30		dc.l	Voice31
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

		CNOP	0,4

Voice31		dc.l	0
		dc.l	0		; channel back-link
		dc.l	0		; instrument wave data
		dc.l	0		; sample index
		dc.l	0		; sample rate
		dc.l	0		; instrument loop point
		dc.l	0		; instrument data length
		dc.l	0		; left volume table
		dc.l	0		; right volume table
		dc.b	0		; voice flags

;--------------------------------------

		STRUCTURE InstrumentRec,0
		APTR	in_Wave
		ULONG	in_Loop
		ULONG	in_Length
		BYTE	in_Flags
		BYTE	in_Base


		CNOP	0,4

Instruments	dcb.l	256,0

		CNOP	0,4

QuietInst	dc.l	0
		dc.l	0
		dc.l	0
		dc.b	0
		dc.b	0


		CNOP	0,4

InstrHandle	dc.l	0
InstrFile	dc.l	InstrName
InstrPool	dc.l	0

InstrName	dc.b	'MIDI_Instruments',0

		CNOP	0,4

validInstr	dc.b	%11111111	; (00-07) Piano
		dc.b	%11111111	; (08-0F) Chrom Perc
		dc.b	%11111111	; (10-17) Organ
		dc.b	%11111111	; (18-1F) Guitar
		dc.b	%11111111	; (20-27) Bass
		dc.b	%11111111	; (28-2F) Strings
		dc.b	%11111111	; (30-37) Ensemble
		dc.b	%11111111	; (38-3F) Brass
		dc.b	%11111111	; (40-47) Reed
		dc.b	%11111111	; (48-4F) Pipe
		dc.b	%11111111	; (50-57) Synth Lead
		dc.b	%11111111	; (58-5F) Synth Pad
		dc.b	%11111111	; (60-67) Synth Effects
		dc.b	%11111111	; (68-6F) Ethnic
		dc.b	%11111111	; (70-77) Percussive
		dc.b	%11111111	; (78-7F) SFX
		dc.b	%00000001	; (80-87) invalid,Drum
		dc.b	%11111111	; (88-8F) Drums/Clap/Hi-Hat
		dc.b	%11111111	; (90-97) Hi-Hats/Toms/Cymb1
		dc.b	%11111111	; (98-9F) Cymbals/Bells/Slap
		dc.b	%11111111	; (A0-A7) Bongos/Congas/Timb
		dc.b	%11111111	; (A8-AF) Agogo/Whistles/Gui
		dc.b	%11111100	; (B0-B7) Claves/Block/Trian
		dc.b	%00000000	; (B8-BF) invalid
		dc.b	%00000000	; (C0-C7)
		dc.b	%00000000	; (C8-CF)
		dc.b	%00000000	; (D0-D7)
		dc.b	%00000000	; (D8-DF)
		dc.b	%00000000	; (E0-E7)
		dc.b	%00000000	; (E8-EF)
		dc.b	%00000000	; (F0-F7)
		dc.b	%00000000	; (F8-FF)

;--------------------------------------------------------------------
		section	PlayMusChip,data_c

chipBuffer	dcb.b	1024,0

ClearBuf	dcb.b	160,0

;------------------------------------------------------------------------
		section	PlayMusBSS,bss

tempAudio	ds.b	256

;------------------------------------------------------------------------

		end
