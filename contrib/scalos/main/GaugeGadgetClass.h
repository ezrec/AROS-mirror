// GaugeGadgetClass.h
// $Date$
// $Revision$

#ifndef GAUGEGADGETCLASS_H
#define GAUGEGADGETCLASS_H


#define	PCBAR_KIND	1004


/* Tags for PcBar Gadget class */

#define	SCAGAUGE_TagBase	(TAG_USER + 0x91a00)

#define	SCAGAUGE_BarPen		(SCAGAUGE_TagBase + 1)	// (short) Pen-Nummer for bar
#define	SCAGAUGE_MinValue	(SCAGAUGE_TagBase + 2)
#define	SCAGAUGE_MaxValue	(SCAGAUGE_TagBase + 3)
#define	SCAGAUGE_Level		(SCAGAUGE_TagBase + 4)
#define	SCAGAUGE_Vertical	(SCAGAUGE_TagBase + 5)
#define	SCAGAUGE_NumFormat	(SCAGAUGE_TagBase + 6)
#define	SCAGAUGE_WantNumber	(SCAGAUGE_TagBase + 7)
#define	SCAGAUGE_Format		(SCAGAUGE_TagBase + 8)	// (char *) Format string for text
#define	SCAGAUGE_TextPen	(SCAGAUGE_TagBase + 9)	// (short) Pen number for text
#define	SCAGAUGE_TextFont	(SCAGAUGE_TagBase + 10)	// (struct TextAttr *) Font for text
#define	SCAGAUGE_TTFont		(SCAGAUGE_TagBase + 11)	// (struct TTTextAttr *) - TT Font for text

#endif /* GAUGEGADGETCLASS_H */

