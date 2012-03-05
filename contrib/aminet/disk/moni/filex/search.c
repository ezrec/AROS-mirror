#ifdef USE_PROTO_INCLUDES
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <intuition/sghooks.h>
#include <intuition/cghooks.h>
#include <libraries/gadtools.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/keymap.h>
#include <proto/gadtools.h>
#include <clib/alib_protos.h>

#include <SDI/SDI_hook.h>

#include <string.h>
#include <ctype.h>
#endif

#include "filexstructs.h"
#include "filexstrings.h"

#include "allprotos.h"

char searchstring[256];					/* ASCII-Text-Suchstring */
static char hexsearchstring[128];	/* Hex-Suchbytes */

char replacestring[256];
static char hexreplacestring[128];

long searchmode = SM_STRING;

static struct Speicher ReplaceSpeicher;
static LONG delta[256];

static struct List SHList = {
	.lh_Head = (struct Node *)&SHList.lh_Tail,
	.lh_TailPred = ( struct Node * )&SHList.lh_Head,
};

static struct List RHList = {
	.lh_Head = ( struct Node * )&RHList.lh_Tail,
	.lh_TailPred = ( struct Node * )&RHList.lh_Head,
};

static struct Node *SHAktu;
static struct Node *RHAktu;

void FreeSearchHistory( void )
{
	while( !IsListEmpty( &SHList ))
		FreeNLNode( SHList.lh_Head );

	while( !IsListEmpty( &RHList ))
		FreeNLNode( RHList.lh_Head );
}

void InitSearchHistory( void )
{
}

/*
 * static long CountSpecChars(UBYTE *string,char zeichen)
 *
 * Ermittelt die Anzahl von einem bestimmten Zeichen in einem String
 */

static long CountSpecChars(UBYTE *string,char zeichen)
{
	long num=0;

	while( *string != 0) if( *string++ == zeichen) num++;

	return( num );
}

/*
 * static void Replace(long pos,UBYTE *quelle,long len)
 *
 * Ersetzt einen Text an pos mit der Länge len durch den ab Quelle
 * und ändert auch den Text im Anzeigebereich falls nötig
 */

static void Replace(long pos,UBYTE *quelle,long len, struct DisplayData *DD)
{
	struct Speicher s, rs;
	s.len = len;

	if((s.mem = AllocMem(s.len,MEMF_ANY)))
	{
		rs.len = ReplaceSpeicher.len;

		if(( rs.mem = AllocMem( rs.len, MEMF_ANY )))
		{
			memcpy( rs.mem, ReplaceSpeicher.mem, rs.len );

			AddUndoPaste(pos, s, rs,DD->FD);

				/* Sichern */

			memcpy(s.mem,DD->FD->Mem+pos,len);

				/* String drüber */

			memcpy(DD->FD->Mem+pos,quelle,len);
	
			AllDisplay( DD->FD, SCROLLERNEU);
		}
		else
			FreeMem( s.mem, s.len );
	}
}

/*
 * int TestHexString(char *s)
 *
 * Liefert Anzahl Bytes, falls s ein gültiger Hexstring ist = nur '0'-'9',
 * ' ','A-F','a-f'. Oder -1 falls üngültige Zeichen enthalten sind.
 */

int TestHexString(char *s)
{
	int anz=0;
	char *starts=s;

	while(*s!=0)
	{
		if(isxdigit(*s))	anz++;
		else if(!(*s==' '))
		{
			MyRequest( MSG_INFO_GLOBAL_ILLEGALCHARACTERS, ( IPTR )starts );
			return(-1);
		}
		s++;
	}
	if(anz%2!=0)
	{
		MyRequest( MSG_INFO_GLOBAL_HEXSTRINGNICHTBYTEALIGNED, ( IPTR )starts );
		return(-1);
	}
	else	return(anz);
}

static long memcmpwild(UBYTE *wild,UBYTE *string,long laenge)
{
	long k;

	for(k=0;k<laenge;k++)if((!(wild[k]==WILDCHAR))&&(string[k]!=wild[k]))return(-1);
	
	return(0);
}

