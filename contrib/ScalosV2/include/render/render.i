	IFND RENDER_I
RENDER_I	SET	1
**
**	$VER: render.i 28.4 (31.5.98)
**
**	render.library definitions
**
**	© 1996 TEK neoscientists
**

;------------------------------------------------------------------------

	IFND UTILITY_TAGITEM_I
	include "utility/tagitem.i"
	ENDC

;------------------------------------------------------------------------

RND_TAGBASE		EQU	TAG_USER+$1000


;------------------------------------------------------------------------
;
;	memhandler
;
;------------------------------------------------------------------------

RND_MemType		EQU	RND_TAGBASE+1	; type of memhandler, see below
RND_MemBlock		EQU	RND_TAGBASE+2	; ptr to block of memory
RND_MemSize		EQU	RND_TAGBASE+3	; size of memblock [bytes]
RND_MemFlags		EQU	RND_TAGBASE+18	; memflags (exec/memory.h)
RND_RMHandler		EQU	RND_TAGBASE+12	; to pass a memhandler as an argument

*
*	memhandler types
*

RMHTYPE_POOL		EQU	1		; v39 exec dynamic pool
RMHTYPE_PRIVATE		EQU	2		; private memory pool
RMHTYPE_PUBLIC		EQU	3		; common public memory


;------------------------------------------------------------------------
;
;	palette
;
;------------------------------------------------------------------------

RND_PaletteFormat	EQU	RND_TAGBASE+19	; palette import/export format
RND_EHBPalette		EQU	RND_TAGBASE+22	; indicate a palette is EHB
RND_FirstColor		EQU	RND_TAGBASE+23	; first color entry
RND_NewPalette		EQU	RND_TAGBASE+24	; dispose the old palette and load a new one
RND_RGBWeight		EQU	RND_TAGBASE+11	; quantization factors for ExtractPalette()

*
*	palette format types
*

PALFMT_RGB32		EQU	1		; ULONG red,green,blue
PALFMT_RGB8		EQU	2		; ULONG $00rrggbb
PALFMT_RGB4		EQU	3		; UWORD	$0rgb
PALFMT_PALETTE		EQU	4		; render.library palette

*
*	palette sort mode types
*	for the use with SortPalette()
*

	; no particular order
PALMODE_NONE		EQU	$0000

	; sort palette entries by brightness
PALMODE_BRIGHTNESS	EQU	$0001		

	; sort palette entries by the number of pixels that they represent.
	; You must supply the RND_Histogram taglist argument.
PALMODE_POPULARITY	EQU	$0002

	; sort palette entries by the number of histogram entries that they
	; represent. You must supply the RND_Histogram taglist argument.
PALMODE_REPRESENTATION	EQU	$0003

	; sort palette entries by their optical significance for the human
	; eye. Implementation is unknown to you and may change.
	; You must supply the RND_Histogram taglist argument.
PALMODE_SIGNIFICANCE	EQU	$0004

	; sort palette entries by color intensity
PALMODE_SATURATION	EQU	$0005

	; By default, sort direction is descending, i.e. the precedence is
	; more-to-less. Combine with this flag to invert the sort direction.
PALMODE_ASCENDING	EQU	$0008


;------------------------------------------------------------------------
;
;	histogram related
;
;------------------------------------------------------------------------

RND_HSType		EQU	RND_TAGBASE+4	; histogram type, see below
RND_Histogram		EQU	RND_TAGBASE+9	; a histogram as an argument

*
*	Histogram / Palette types
*	to be specified with RND_HSType
*

HSTYPE_12BIT		EQU	4		; 12bit dynamic histogram
HSTYPE_15BIT		EQU	5		; 15bit dynamic histogram
HSTYPE_18BIT		EQU	6		; 18bit dynamic histogram
HSTYPE_21BIT		EQU	7		; 21bit dynamic histogram
HSTYPE_24BIT		EQU	8		; 24bit dynamic histogram
HSTYPE_12BIT_TURBO	EQU	20		; 12bit tabular histogram
HSTYPE_15BIT_TURBO	EQU	21		; 15bit tabular histogram
HSTYPE_18BIT_TURBO	EQU	22		; 18bit tabular histogram

