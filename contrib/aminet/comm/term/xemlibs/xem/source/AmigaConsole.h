#include "ownincs/xem.h"

#define NEWLINE			(1<<0)		/* für r_stat */
#define X_SCROLLCURSOR	(1<<1)
#define Y_SCROLLCURSOR	(1<<2)
#define CURSOR				(1<<3)

struct AmigaConsole {
	struct XEM_IO *io;
	struct XEmulatorMacroKey *macrokeys;

	/* AmigaANSI spezifisches.. */
/*	struct	TextFont	*font;*/
	struct	MsgPort	*wconport;
	struct	IOStdReq *wcon;

	UBYTE		preStr[1024];	/* hier werden die presets als ANSI-String gespeichert */
};