static long strnicmpwild(UBYTE *wild,UBYTE *string,long laenge)
{
	long k;

	for(k=0;k<laenge;k++)if((!(wild[k]==WILDCHAR))&&(ToUpper(string[k])!=ToUpper(wild[k])))return(-1);
	
	return(0);
}

/*
 * UBYTE *BoyerMoore(UBYTE *start,UBYTE *ss,ULONG typ)
 *
 * Sucht ab start und gibt Zeiger auf gefundenes zurück
 * ss=Zeiger auf zu suchenden String
 */

static UBYTE *BoyerMoore(UBYTE *start,UBYTE *ss,ULONG typ,LONG len, struct DisplayData *DD )
{
	UBYTE *t;

	if(DD->FD->Len==0)return(NULL);

	if(typ&BM_BACKWARD)
	{
			/* Falls wir zu weit hinten stehen, auf erste pos setzen */
		if(len>DD->FD->Len-(start-DD->FD->Mem))start=DD->FD->Len+DD->FD->Mem-len;

			/* Falls wir nun außerhalb sind */
		if(start<=DD->FD->Mem)return(NULL);

		len--;
		t = start;

		if(typ&BM_WILDCARDS)
		{
			if(typ&BM_CASE)
			while(t>=DD->FD->Mem)
			{
				if(((*ss==WILDCHAR)||(*ss == *t))	&&
					(memcmpwild(ss+1,t+1,len) == 0))
						return(t);
	
				t-=delta[*t];
			}
			else
			while(t>=DD->FD->Mem)
			{
				if(((*ss==WILDCHAR)||(ToUpper(*ss) == ToUpper(*t)))
					&&(strnicmpwild(ss+1,t+1,len) == 0))
						return(t);
	
				t-=delta[*t];
			}
		}
		else
		{
			if(typ&BM_CASE)
			while(t>=DD->FD->Mem)
			{
				if((*ss == *t)	&&
					(memcmp(ss+1,t+1,len) == 0))
						return(t);
	
				t-=delta[*t];
			}
			else
			while(t>=DD->FD->Mem)
			{
				if((ToUpper(*ss) == ToUpper(*t))
					&&(strnicmp(ss+1,t+1,len) == 0))
						return(t);
	
				t-=delta[*t];
			}
		}
	}
	else
	{
		UBYTE *zfende;

		if(len>DD->FD->Len+DD->FD->Mem-start)return(NULL);

		len--;
		zfende=ss+len;
		t = start + len;

		if(typ&BM_WILDCARDS)
		{
			if(typ&BM_CASE)
			while(t<DD->FD->Mem+DD->FD->Len)
			{
				if(((*zfende==WILDCHAR)||(*zfende == *t))&&
					(memcmpwild(zfende-len,t-len,len)==0))
						return(t - len);
	
				t+=delta[*t];
			}
			else
			while(t<DD->FD->Mem+DD->FD->Len)
			{
				if(((*zfende==WILDCHAR)||(ToUpper(*zfende)== ToUpper(*t)))&&
					(strnicmpwild(zfende-len,t-len,len) == 0))
						return(t - len);
	
				t+=delta[*t];
			}
		}
		else
		{
			if(typ&BM_CASE)
			while(t<DD->FD->Mem+DD->FD->Len)
			{
				if((*zfende == *t)&&
					(memcmp(zfende-len,t-len,len)==0))
						return(t - len);
	
				t+=delta[*t];
			}
			else
			while(t<DD->FD->Mem+DD->FD->Len)
			{
				if((ToUpper(*zfende)== ToUpper(*t))&&
					(strnicmp(zfende-len,t-len,len) == 0))
						return(t - len);
	
				t+=delta[*t];
			}
		}
	}
	return(NULL);
}

/*
 * static void InitBM(UBYTE *ss,ULONG typ,ULONG len)
 *
 * Baut die Deltatabelle für den BoyerMooreAlgorithmus auf
 * ss  = zu suchender String
 * typ = Suchart (CASE/BACK)
 */

