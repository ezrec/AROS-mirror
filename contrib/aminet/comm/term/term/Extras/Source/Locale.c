/*
**	Locale.c
**
**	Localization support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* LocaleOpen(STRPTR CatalogName,STRPTR BuiltIn):
	 *
	 *	Open string translation tables.
	 */

VOID
LocaleOpen(STRPTR CatalogName,STRPTR BuiltIn,LONG Version)
{
	DecimalPoint = ".";

	strcpy(ConvNumber,"%ld");
	strcpy(ConvNumber10,"%10ld");

	if(LocaleBase = (struct LocaleBase *)OpenLibrary("locale.library",38))
	{
		if(LocaleBase->lb_SysPatches)
		{
			strcpy(ConvNumber,"%lD");
			strcpy(ConvNumber10,"%10lD");

			if(Catalog = OpenCatalog(NULL,CatalogName,
				OC_BuiltInLanguage,	BuiltIn,
				OC_BuiltInCodeSet,	0,

				Language[0] ? OC_Language : TAG_IGNORE,Language,
			TAG_DONE))
			{
				BOOL TooOld = FALSE;

					/* Don't load an outdated catalog file */

				if(Catalog->cat_Version < Version)
					TooOld = TRUE;
				else
				{
					if(strcmp(GetCatalogStr(Catalog,MSG_OFFSET_TEST1_TXT,""),"v4.0"))
						TooOld = TRUE;
				}

				if(TooOld)
				{
					BOOL IntuitionOpen;

					if(IntuitionBase)
						IntuitionOpen = TRUE;
					else
					{
						IntuitionOpen = FALSE;

						IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",37);
					}

					if(IntuitionBase)
						ShowRequest(NULL,"The catalog file is too old to be used\nwith this `term' revision.","Continue");

					if(!IntuitionOpen)
					{
						CloseLibrary((struct Library *)IntuitionBase);
						IntuitionBase = NULL;
					}

					CloseCatalog(Catalog);
					Catalog = NULL;
				}
			}

			Locale = OpenLocale(NULL);

			DecimalPoint = Locale->loc_DecimalPoint;
		}
		else
		{
			CloseLibrary((struct Library *)LocaleBase);
			LocaleBase = NULL;
		}
	}

		/* If we couldn't open locale.library or if the patches are
		 * not installed, and if we are running under Kickstart 2.04
		 * we'll change the number formatting codes in the built-in
		 * catalog strings.
		 */

	if(!LocaleBase && SysBase->LibNode.lib_Version == 37)
	{
		struct CatCompArrayType *Entry;
		STRPTR String;
		UBYTE c;
		LONG i;

		for(i = 0, Entry = AppStrings ; i < NumAppStrings ; i++, Entry++)
		{
			String = Entry->cca_Str;

			while(c = *String++)
			{
				if(c == '%' && *String != '%')
				{
					STATIC BYTE Stops[256] =
					{
						0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,
						0,0,0,1,0,1,0,0,1,0,0,0,0,0,0,0,
						0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,
						0,0,0,1,0,1,0,0,1,0,0,0,0,0,0,0,
						0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
					};

					while(c = *String)
					{
						if(!Stops[c])
							String++;
						else
						{
							if(c == 'D')
								*String = 'd';

							break;
						}
					}

					if(!c)
						break;
				}
			}
		}
	}
}

	/* LocaleClose():
	 *
	 *	Close the translation tables.
	 */

VOID
LocaleClose()
{
	if(LocaleBase)
	{
		CloseLocale(Locale);
		Locale = NULL;

		CloseCatalog(Catalog);
		Catalog = NULL;

		CloseLibrary((struct Library *)LocaleBase);
		LocaleBase = NULL;
	}
}

	/* LanguageCheck():
	 *
	 *	Checks to see if the currently selected language
	 *	is english.
	 */

VOID
LanguageCheck()
{
	if(Locale && Catalog)
	{
		if(Locale->loc_LanguageName)
		{
			if(!Stricmp(Locale->loc_LanguageName,"english.language"))
				English = TRUE;
			else
				English = FALSE;
		}
		else
			English = FALSE;
	}
	else
		English = TRUE;
}

	/* SmallCurrency():
	 *
	 *	Support function for the rates control panel, returns a formatted
	 *	string to contain a string like "cents/unit".
	 */

