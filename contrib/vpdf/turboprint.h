/**************************************
 * C-Includes
 * for TurboPrint Professional 3.x/4.x
 * December 1995
 **************************************
 * IrseeSoft
 * Meinrad-Spiess-Platz 2
 * D-87660 Irsee
 * Germany
 * Tel. (49) 8341 74327
 * Fax  (49) 8341 12042
 * Email: IrseeSoft@t-online.de
 **************************************/


#define TPFMT_BitPlanes 	0x00
#define TPFMT_Chunky8   	0x01
#define TPFMT_RGB15 			0x12
#define TPFMT_RGB16 			0x13
#define TPFMT_RGB24     	0x14
#define TPFMT_BGR15 			0x02
#define TPFMT_BGR16 			0x03
#define TPFMT_BGR24			0x04

#define TPFMT_HAM				0x800
#define TPFMT_EHB 			0x080
#define TPFMT_CyberGraphX  0x400

#define TPMATCHWORD 0xf10a57ef

#define PRD_TPEXTDUMPRPORT (PRD_DUMPRPORT | 0x80)


#pragma pack(2)
struct TPExtIODRP {
	UWORD PixAspX;		/*Aspect Ratio x/y of a pixel*/
	UWORD PixAspY;
	UWORD Mode;			/* Determines Format of BitMap, e.g. TPFMT_RGB24 */
	};
#pragma pack()
	/*
	APTR Planes[8];
	UWORD BytesPerRow;
	UWORD XOffset;
	}; for internal use only */