static void InitBM( UBYTE *ss, ULONG typ, ULONG len )
{
	unsigned char *zf;
	LONG i;

		/* Deltas berechnen */
	for( i = 0; i < 256; i++ )
		delta[ i ] = len;

	if( typ & BM_CASE )
	{
		if(typ&BM_BACKWARD)	for(zf=ss,i=len-1;i>0;--i)	delta[zf[i]]=i;
		else	for(i=len,zf=ss; --i > 0;)	delta[*zf++] = i;
	}
	else
	{
		if(typ&BM_BACKWARD)
		{
			for(zf=ss,i=len-1;i>0;--i)	delta[ToUpper(zf[i])]=i;
			for(i=0;i<256;i++)delta[tolower(i)]=delta[i];
		}
		else
		{
			for(i=len,zf=ss; --i > 0;)
			{
				UBYTE dummy;
				dummy=*zf++;
				delta[ToUpper(dummy)]=i;
			}
			for(i=0;i<256;i++)delta[tolower(i)]=delta[i];
		}
	}

	if( typ & BM_WILDCARDS )
	{
		if( typ & BM_BACKWARD )
		{
			long pos = 0;
	
			for( zf = ss; zf < ss + len; pos ++ )
			{
				if( *zf++ == WILDCHAR )
					break;
			}

			if( !pos )
				pos = 1;
	
			for( i = 0; i < 256; i++ )
				if( delta[ i ] > pos )
					delta[ i ] = pos;
		}
		else
		{
			long pos = 0;
	
			for( zf = ss + len - 1; zf >= ss; pos ++ )
				if( *zf-- == WILDCHAR )
					break;
	
			if( !pos )
				pos = 1;

			for( i = 0; i < 256; i++ )
				if( delta[ i ] > pos )
					delta[ i ] = pos;
		}
	}
}