VOID
SmallCurrency(STRPTR Buffer,LONG BufferSize)
{
	if(Locale)
		LimitedSPrintf(BufferSize,Buffer,LocaleString(MSG_RATEPANEL_PAY_PER_UNIT_GAD),Locale->loc_MonSmallCS);
	else
		LimitedSPrintf(BufferSize,Buffer,LocaleString(MSG_RATEPANEL_PAY_PER_UNIT_GAD),"Pay");
}

	/* InsertGrouping(STRPTR Buffer,STRPTR GroupData,STRPTR GroupSeparator):
	 *
	 *	Tricky stuff, folks! This beauty will insert grouping characters
	 *	into a readily-prepared string buffer filled with numeric
	 *	contents. It takes the group separator tokens and group separator
	 *	strings into account.
	 */

VOID
InsertGrouping(STRPTR Buffer,STRPTR GroupData,STRPTR GroupSeparator)
{
	UBYTE	LocalBuffer[80];	/* Sufficient, but too large */
	STRPTR	Index;
	LONG	i,j,SeparatorSize;
	LONG	Count;				/* How many characters per group */
	BOOL	RepeatGroupCount;	/* Keep repeating group size until end */

	RepeatGroupCount = FALSE;

		/* Set up for the first group */

	switch(*GroupData)
	{
		case 0:			/* Repeat current grouping scheme until end */
		case 255:		/* No further grouping is to be performed */

			Count = 0;

			break;

		default:		/* Initial group size */

			Count = *GroupData++;

			break;
	}

		/* Check the size of the group separator string */

	if((SeparatorSize = strlen(GroupSeparator) - 1) < 1)
		Count = 0;

		/* That where we'll start */

	Index = LocalBuffer;

		/* Build the string back to front, we will reverse it later */

	for(i = strlen(Buffer) - 1, j = 1 ; i >= 0 ; i--, j++)
	{
			/* Pick up the next number character */

		*Index++ = Buffer[i];

			/* Are we to insert the grouping characters here? */

		if(Count && j == Count)
		{
			LONG k;

				/* Insert the grouping characters */

			for(k = SeparatorSize ; k >= 0 ; k--)
				*Index++ = GroupSeparator[k];

				/* Reset the group size counter */

			j = 0;

				/* Pick up the next grouping token? */

			if(!RepeatGroupCount)
			{
					/* Ok, what kind of token is it? */

				switch(*GroupData)
				{
					case 0:		/* Repeat current grouping scheme */

						RepeatGroupCount = TRUE;
						break;

					case 255:	/* Perform no further grouping */

						Count = 0;
						break;

					default:	/* New group size */

						Count = *GroupData++;
						break;
				}
			}
		}
	}

		/* Look how long the resulting string is */

	j = ((LONG)Index - (LONG)LocalBuffer) - 1;

		/* Copy it back */

	Index = Buffer;

		/* Reverse the order of characters while copying */

	for(i = j ; i >= 0 ; i--)
		*Index++ = LocalBuffer[i];

		/* Provide null-termination */

	*Index = 0;
}