*
*	tags that can be queried via QueryHistogram()
*

RND_NumPixels		EQU	RND_TAGBASE+5	; # pixels in a histogram
RND_NumColors		EQU	RND_TAGBASE+6	; # colors in a histogram


;------------------------------------------------------------------------
;
;	rendering and conversions
;
;------------------------------------------------------------------------

RND_ColorMode		EQU	RND_TAGBASE+7	; color mode, see below
RND_DitherMode		EQU	RND_TAGBASE+8	; dither mode, see below
RND_DitherAmount	EQU	RND_TAGBASE+26	; dither amount
RND_OffsetColorZero	EQU	RND_TAGBASE+10	; first color index to be output

*
*	color mode types
*	to be specified with RND_ColorMode
*

COLORMODE_CLUT		EQU	$0000		; normal palette lookup
COLORMODE_HAM8		EQU	$0001		; HAM8 mode
COLORMODE_HAM6		EQU	$0002		; HAM6 mode
COLORMODE_MASK		EQU	$0003		; mask to determine COLORMODE

*
*	dither mode types
*	to be specified with RND_DitherMode
*

DITHERMODE_NONE		EQU	$0000		; no dither
DITHERMODE_FS		EQU	$0001		; Floyd-Steinberg dither
DITHERMODE_RANDOM	EQU	$0002		; random dither
DITHERMODE_EDD		EQU	$0003		; edd dither


;------------------------------------------------------------------------
;
;	miscellaneous
;
;------------------------------------------------------------------------

RND_ProgressHook	EQU	RND_TAGBASE+13	; progress callback hook
RND_SourceWidth		EQU	RND_TAGBASE+14	; total input width [pixels]
RND_DestWidth		EQU	RND_TAGBASE+15	; total output width [pixels]
RND_PenTable		EQU	RND_TAGBASE+16	; ptr to a chunky conversion table
RND_LeftEdge		EQU	RND_TAGBASE+17	; chunky data left edge [pixels]
RND_LineHook		EQU     RND_TAGBASE+20	; line callback hook
RND_MapEngine		EQU	RND_TAGBASE+27	; Mapping-Engine
RND_Interleave		EQU	RND_TAGBASE+28	; Interleave
RND_Palette		EQU	RND_TAGBASE+29	; Palette
RND_Weight		EQU	RND_TAGBASE+30	; Weight factor
RND_ScaleEngine		EQU	RND_TAGBASE+31	; ScaleEngine
RND_DestCoordinates	EQU	RND_TAGBASE+42	; Texture Coordinates
RND_BGColor		EQU	RND_TAGBASE+43	; backcolor for filling
RND_BGPen		EQU	RND_TAGBASE+44	; backpen for filling


;------------------------------------------------------------------------
;
;	alpha-channel
;
;------------------------------------------------------------------------

RND_AlphaChannel	EQU	RND_TAGBASE+32	; custom alpha-channel
RND_AlphaModulo		EQU	RND_TAGBASE+33	; bytes between alpha-channel pixels
RND_AlphaWidth		EQU	RND_TAGBASE+34	; width of alpha-channel array
RND_MaskRGB		EQU	RND_TAGBASE+35	; masking RGB for CreateAlphaArray
RND_MaskFalse		EQU	RND_TAGBASE+36	; mask value for outside color range
RND_MaskTrue		EQU	RND_TAGBASE+37	; mask value for inside color range

RND_SourceWidth2	EQU	RND_TAGBASE+38	; total source width for 3channel operations
RND_AlphaChannel2	EQU	RND_TAGBASE+39	; second custom alpha-channel
RND_AlphaModulo2	EQU	RND_TAGBASE+40	; pixel modulo for a second alpha-channel
RND_AlphaWidth2		EQU	RND_TAGBASE+41	; width of a second alpha-channel array