BOOL SearchNext(int typ,BOOL quiet,BOOL all, struct DisplayData *DD )
{
	UBYTE *pos;
	char *string = NULL;
	LONG len = 0;
	BOOL Success = TRUE;
	struct Window *SWnd;

	SWnd = AktuDI->Wnd;

	SetStatusZeile( GetStr( MSG_INFO_SEARCH_SEARCHING ), DD);

	if(searchmode&SM_REPLACE)
	{
		char *rstring = NULL,*kstring,*krstring;
		long richtung=(typ&BM_BACKWARD)?-1:1;

		if( !(searchmode&SM_STRING))
		{
			int hexsearchbytes, hexreplacebytes;

			typ|=BM_CASE;

			if((-1!=(hexsearchbytes =TestHexString(searchstring)))&&
				(-1!=(hexreplacebytes=TestHexString(replacestring))))
			{
				if(hexsearchbytes == hexreplacebytes)
				{
					len = ConvertHexstringToHexnumber(kstring=searchstring,string=hexsearchstring);
					ConvertHexstringToHexnumber(krstring=replacestring,rstring=hexreplacestring);
				}
				else
				{
					DisplayLocaleText( MSG_INFO_SEARCH_BYTESNOTEQUAL );
					Success = FALSE;
				}
			}
			else
				Success = FALSE;
		}
		else
		{
			if(searchmode&SM_WILDCARDS)
			{
				if(CountSpecChars(searchstring,WILDCHAR))typ|=BM_WILDCARDS;
			}

			if(!(strlen(krstring=rstring=replacestring)==(len=strlen(kstring=string=searchstring))))
			{
				DisplayLocaleText( MSG_INFO_SEARCH_CHARSNOTEQUAL );
				Success = FALSE;
			}

			if(searchmode&SM_CASE)typ|=BM_CASE;
		}

		if(len && Success)
		{
			ReplaceSpeicher.len = len;

			if((ReplaceSpeicher.mem = AllocMem(len,MEMF_ANY)))
			{
				char fstring[256], *fstringzeiger;
				
				fstring[len]=0;
				fstringzeiger = fstring;

				memcpy(ReplaceSpeicher.mem,rstring,len);

				InitBM(string,typ,len);
	
				if(all)
				{
					SetWaitPointer( SWnd );
					if((pos=BoyerMoore(DD->FD->Mem+DD->CPos,string,typ,len, DD)))
					{
						Replace(pos-DD->FD->Mem,rstring,len,DD);
						while((pos=BoyerMoore(pos+richtung,string,typ,len,DD)))
							Replace(pos-DD->FD->Mem,rstring,len,DD);
					}
					ClrWaitPointer( SWnd );

					Success=TRUE;
				}
				else		
				if(quiet)
				{
					if((pos=BoyerMoore(DD->FD->Mem+DD->CPos,string,typ,len,DD)))
					{
						Replace(pos-DD->FD->Mem,rstring,len,DD);
						Success=TRUE;
					}
					else
					{
						Success=FALSE;
					}
				}
				else
				{
					BOOL all=FALSE,EndeFlag=FALSE;
	
					SetWaitPointer( SWnd );

					if((pos=BoyerMoore(DD->FD->Mem+DD->CPos,string,typ,len,DD)))
					do
					{
						Success=TRUE;
						ClrWaitPointer( SWnd );
	
						if(( searchmode & SM_STRING ) && ( searchmode & SM_WILDCARDS ))
							strncpy(fstring,pos,len);
						else
						{
							fstringzeiger = searchstring;
						}
	
						SetCursor(pos-DD->FD->Mem, DD);
						switch(MyFullRequest( MSG_INFO_SEARCH_FOUNDTOREPLACE, MSG_GADGET_REPLACE, fstringzeiger, krstring ))
						{
							case 1:
								Replace(pos-DD->FD->Mem,rstring,len,DD);
								break;
							case 2:
								Replace(pos-DD->FD->Mem,rstring,len,DD);
								all=TRUE;
								SetWaitPointer( SWnd );
								while((pos=BoyerMoore(pos+richtung,string,typ,len,DD)))
									Replace(pos-DD->FD->Mem,rstring,len,DD);
								ClrWaitPointer( SWnd );
								EndeFlag=TRUE;
								break;
							case 3:
								Replace(pos-DD->FD->Mem,rstring,len,DD);
								EndeFlag=TRUE;
								break;
							case 4:
								EndeFlag=TRUE;
								break;
						}
		
						SetWaitPointer( SWnd );
		
					}while((!EndeFlag)&&(pos=BoyerMoore(pos+richtung,string,typ,len,DD)));
					else Success=FALSE;
		
					ClrWaitPointer( SWnd );
		
					if((all==FALSE)&&(EndeFlag==FALSE))
					{
						if(!quiet)
							MyRequest( MSG_INFO_SEARCH_STRINGNOTFOUND, ( IPTR )searchstring );
					}
				}

				FreeMem( ReplaceSpeicher.mem, ReplaceSpeicher.len );
			}
			else
				MyRequest( MSG_INFO_GLOBAL_CANTALLOCMEM, len );
		}
	}
	else
	{
		if(!(searchmode&SM_STRING))
		{
			if(-1!=TestHexString(searchstring))
			{
				len=ConvertHexstringToHexnumber(searchstring,hexsearchstring);
				string=hexsearchstring;
				typ|=BM_CASE;
			}
			else
				Success = FALSE;
		}
		else
		{
			if(searchmode&SM_WILDCARDS)
			{
				if(CountSpecChars(searchstring,WILDCHAR))typ|=BM_WILDCARDS;
			}

			len=strlen(string=searchstring);
			if(searchmode&SM_CASE)typ|=BM_CASE;
		}

		if(Success && len)
		{
			InitBM(string,typ,len);
			SetWaitPointer( SWnd );
			pos=BoyerMoore(DD->FD->Mem+DD->CPos+((typ&BM_BACKWARD)?-1:1),string,typ,len,DD);
			ClrWaitPointer( SWnd );

			if(pos)
			{
				SetCursor(pos-DD->FD->Mem, DD);
				Success=TRUE;
			}
			else
			{
				if(!quiet)
				{
				if(searchmode&SM_STRING)MyRequest( MSG_INFO_SEARCH_STRINGNOTFOUND, ( IPTR )searchstring );
				else MyRequest( MSG_INFO_SEARCH_STRINGNOTFOUND, ( IPTR )searchstring );
    	    	    	    	}
				Success=FALSE;
			}
		}
	}

	UpdateStatusZeile(DD);

	return(Success);
}

void SetSearchString( char *string )
{
	strcpy( searchstring, string );

	if( strlen( string ))
	if( IsListEmpty( &SHList ) || strcmp( string, SHList.lh_TailPred->ln_Name ))
		AddNLName( &SHList, string );
}