STATIC VOID
ConvertMonetaryQuantity(LONG Units,STRPTR Destination,LONG DestinationSize,BOOL UseCurrency)
{
	UBYTE	IntegerBuffer[80];	/* Sufficient, but too large */

	STRPTR	SignText,			/* Signed/unsigned quantity text */
			SpaceText,			/* Currency/number separation */
			Currency;			/* The name of the currency */
	LONG	SpaceSep,			/* A space separates currency and quantity? */
			SignPos,			/* Where to place the sign text */
			CSPos;				/* Where to place the currency text */
	LONG	Sign;				/* Negative or positive quantity? */

		/* Negative quantity? */

	if(Units < 0)
	{
		Sign = -1;

		Units = -Units;
	}
	else
		Sign = 1;

		/* Does this currency sport a fractional smaller currency? */

	if(Locale->loc_MonFracDigits)
	{
		UBYTE	NumberBuffer[10],
				FractionBuffer[40];
		LONG	Integer,
				Fraction,
				Scale;
		LONG	i;

			/* Prepare the formatting string */

		LimitedSPrintf(sizeof(NumberBuffer),NumberBuffer,"%%0%ldld",Locale->loc_MonFracDigits);

			/* Turn the number of fractional digits into a power of ten */

		for(i = 0, Scale = 1 ; i < Locale->loc_MonFracDigits ; i++)
			Scale *= 10;

			/* Split the quantity in integer and fractional part */

		Integer		= Units / Scale;
		Fraction	= Units % Scale;

			/* Build the integer text */

		LimitedSPrintf(sizeof(IntegerBuffer),IntegerBuffer,"%ld",Integer);

		InsertGrouping(IntegerBuffer,Locale->loc_MonGrouping,Locale->loc_MonGroupSeparator);

			/* Build the fractional text */

		LimitedSPrintf(sizeof(FractionBuffer),FractionBuffer,NumberBuffer,Fraction);

		InsertGrouping(FractionBuffer,Locale->loc_MonFracGrouping,Locale->loc_MonFracGroupSeparator);

			/* Add the monetary decimal point */

		LimitedStrcat(sizeof(IntegerBuffer),IntegerBuffer,Locale->loc_MonDecimalPoint);

			/* Add the fractional part */

		LimitedStrcat(sizeof(IntegerBuffer),IntegerBuffer,FractionBuffer);
	}
	else
	{
			/* Build the integer text */

		LimitedSPrintf(sizeof(IntegerBuffer),IntegerBuffer,"%ld",Units);

		InsertGrouping(IntegerBuffer,Locale->loc_MonGrouping,Locale->loc_MonGroupSeparator);
	}

		/* Pick up the appropriate formatting parameters */

	if(Sign < 0)
	{
		SignText	= Locale->loc_MonNegativeSign;
		SpaceSep	= Locale->loc_MonNegativeSpaceSep;
		SignPos		= Locale->loc_MonNegativeSignPos;
		CSPos		= Locale->loc_MonNegativeCSPos;
	}
	else
	{
		SignText	= Locale->loc_MonPositiveSign;
		SpaceSep	= Locale->loc_MonPositiveSpaceSep;
		SignPos		= Locale->loc_MonPositiveSignPos;
		CSPos		= Locale->loc_MonPositiveCSPos;
	}

		/* Are we to use the currency symbol? */

	if(UseCurrency)
	{
			/* Pick up the currency text */

		Currency = Locale->loc_MonCS;

			/* Take care of the separation information */

		if(SpaceSep == SS_NOSPACE)
			SpaceText = "";
		else
			SpaceText = " ";
	}
	else
		Currency = SpaceText = "";

		/* Now merge all the information into one single string */

	if(CSPos == CSP_PRECEDES)
	{
		switch(SignPos)
		{
			case SP_PARENS:

				/* (Currency <Space> Sign Value) */
				LimitedSPrintf(DestinationSize,Destination,"(%s%s%s%s)",Currency,SpaceText,SignText,IntegerBuffer);
				break;

			case SP_PREC_ALL:

				/* Sign Currency <Space> Value */
				LimitedSPrintf(DestinationSize,Destination,"%s%s%s%s",SignText,Currency,SpaceText,IntegerBuffer);
				break;

			case SP_SUCC_ALL:

				/* Currency <Space> Value Sign */
				LimitedSPrintf(DestinationSize,Destination,"%s%s%s%s",Currency,SpaceText,IntegerBuffer,SignText);
				break;

			case SP_PREC_CURR:

				/* Sign Currency <Space> Value */
				LimitedSPrintf(DestinationSize,Destination,"%s%s%s%s",SignText,Currency,SpaceText,IntegerBuffer);
				break;

			case SP_SUCC_CURR:

				/* Currency Sign <Space> Value */
				LimitedSPrintf(DestinationSize,Destination,"%s%s%s%s",Currency,SignText,SpaceText,IntegerBuffer);
				break;
		}
	}
	else
	{
		switch(SignPos)
		{
			case SP_PARENS:

				/* (Sign Value <Space> Currency) */
				LimitedSPrintf(DestinationSize,Destination,"(%s%s%s%s)",SignText,IntegerBuffer,SpaceText,Currency);
				break;

			case SP_PREC_ALL:

				/* Sign Value <Space> Currency */
				LimitedSPrintf(DestinationSize,Destination,"%s%s%s%s",SignText,IntegerBuffer,SpaceText,Currency);
				break;

			case SP_SUCC_ALL:

				/* Value <Space> Currency Sign */
				LimitedSPrintf(DestinationSize,Destination,"%s%s%s%s",IntegerBuffer,SpaceText,Currency,SignText);
				break;

			case SP_PREC_CURR:

				/* Value <Space> Sign Currency */
				LimitedSPrintf(DestinationSize,Destination,"%s%s%s%s",IntegerBuffer,SpaceText,SignText,Currency);
				break;

			case SP_SUCC_CURR:

				/* Value <Space> Currency Sign */
				LimitedSPrintf(DestinationSize,Destination,"%s%s%s%s",IntegerBuffer,SpaceText,Currency,SignText);
				break;
		}
	}
}

	/* CreateSum(LONG Quantity):
	 *
	 *	Create a string containing a monetary quantity formatted
	 *	according to the current locale rules.
	 */

