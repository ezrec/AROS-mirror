	/* Relative Koordinaten beim   */
	/* ersten Öffnen eines Fenster */

#define WNDXOPENOFF (25)
#define WNDYOPENOFF ((Scr->RastPort.TxHeight + Scr->WBorTop + 1) * 3 / 2)

#define BORDERX 4		/* Abstand Border - Bevelbox/Buttons */
#define BORDERY 2

#define BUTTONX 4		/* Abstand Buttons - Bevelbox */
#define BUTTONY 2

#define INNERX 4		/* Abstand Gadget - Bevelbox */
#define INNERY 2

#define INNERGADGETY 2	/* Abstand Gadget - Gadget */
#define INNERGADGETX 4

	/* Anzahl der Zeilen, die im Listview dargestellt werden */
	/* Wert für Kick <= 2.0: LISTVIEWLINES - 1 */

#define LISTVIEWLINES 6

	/* Vom System vorgegebene feste Werte */

#define BEVELX 2						/* Breite der Bevelboxlinie */
#define BEVELY 1						/* Höhe der Bevelboxlinie */
#define TEXTGADGETX 8				/* Abstand Text - Gadget */
#define LISTVIEWBORDERY 4			/* Zusätzlich Höhe für YBorder */
#define LISTVIEWBORDERY20 4		/* Differenz Kick30-Kick20 */
#define LISTVIEWTEXTGADGETY 4		/* Abstand Gadget - Title */

struct WindowInfo
{
	struct RastPort RP;
	struct TextAttr TA;
	struct TextFont *TF;

	ULONG MaxCharWidth, AverageCharWidth, MaxNumericWidth, MaxHexWidth;
};