void SetReplaceString( char *string )
{
	strcpy( replacestring, string );

	if( strlen( string ))
	if( IsListEmpty( &RHList ) || strcmp( string, RHList.lh_TailPred->ln_Name ))
		AddNLName( &RHList, string );
}

char *GetAktuSearchString( void )
{
	return( searchstring );
}

char *GetAktuReplaceString( void )
{
	return( replacestring );
}

/****************************** SearchWindow ******************************/

enum {

GD_SEARCH_SEARCHSTRING,
GD_SEARCH_REPLACESTRING,

GD_SEARCH_CASESENSITIV,
GD_SEARCH_WILDCARDS,
GD_SEARCH_STRINGSEARCH,
GD_SEARCH_REPLACEMODE,

GD_SEARCH_NEXT,
GD_SEARCH_PREVIOUS,
GD_SEARCH_CANCEL
};

static struct MyNewGadget SearchNewGadgets[] =
{
	{STRING_KIND, 0, 0, 0, MSG_GADGET_SEARCH_SEARCHSTRING, 0, 256, 0, 30, 30},
	{STRING_KIND, 0, 0, 0, MSG_GADGET_SEARCH_REPLACESTRING, 0, 256, 0, 30, 30},

	{CHECKBOX_KIND, GP_NEWCOLUMN, 0, 0, MSG_GADGET_SEARCH_CASESENSITIV, 0, 0, 0, 0, 0},
	{CHECKBOX_KIND, 0, 0, 0, MSG_GADGET_SEARCH_WILDCARDS, 0, 0, 0, 0, 0},
	{CHECKBOX_KIND, 0, 0, 0, MSG_GADGET_SEARCH_STRINGSEARCH, 0, 0, 0, 0, 0},
	{CHECKBOX_KIND, 0, 0, 0, MSG_GADGET_SEARCH_REPLACEMODE, 0, 0, 0, 0, 0},

	{BUTTON_KIND, GP_LEFTBOTTOM, 0, 0, MSG_GADGET_SEARCH_NEXT, 0, 0, 0, 0, 0},
	{BUTTON_KIND, GP_MIDDLEBOTTOM, 0, 0, MSG_GADGET_SEARCH_PREVIOUS, 0, 0, 0, 0, 0},
	{BUTTON_KIND, GP_RIGHTBOTTOM, 0, 0, MSG_GADGET_CANCEL, 0, 0, 0, 0, 0},
	{0}
};

static struct WindowData SearchWD =
{
	NULL,	NULL,	FALSE, NULL, NULL,
	0,0,
	&SearchNewGadgets[ 0 ], 9
};

