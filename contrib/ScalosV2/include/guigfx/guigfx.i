	IFND GUIGFX_I
GUIGFX_I	SET	1
**
**	$VER: guigfx.i 15.0 (9.2.99)
**
**	guigfx.library definitions
**
**	© 1997-99 TEK neoscientists
**

;------------------------------------------------------------------------

	IFND UTILITY_TAGITEM_I
	include "utility/tagitem.i"
	ENDC

	IFND RENDER_I
	include "render/render.i"
	ENDC

;------------------------------------------------------------------------

*
*	Tags
*

GGFX_Dummy		EQU 	4567+TAG_USER
GGFX_Owner		EQU	GGFX_Dummy+0	; strictly private
GGFX_HSType		EQU	GGFX_Dummy+1
GGFX_DitherMode		EQU	GGFX_Dummy+2
GGFX_DitherAmount	EQU	GGFX_Dummy+3
GGFX_AutoDither		EQU	GGFX_Dummy+4
GGFX_DitherThreshold	EQU	GGFX_Dummy+5
GGFX_AspectX		EQU	GGFX_Dummy+6
GGFX_AspectY		EQU	GGFX_Dummy+7
GGFX_PixelFormat	EQU	GGFX_Dummy+8
GGFX_Palette		EQU	GGFX_Dummy+9
GGFX_PaletteFormat	EQU	GGFX_Dummy+10
GGFX_NumColors		EQU	GGFX_Dummy+11
GGFX_Precision		EQU	GGFX_Dummy+12
GGFX_Weight		EQU	GGFX_Dummy+13
GGFX_Ratio		EQU	GGFX_Dummy+14
GGFX_SourceWidth	EQU	GGFX_Dummy+15
GGFX_SourceHeight	EQU	GGFX_Dummy+16
GGFX_SourceX		EQU	GGFX_Dummy+17
GGFX_SourceY		EQU	GGFX_Dummy+18
GGFX_DestWidth		EQU	GGFX_Dummy+19
GGFX_DestHeight		EQU	GGFX_Dummy+20
GGFX_DestX		EQU	GGFX_Dummy+21
GGFX_DestY		EQU	GGFX_Dummy+22
GGFX_CallBackHook	EQU	GGFX_Dummy+23
GGFX_ErrorCode		EQU	GGFX_Dummy+24
GGFX_MaxAllocPens	EQU	GGFX_Dummy+25
GGFX_BufferSize		EQU	GGFX_Dummy+26
GGFX_AlphaPresent	EQU	GGFX_Dummy+27
GGFX_Independent	EQU	GGFX_Dummy+28
GGFX_ModeID		EQU	GGFX_Dummy+29
GGFX_PenTable		EQU	GGFX_Dummy+30
GGFX_License		EQU	GGFX_Dummy+31		; obsolete
GGFX_BGColor		EQU	GGFX_Dummy+32		; private
GGFX_UseMask		EQU	GGFX_Dummy+33


*
*	Picture Attributes
*

PICATTR_Dummy		EQU	123+TAG_USER
PICATTR_Width		EQU	PICATTR_Dummy+0
PICATTR_Height		EQU	PICATTR_Dummy+1
PICATTR_RawData		EQU	PICATTR_Dummy+2
PICATTR_PixelFormat	EQU	PICATTR_Dummy+3
PICATTR_AspectX		EQU	PICATTR_Dummy+4
PICATTR_AspectY		EQU	PICATTR_Dummy+5
PICATTR_AlphaPresent	EQU	PICATTR_Dummy+6


*
*	Picture Methods
*

PICMTHD_CROP		EQU	1
PICMTHD_RENDER		EQU	2
PICMTHD_SCALE		EQU	3
PICMTHD_MIX		EQU	4
PICMTHD_SETALPHA	EQU	5
PICMTHD_MIXALPHA	EQU	6
PICMTHD_MAPDRAWHANDLE	EQU	7
PICMTHD_CREATEALPHAMASK	EQU	8
PICMTHD_TINT		EQU	9
PICMTHD_TEXTURE		EQU	10
PICMTHD_SET		EQU	11
PICMTHD_TINTALPHA	EQU	12
PICMTHD_INSERT		EQU	13
PICMTHD_FLIPX		EQU	14
PICMTHD_FLIPY		EQU	15
PICMTHD_CHECKAUTODITHER	EQU	16
PICMTHD_NEGATIVE	EQU	17
PICMTHD_AUTOCROP	EQU	18


*
*	hook message types
*

GGFX_MSGTYPE_LINEDRAWN	EQU	1


*
*	picture locking
*

LOCKMODE_DRAWHANDLE	EQU	1
LOCKMODE_FORCE		EQU	(1<<8)
LOCKMODE_MASK		EQU	($ff)


;------------------------------------------------------------------------

	ENDC
