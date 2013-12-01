// Execute_Command.h
// $Date$
// $Revision$


#ifndef EXECUTE_COMMAND_H
#define	EXECUTE_COMMAND_H

#define d1(x)		;
#define d2(x)		x;


extern int kprintf(CONST_STRPTR, ...);
extern int KPrintF(CONST_STRPTR, ...);

struct Execute_Command_LocaleInfo
{
	APTR li_LocaleBase;
	APTR li_Catalog;
	struct LocaleIFace *li_ILocale;
};

#endif	/* EXECUTE_COMMAND_H */