static void DoSearchWndMsg( void )
{
	struct IntuiMessage	*m, Msg;
	struct Gadget *gad;
	int Select = -1;	/* Wie wurde Window verlassen? (-1=Nicht/0=Cancel / Next / Previous) */

	while(( m = GT_GetIMsg( SearchWD.Wnd->UserPort )))
	{
		CopyMem(( char * )m, ( char * )&Msg, (long)sizeof( struct IntuiMessage ));

		GT_ReplyIMsg( m );

		KeySelect( SearchWD.Gadgets, &Msg );

		gad = (struct Gadget *) Msg.IAddress;

		switch( Msg.Class )
		{
/*			case	IDCMP_REFRESHWINDOW:
				GT_BeginRefresh( SearchWD.Wnd );
				GT_EndRefresh( SearchWD.Wnd, TRUE );
				break;*/

			case	IDCMP_VANILLAKEY:
				if( Msg.Code == 13)
					Select = 1;
				break;

			case	IDCMP_CLOSEWINDOW:
				Select = 0;
				break;

			case	IDCMP_GADGETUP:
				switch(gad->GadgetID)
				{
					case GD_SEARCH_SEARCHSTRING:
						if( SearchNewGadgets[ GD_SEARCH_STRINGSEARCH ].CurrentValue == FALSE )
						{
							if(-1==TestHexString(GetString(SearchWD.Gadgets[GD_SEARCH_SEARCHSTRING])))
								ActivateGadget(SearchWD.Gadgets[GD_SEARCH_SEARCHSTRING],SearchWD.Wnd,0);
							else
								if( SearchNewGadgets[ GD_SEARCH_REPLACEMODE ].CurrentValue )
									ActivateGadget( SearchWD.Gadgets[ GD_SEARCH_REPLACESTRING ], SearchWD.Wnd, 0 );
								else
									Select = 1;
						}
						else
						{
							if( SearchNewGadgets[ GD_SEARCH_REPLACEMODE ].CurrentValue )
								ActivateGadget( SearchWD.Gadgets[ GD_SEARCH_REPLACESTRING ], SearchWD.Wnd, 0 );
							else
								Select = 1;
						}
						break;

					case GD_SEARCH_REPLACESTRING:
						if(( SearchNewGadgets[ GD_SEARCH_STRINGSEARCH ].CurrentValue == FALSE ) &&
							(-1==TestHexString(GetString(SearchWD.Gadgets[GD_SEARCH_SEARCHSTRING]))))
								ActivateGadget(SearchWD.Gadgets[GD_SEARCH_REPLACESTRING],SearchWD.Wnd,0);
						else
							Select = 1;
						break;

					case GD_SEARCH_REPLACEMODE:
						GT_SetGadgetAttrs( SearchWD.Gadgets[ GD_SEARCH_REPLACESTRING ], SearchWD.Wnd, NULL, GA_Disabled, ((struct MyNewGadget *)gad->UserData)->CurrentValue, TAG_DONE );
					case GD_SEARCH_CASESENSITIV:
					case GD_SEARCH_WILDCARDS:
					case GD_SEARCH_STRINGSEARCH:
						((struct MyNewGadget *)gad->UserData)->CurrentValue = !((struct MyNewGadget *)gad->UserData)->CurrentValue;
						break;

					case GD_SEARCH_NEXT:
						Select = 1;
						break;

					case GD_SEARCH_PREVIOUS:
						Select = 2;
						break;

					case GD_SEARCH_CANCEL:
						Select = 0;
						break;
				}
				break;
		}
	}


	if( Select > 0 )		/* Next/Previous -> Werte merken */
	{
		SetSearchString( GetString( SearchWD.Gadgets[ GD_SEARCH_SEARCHSTRING ] ));
		SetReplaceString( GetString( SearchWD.Gadgets[ GD_SEARCH_REPLACESTRING ] ));

		searchmode &= ~( SM_CASE | SM_STRING | SM_WILDCARDS | SM_REPLACE );

		if( SearchNewGadgets[ GD_SEARCH_CASESENSITIV ].CurrentValue )
			searchmode |= SM_CASE;

		if( SearchNewGadgets[ GD_SEARCH_WILDCARDS ].CurrentValue )
			searchmode |= SM_WILDCARDS;

		if( SearchNewGadgets[ GD_SEARCH_STRINGSEARCH ].CurrentValue )
			searchmode |= SM_STRING;

		if( SearchNewGadgets[ GD_SEARCH_REPLACEMODE ].CurrentValue )
			searchmode |= SM_REPLACE;
	}

	if( Select != -1 )
	{
		MyRemoveSignal( 1L << SearchWD.Wnd->UserPort->mp_SigBit );
		NewCloseAWindow( &SearchWD );
	}
	
	switch( Select )
	{
		case 1:
			SearchNext(0, FALSE, FALSE,AktuDD);
			break;

		case 2:
			SearchNext(BM_BACKWARD, FALSE, FALSE,AktuDD);
			break;
	}
}

void SendMsg( struct Window * w, UBYTE Key )
{
	struct IntuiMessage *Msg;

	if(( Msg = AllocVec( sizeof( struct ExtIntuiMessage ), MEMF_CLEAR )))
	{
		Msg->ExecMessage.mn_Node.ln_Type = NT_MESSAGE;
		Msg->ExecMessage.mn_Length = sizeof( struct IntuiMessage );
		Msg->ExecMessage.mn_ReplyPort = GadgetReplyPort;

		Msg->Class = IDCMP_VANILLAKEY;
		Msg->Code = (UWORD)Key;
		Msg->Qualifier = 0;
		Msg->IAddress = NULL;
		Msg->IDCMPWindow = w;

		PutMsg( w->UserPort, ( struct Message * )Msg );
	}
}

