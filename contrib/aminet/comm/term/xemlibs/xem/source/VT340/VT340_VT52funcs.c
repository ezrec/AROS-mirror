#include "ownincs/VT340console.h"


/* allowed globals.. */
IMPORT struct	ExecBase			*SysBase;
IMPORT struct	DosLibrary		*DOSBase;
IMPORT struct	GfxBase			*GfxBase;
IMPORT struct	IntuitionBase	*IntuitionBase;


                 /* VT52 Funktionen zu VT340 Emulator.. */



VOID VT340_invokeVT52(struct VT340Console *con)
{
	VT340_resetcon(con, FALSE);
	con->pstat &= ~P_ANSI_MODE;
	con->pstat |= P_CONVERT;
	con->VT52_dca = FALSE;

	CreateOwnStatusLine(con);
}


VOID VT52_EnterGrafixMode(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	VT340_setset(con, GRAPHICS, 0);
}


VOID VT52_ExitGrafixMode(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	VT340_setset(con, ASCII, 0);
}


VOID VT52_Identify(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	con->io->xem_swrite("\033/Z", 3);
}


VOID VT52_EnterAltKeyboardMode(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	con->ustat |= U_KEYAPP;
}


VOID VT52_ExitAltKeyboardMode(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	con->ustat &= ~U_KEYAPP;
}


VOID VT52_EnterANSI(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	con->pstat |= P_ANSI_MODE;	/* enter last ANSI mode (VT-340 / VT-102) */
	CreateOwnStatusLine(con);
	VT340_resetfonts(con);
}

VOID VT52_EnterAutoPrint(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	if(VT340_openprinterFH(con) != FALSE)
	{
		if(con->gstat & G_PRINT_CONTROL)
			con->gstat |= G_AUTO_PRINT_INACTIVE;
		else
		{
			con->gstat |= G_AUTO_PRINT;
			CreateOwnStatusLine(con);
		}
	}
}


VOID VT52_ExitAutoPrint(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	con->gstat &= ~(G_AUTO_PRINT | G_AUTO_PRINT_INACTIVE);
	VT340_printterminator(con, con->printerHandle);
	CreateOwnStatusLine(con);
}


VOID VT52_EnterControlPrint(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	if(VT340_openprinterFH(con) != FALSE)
	{
		con->gstat |= G_PRINT_CONTROL;

		if(con->gstat & G_AUTO_PRINT)
		{
			con->gstat |=  G_AUTO_PRINT_INACTIVE;
			con->gstat &= ~G_AUTO_PRINT;
		}

		CreateOwnStatusLine(con);
	}
}


VOID VT52_ExitControlPrint(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	con->gstat &= ~G_PRINT_CONTROL;

	if(con->gstat & G_AUTO_PRINT_INACTIVE)
	{
		con->gstat &= ~G_AUTO_PRINT_INACTIVE;
		con->gstat |=  G_AUTO_PRINT;
	}

	VT340_printterminator(con, con->printerHandle);
	CreateOwnStatusLine(con);
}


VOID VT52_PrintScreen(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	if(VT340_openprinterFH(con) != FALSE)
		PagePrintDisplay(con, con->printerHandle, TRUE);
}


VOID VT52_PrintRow(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	if(VT340_openprinterFH(con) != FALSE)
		PagePrintDisplay(con, con->printerHandle, FALSE);
}



/* end of source-code */