VOID
CreateSum(LONG Quantity,BOOL UseCurrency,STRPTR Buffer,LONG BufferSize)
{
	if(Locale)
		ConvertMonetaryQuantity(Quantity,Buffer,BufferSize,UseCurrency);
	else
		LimitedSPrintf(BufferSize,Buffer,"%ld.%02ld",Quantity / 100,Quantity % 100);
}

	/* LocalizeString(STRPTR *Strings,LONG From,LONG To):
	 *
	 *	Localize an array of strings.
	 */

VOID
LocalizeString(STRPTR *Strings,LONG From,LONG To)
{
	if(!Strings[0])
	{
		LONG i,j;

		for(i = From, j = 0 ; i <= To ; i++, j++)
			Strings[j] = LocaleString(i);
	}
}

	/* LocalizeStringTable(STRPTR *Strings,LONG *Table)
	 *
	 *	Localize an array of strings by table.
	 */

VOID
LocalizeStringTable(STRPTR *Strings,LONG *Table)
{
	while(*Table != -1)
		*Strings++ = LocaleString(*Table++);

	*Strings = NULL;
}

	/* LocaleString(ULONG ID):
	 *
	 *	Obtain a string from the translation pool.
	 */

STRPTR
LocaleString(ULONG ID)
{
	STRPTR Builtin;

	if(ID < NumAppStrings && AppStrings[ID].cca_ID == ID)
		Builtin = AppStrings[ID].cca_Str;
	else
	{
		LONG Left,Mid,Right;

			/* Binary search, note: can be applied here */
			/* only because the catalog entries are */
			/* stored in ascending ID order. But actually, */
			/* this piece of code should never get called. */

		Left	= 0;
		Right	= NumAppStrings - 1;

		do
		{
			Mid = (Left + Right) / 2;

			if(ID < AppStrings[Mid].cca_ID)
				Right	= Mid - 1;
			else
				Left	= Mid + 1;
		}
		while(ID != AppStrings[Mid].cca_ID && Left <= Right);

		if(ID == AppStrings[Mid].cca_ID)
			Builtin = AppStrings[Mid].cca_Str;
		else
			Builtin = "";
	}

	if(Catalog)
	{
		STRPTR String = GetCatalogStr(Catalog,ID,Builtin);

		if(String[0])
			return(String);
	}

	return(Builtin);
}

#ifndef __AROS__
STRPTR SAVE_DS ASM
LocaleHookFunc(REG(a0) struct Hook *UnusedHook,REG(a2) APTR Unused,REG(a1) LONG ID)
#else
AROS_UFH3(STRPTR, LocaleHookFunc,
 AROS_UFHA(struct Hook * , UnusedHook , A0),
 AROS_UFHA(APTR          , Unused, A2),
 AROS_UFHA(LONG          , ID, A1))
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif
	return(LocaleString(ID));
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

#ifndef __AROS__
STATIC LONG SAVE_DS ASM
FormatDateHookFunc(REG(a0) struct Hook *Hook,REG(a2) APTR Unused,REG(a1) UBYTE Char)
#else
AROS_UFH3(STATIC ULONG, FormatDateHookFunc,
 AROS_UFHA(struct Hook *  , Hook , A0),
 AROS_UFHA(APTR           , Unused, A2),
 AROS_UFHA(UBYTE          , Char, A1))
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif
	struct FormatContext *Context = Hook->h_Data;

	if(Context->Size > 0)
	{
		*Context->Index++ = Char;

		if(--Context->Size == 1)
		{
			*Context->Index = 0;
			Context->Size = 0;
		}
	}

	return(TRUE);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

	/* FormatStamp():
	 *
	 *	Convert a date stamp into human readable
	 *	form by taking the current locale parameters
	 *	into account.
	 */