HOOKPROTO(SearchHookFunc, ULONG, struct SGWork *sgw, Msg msg)
{
	struct InputEvent *ie;
	ULONG return_code = ~0;
	static long ShiftSearchLaenge = -1;

	if( msg->MethodID == SGH_KEY )
	{
		ie = sgw->IEvent;

		if( ie->ie_Class == IECLASS_RAWKEY )
		{
			if(( ie->ie_Qualifier & IEQUALIFIER_RCOMMAND ) && ( sgw->EditOp == EO_INSERTCHAR ))
			{
				UBYTE Buffer[10];

				if( 1 == MapRawKey(ie, Buffer, 10, 0))
				{
					SendMsg( sgw->GadgetInfo->gi_Window,Buffer[0] );
					sgw->Actions = 0;
				}
			}
			else
			if( SearchWD.Wnd && (sgw->GadgetInfo->gi_Window == SearchWD.Wnd ))
			{
				struct Node *HAktu;
				struct List *HList;

				if( sgw->Gadget == SearchWD.Gadgets[ GD_SEARCH_SEARCHSTRING ])
				{
					HAktu = SHAktu;
					HList = &SHList;
				}
				else
				{
					HAktu = RHAktu;
					HList = &RHList;
				}

				if( sgw->EditOp != EO_NOOP )
				{
					ShiftSearchLaenge = -1;
				}
				else
				if( ie->ie_Qualifier & ( IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT ))
				{
					struct Node *node;

					if(( ie->ie_Code == CURSORUP ) || ( ie->ie_Code == CURSORDOWN ))
					{
						if( ShiftSearchLaenge == -1 )
							ShiftSearchLaenge = strlen( sgw->WorkBuffer );

						if( !IsListEmpty( HList ))
						{
							if( HAktu )
								node = HAktu;
							else
								node = HList->lh_TailPred;

							if( ie->ie_Code == CURSORUP )
							{
								if( !strcmp( node->ln_Name, sgw->WorkBuffer ))
								{
									node = node->ln_Pred;
								}
							
								if( node != ( struct Node * )HList )
								while( strnicmp( sgw->WorkBuffer, node->ln_Name, ShiftSearchLaenge ))
								{
									node = node->ln_Pred;
			
									if( node == ( struct Node * )HList )
										break;
								}
	
								if( node == ( struct Node * )HList )
								{
									sgw->Actions = SGA_BEEP;
								}
								else
								{
									HAktu = node;
									sgw->WorkBuffer = HAktu->ln_Name;
									sgw->NumChars = strlen( HAktu->ln_Name );
								}
							}
							else
							{
								if( !strcmp( node->ln_Name, sgw->WorkBuffer ))
								{
									node = node->ln_Succ;
								}
							
								if( node != ( struct Node * )&HList->lh_Tail )
								while( strnicmp( sgw->WorkBuffer, node->ln_Name, ShiftSearchLaenge ))
								{
									node = node->ln_Succ;
			
									if( node == ( struct Node * )&HList->lh_Tail )
										break;
								}
	
								if( node == ( struct Node * )&HList->lh_Tail )
								{
									sgw->Actions = SGA_BEEP;
								}
								else
								{
									HAktu = node;
									sgw->WorkBuffer = HAktu->ln_Name;
									sgw->NumChars = strlen( HAktu->ln_Name );
								}
							}
						}
					}
				}
				else
				if( ie->ie_Qualifier & IEQUALIFIER_CONTROL )
				{
					switch( ie->ie_Code )
					{
						case CURSORUP:
							if( !IsListEmpty( HList ))
							{
								HAktu = HList->lh_Head;
								sgw->WorkBuffer = HAktu->ln_Name;
								sgw->NumChars = strlen( HAktu->ln_Name );
							}
							break;
	
						case CURSORDOWN:
							if( !IsListEmpty( HList ))
							{
								HAktu = HList->lh_TailPred;
								sgw->WorkBuffer = HAktu->ln_Name;
								sgw->NumChars = strlen( HAktu->ln_Name );
							}
							break;
					}
				}
				else
				{
					ShiftSearchLaenge = -1;

					switch( ie->ie_Code )
					{
						case CURSORUP:
							if( HAktu == 0 )
							{
								if( !IsListEmpty( HList ))
								{
									HAktu = HList->lh_TailPred;
									sgw->WorkBuffer = HAktu->ln_Name;
									sgw->NumChars = strlen( HAktu->ln_Name );
								}
							}
							else
							if( HAktu->ln_Pred != ( struct Node * )HList )
							{
								HAktu = HAktu->ln_Pred;
								sgw->WorkBuffer = HAktu->ln_Name;
								sgw->NumChars = strlen( HAktu->ln_Name );
							}
							break;
	
						case CURSORDOWN:
							if( HAktu != 0 )
							{
							if( HAktu->ln_Succ != ( struct Node * )&HList->lh_Tail )
							{
								HAktu = HAktu->ln_Succ;
								sgw->WorkBuffer = HAktu->ln_Name;
								sgw->NumChars = strlen( HAktu->ln_Name );
							}
							else
							{
								sgw->WorkBuffer = "";
								sgw->NumChars = 0;
								HAktu = 0;
							}
							}
							break;
					}
				}

				if( sgw->Gadget == SearchWD.Gadgets[ GD_SEARCH_SEARCHSTRING ])
				{
					SHAktu = HAktu;
				}
				else
				{
					RHAktu = HAktu;
				}
			}
			else
			{
					/* Prüfen, ob es sich um ein Gadget zur Hexzahleingabe handelt */

				if( ((struct MyNewGadget *)sgw->Gadget->UserData)->Typ == HEX_KIND )
				{
					if( sgw->EditOp == EO_INSERTCHAR )
					{
						if( !isxdigit( sgw->Code ))
						{
							sgw->Actions = SGA_BEEP;
						}
					}
				}
			}
		}
	}
	else
		return_code = 0;

   return(return_code);
}