;------------------------------------------------------------------------
;
;	PixelFormat
;
;------------------------------------------------------------------------

RND_PixelFormat    	EQU	RND_TAGBASE+25	; pixel format, see below

PIXFMTB_CHUNKY		EQU	3
PIXFMTB_BITMAP		EQU	4
PIXFMTB_RGB		EQU	5

PIXFMT_CHUNKY_CLUT	EQU	1<<PIXFMTB_CHUNKY+COLORMODE_CLUT
PIXFMT_0RGB_32		EQU	1<<PIXFMTB_RGB+0

*
*	these types are currently not used by render.library, but
*	some of them are applicable for guigfx.library functions:
*

PIXFMT_CHUNKY_HAM8	EQU	1<<PIXFMTB_CHUNKY+COLORMODE_HAM8
PIXFMT_CHUNKY_HAM6	EQU	1<<PIXFMTB_CHUNKY+COLORMODE_HAM6
PIXFMT_BITMAP_CLUT	EQU	1<<PIXFMTB_BITMAP+COLORMODE_CLUT
PIXFMT_BITMAP_HAM8	EQU	1<<PIXFMTB_BITMAP+COLORMODE_HAM8
PIXFMT_BITMAP_HAM6	EQU	1<<PIXFMTB_BITMAP+COLORMODE_HAM6

PIXFMT_RGB_24		EQU	1<<PIXFMTB_RGB+1

*
*	strictly internal:
*

PIXFMT_BITMAP_RGB	EQU	(1<<PIXFMTB_BITMAP)+(1<<PIXFMTB_RGB)


;------------------------------------------------------------------------
;
;	ExtractPalette return codes
;
;	You must at least check for EXTP_SUCCESS.
;	EXTP_NO_DATA indicates that there were no colors
;	in the histogram.
;
;------------------------------------------------------------------------

EXTP_SUCCESS			EQU	0
EXTP_NOT_ENOUGH_MEMORY		EQU	1
EXTP_CALLBACK_ABORTED		EQU	2
EXTP_NO_DATA			EQU	3


;------------------------------------------------------------------------
;
;	AddRGB, AddRGBImage, AddChunkyImage return codes
;
;	You must at least check for ADDH_SUCCESS.
;	If not returned, the histogram is no longer
;	guaranteed to be accurate.
;
;------------------------------------------------------------------------

ADDH_SUCCESS			EQU	0
ADDH_NOT_ENOUGH_MEMORY		EQU	1
ADDH_CALLBACK_ABORTED		EQU	2
ADDH_NO_DATA			EQU	3


;------------------------------------------------------------------------
;
;	Render return codes
;
;	You must at least check for REND_SUCCESS.
;	If not delivered, the image has not been
;	rendered completely.
;
;------------------------------------------------------------------------

REND_SUCCESS			EQU	0
REND_NOT_ENOUGH_MEMORY		EQU	1
REND_CALLBACK_ABORTED		EQU	2
REND_NO_VALID_PALETTE		EQU	3


;------------------------------------------------------------------------
;
;	SortPalette return codes
;
;	You must at least check for SORTP_SUCCESS.
;	SORTP_NO_DATA indicates that there were data missing,
;	e.g. you specified no histogram or the histogram was empty.
;
;------------------------------------------------------------------------

SORTP_SUCCESS			EQU	0
SORTP_NO_DATA			EQU	1
SORTP_NOT_ENOUGH_MEMORY		EQU	2


;------------------------------------------------------------------------
;
;	conversion return codes
;
;	These return codes apply to conversion functions
;	such as Chunky2RGB and ConvertChunky.
;
;------------------------------------------------------------------------

CONV_SUCCESS			EQU	0
CONV_CALLBACK_ABORTED		EQU	1
CONV_NOT_ENOUGH_MEMORY		EQU	2


;========================================================================

	ENDC
