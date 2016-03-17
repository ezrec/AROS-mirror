#include "Global.h"
#include "GTPopUp_text.h"
#include "protos.h"

const struct	CatCompArrayType	*AppStrings	= &CatCompArray[0];
WORD		NumAppStrings	= sizeof(CatCompArray) / sizeof(struct CatCompArrayType);

STRPTR LocaleString(ULONG ID);

/*VOID LimitedStrcpy(LONG Size,STRPTR To,STRPTR From)
{
	while(*From && Size > 0)
	{
		*To++ = *From++;

		Size--;
	}

	*To = 0;
}

void LimitedSPrintf(LONG Size,STRPTR Buffer,STRPTR FormatString,...)
{
	va_list VarArgs;

	va_start(VarArgs,FormatString);
	LimitedVSPrintf(Size,Buffer,FormatString,VarArgs);
	va_end(VarArgs);
}

void LimitedVSPrintf(LONG Size,STRPTR Buffer,STRPTR FormatString,va_list VarArgs)
{
	struct FormatContext Context;

	Context.Index	= Buffer;
	Context.Size	= Size;

	RawDoFmt(FormatString,(APTR)VarArgs,(PUTCHAR)StuffChar,(APTR)&Context);
}

void __asm StuffChar(register __a3 struct FormatContext *Context,register __d0 UBYTE Char)
{
	if(Context->Size > 0)
	{
		*Context->Index++ = Char;

		if(--Context->Size == 1)
		{
			*Context->Index = 0;
			Context->Size = 0;
		}
	}
}*/

VOID InitHook(struct Hook *Hook,HOOKFUNC Func,APTR Data)
{
	Hook->h_Entry		= Func;
	Hook->h_Data		= Data;
}

STRPTR LocaleHookFunc(struct Hook *UnusedHook, APTR Unused, LONG ID, struct GlobalData * gd)
{
	return(LocaleString(ID));
}

void Message(UBYTE *Msg,...)
{
	struct EasyStruct Req={sizeof(struct EasyStruct),0,"GTPopUp message",0,"Okay"};
	
	Req.es_TextFormat=Msg;

	DisplayBeep(NULL);

	AROS_SLOWSTACKFORMAT_PRE(Msg);
	EasyRequestArgs(NULL,&Req,0,AROS_SLOWSTACKFORMAT_ARG(Msg));
	AROS_SLOWSTACKFORMAT_POST(Msg);
}

STRPTR LocaleString(ULONG ID)
{
	STRPTR Builtin;
	struct Catalog	*Catalog = NULL;

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
		STRPTR String = (STRPTR)GetCatalogStr(Catalog,ID,Builtin);

		if(String && String[0])
			return(String);
	}

	return(Builtin);
}