MakeHook(SearchHook, SearchHookFunc);

BOOL OpenSearchWindow( BOOL ReplaceMode )
{
	ULONG err;

	if( SearchWD.Wnd )
	{
		ActivateWindow( SearchWD.Wnd );
		return( TRUE );
	}

	if( !IsListEmpty( &SHList ))
	{
	 	SearchNewGadgets[ GD_SEARCH_SEARCHSTRING ].CurrentValue = (IPTR) SHList.lh_TailPred->ln_Name;
		SHAktu = SHList.lh_TailPred;
	}
	else
	 	SearchNewGadgets[ GD_SEARCH_SEARCHSTRING ].CurrentValue = (IPTR) 0;

	if( !IsListEmpty( &RHList ))
	{
		SearchNewGadgets[ GD_SEARCH_REPLACESTRING ].CurrentValue = (IPTR) RHList.lh_TailPred->ln_Name;
		RHAktu = RHList.lh_TailPred;
	}
	else
		SearchNewGadgets[ GD_SEARCH_REPLACESTRING ].CurrentValue = (LONG) 0;

	SearchNewGadgets[ GD_SEARCH_CASESENSITIV ].CurrentValue = ( searchmode & SM_CASE ) ? TRUE : FALSE;
	SearchNewGadgets[ GD_SEARCH_WILDCARDS ].CurrentValue    = ( searchmode & SM_WILDCARDS ) ? TRUE: FALSE;
	SearchNewGadgets[ GD_SEARCH_STRINGSEARCH ].CurrentValue = ( searchmode & SM_STRING ) ? TRUE: FALSE;
	SearchNewGadgets[ GD_SEARCH_REPLACEMODE ].CurrentValue = ReplaceMode;
	SearchNewGadgets[ GD_SEARCH_REPLACESTRING ].Disabled = !ReplaceMode;

	if(( err =NewOpenAWindow( &SearchWD, GetStr( MSG_WINDOWTITLE_SEARCH ))))
	{
		MyRequest( MSG_INFO_GLOBAL_CANTOPENWINDOW, err);
		return( FALSE );
	}
	else
	{
		ActivateGadget( SearchWD.Gadgets[ GD_SEARCH_SEARCHSTRING ], SearchWD.Wnd, 0 );

		MyAddSignal( 1L << SearchWD.Wnd->UserPort->mp_SigBit, &DoSearchWndMsg );
	}
	
	return (TRUE);
}