BOOL
FormatStamp(struct DateStamp *Stamp,STRPTR BothBuffer,LONG BothBufferSize,BOOL SubstituteDay)
{
	struct DateStamp Now;

		/* If no time stamp given, do with current time */

	if(!Stamp)
		CurrentTimeToStamp(Stamp = &Now);

		/* Is the current locale available? */

	if(Locale)
	{
		struct FormatContext Context;
		struct Hook LocalHook;

		InitHook(&LocalHook,(HOOKFUNC)FormatDateHookFunc,&Context);

			/* Combine date and time text? */

		if(!SubstituteDay)
		{
			Context.Index	= BothBuffer;
			Context.Size	= BothBufferSize;

			FormatDate(Locale,Locale->loc_DateTimeFormat,Stamp,&LocalHook);

			StripSpaces(BothBuffer);
		}
		else
		{
			UBYTE DateBuffer[40],TimeBuffer[40];

				/* Are we to substitute the current day with */
				/* text such as today, yesterday, etc.? */

			if(SubstituteDay)
			{
				struct DateStamp Today;
				STRPTR String;

					/* Get the current time */

				CurrentTimeToStamp(&Today);

					/* Does the date refer to yesterday? */

				if(Stamp->ds_Days == Today.ds_Days - 1)
					String = GetLocaleStr(Locale,YESTERDAYSTR);
				else
				{
						/* Does the date refer to today? */

					if(Stamp->ds_Days == Today.ds_Days)
						String = GetLocaleStr(Locale,TODAYSTR);
					else
					{
							/* Does the date refer to tomorrow? */

						if(Stamp->ds_Days == Today.ds_Days + 1)
							String = GetLocaleStr(Locale,TOMORROWSTR);
						else
						{
							String			= NULL;
							SubstituteDay	= FALSE;
						}
					}
				}

				if(String)
					LimitedStrcpy(sizeof(DateBuffer),DateBuffer,String);
				else
					DateBuffer[0] = 0;
			}

			if(!SubstituteDay)
			{
				Context.Index	= DateBuffer;
				Context.Size	= sizeof(DateBuffer);

				FormatDate(Locale,Locale->loc_DateFormat,Stamp,&LocalHook);
			}

			StripSpaces(DateBuffer);

			Context.Index	= TimeBuffer;
			Context.Size	= sizeof(TimeBuffer);

			FormatDate(Locale,Locale->loc_TimeFormat,Stamp,&LocalHook);

			StripSpaces(TimeBuffer);

				/* Combine date and time */

			LimitedSPrintf(BothBufferSize,BothBuffer,"%s %s",DateBuffer,TimeBuffer);
		}
	}
	else
	{
		UBYTE DateBuffer[40],TimeBuffer[40];
		struct DateTime	DateTime;

			/* No locale, so we will use dos.library instead. */

		CopyMem(Stamp,&DateTime.dat_Stamp,sizeof(struct DateStamp));

		DateTime.dat_Format		= FORMAT_DOS;
		DateTime.dat_Flags		= SubstituteDay ? DTF_SUBST : NULL;
		DateTime.dat_StrDay		= NULL;
		DateTime.dat_StrDate	= DateBuffer;
		DateTime.dat_StrTime	= TimeBuffer;

		if(!DateToStr(&DateTime))
			return(FALSE);

		StripSpaces(DateBuffer);
		StripSpaces(TimeBuffer);

			/* Combine date and time */

		LimitedSPrintf(BothBufferSize,BothBuffer,"%s %s",DateBuffer,TimeBuffer);
	}

	return(TRUE);
}

	/* FormatTime(STRPTR Buffer,LONG Hours,LONG Minutes,LONG Seconds):
	 *
	 *	Given hours, minutes and seconds, format this data into
	 *	a human-readable string.
	 */

VOID
FormatTime(STRPTR Buffer,LONG BufferSize,LONG Hours,LONG Minutes,LONG Seconds)
{
	if(Locale)
	{
		struct Hook				LocalHook;
		struct FormatContext	Context;
		struct DateStamp		Stamp;

		Stamp.ds_Days		= 0;
		Stamp.ds_Minute		= Hours * 60 + Minutes;
		Stamp.ds_Tick		= MAX(0,Seconds) * TICKS_PER_SECOND;

		Context.Index	= Buffer;
		Context.Size	= BufferSize;

		InitHook(&LocalHook,(HOOKFUNC)FormatDateHookFunc,&Context);

		if(Seconds < 0)
			FormatDate(Locale,Locale->loc_ShortTimeFormat,&Stamp,&LocalHook);
		else
			FormatDate(Locale,Locale->loc_TimeFormat,&Stamp,&LocalHook);
	}
	else
	{
		if(Seconds < 0)
			LimitedSPrintf(BufferSize,Buffer,"%02ld:%02ld",Hours,Minutes);
		else
			LimitedSPrintf(BufferSize,Buffer,"%02ld:%02ld:%02ld",Hours,Minutes,Seconds);
	}
}

	/* StandardShowTime(struct Gadget *SomeGadget,LONG Level):
	 *
	 *	Callback routine to display some time level.
	 */

STRPTR SAVE_DS STACKARGS
StandardShowTime(struct Gadget *UnusedGadget,LONG Seconds)
{
	STATIC UBYTE Time[10];
	STRPTR FormatString;

	if(LocaleBase)
		FormatString = "%2lD%s%02lD";
	else
		FormatString = "%2ld%s%02ld";

	LimitedSPrintf(sizeof(Time),Time,FormatString,Seconds / 100,DecimalPoint,Seconds % 100);

	return(Time);
}
