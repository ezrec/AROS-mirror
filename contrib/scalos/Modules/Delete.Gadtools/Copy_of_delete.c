#include <string.h>
#include <dos.h>

#include <exec/lists.h>
#include <exec/memory.h>

#include <dos/exall.h>
#include <dos/dostags.h>
#include <intuition/imageclass.h>
#include <workbench/startup.h>
#include <libraries/ctrequest.h>

#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/icon_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/ctrequest_protos.h>
#include <clib/utility_protos.h>

#include <pragmas/exec_sysbase_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/icon_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/gadtools_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/ctrequest_pragmas.h>
#include <pragmas/utility_pragmas.h>

__stdargs void NewList( struct List *list );

IMPORT struct Library *GadToolsBase;
IMPORT struct Library *IntuitionBase;
IMPORT struct Library *SysBase;
IMPORT struct Library *GfxBase;
IMPORT struct Library *DOSBase;
IMPORT struct Library *UtilityBase;

struct Library *IconBase;
struct Library *CTRequestBase;

struct Screen *scr;
struct DrawInfo *drinfo;
APTR visualinfo;
ULONG offx,offy,fonty,sizeheight;
ULONG wndwidth,wndheight,drawerwidth;

struct Window *wnd;
struct Gadget *glist;
BOOL glistAttached;
BOOL fromWB;

const STATIC STRPTR engLocText[] =
{
	"Warning! You cannot get back what you delete!\nOK to delete:",
	"_OK",
	"_Cancel",
	"Delete",
	"file(s) and",
	"drawer(s) (and there contents)?",
	"Delete|Abort",
	"Deleting...",
	"Path",
	"Abort",
	"Building Filelist...",
	"Not Enough Memory",
	"Error",
	"couldn't be deleted",
	"Skip it|Abort",
	"is protected against deletion.\n\nDelete anywhy?",
	"Delete|Skip it"
};

UWORD abort_key;

STRPTR locText[18];

static const STRPTR VerString = "$VER: delete.module 1.0 "__AMIGADATE__;

STATIC BOOL CreateGadgets(void);

//-------------------------------------
// Requester Support

#ifdef __SASC

STATIC VOID SR_LenFunc(VOID)
{
	__emit(0x5293); // ADDQ.L  #1,(A3)
}

STATIC VOID SR_CopyFunc(VOID)
{
	__emit(0x16c0); // MOVE.B  D0,(A3)+
}

STATIC VOID SR_CopyFuncNoUnderscore(VOID)
{
	__emit(0xb03c); __emit(0x005f); // CMP.B		#'_',D0
	__emit(0x6702); // BEQ.B		.end(RTS)
	__emit(0x16c0); // MOVE.B	D0,(A3)+
}

STATIC VOID SR_Dummy(VOID)
{
}

#else

VOID SR_LenFunc(VOID);
VOID SR_CopyFunc(VOID);
VOID SR_CopyFuncNoUnderscore(VOID);
VOID SR_Dummy(VOID);

#endif

//-------------------------------------
LONG SimpleRequestArgs( struct Window *wnd, struct EasyStruct *easy, ULONG *idcmp, STRPTR reqType, STRPTR fallBackType, APTR argList )
{
	struct EasyCTRStruct *ctr;

	// if ctrrequest.library isn't available call the
	// intuituin function instead
	if( !CTRequestBase )
	{
		STRPTR gadgetText = (STRPTR)easy->es_GadgetFormat;
		STRPTR newGadgetText;
		ULONG len;
		APTR nd;

		if( !gadgetText )
		{
			DisplayBeep(NULL);
			return 0;
		}

		// Skip the text format
		if( easy->es_TextFormat ) nd = RawDoFmt(easy->es_TextFormat,argList,(void(*)())SR_Dummy,NULL);
		else nd = argList;

		// Calculate the length of the formatted GadgetText
		len = 0;
		RawDoFmt(easy->es_GadgetFormat,nd,(void(*)())SR_LenFunc,&len);

		newGadgetText = (STRPTR)AllocVec( len+2, 0 );
		if( !newGadgetText ) return EasyRequestArgs(wnd,easy,idcmp,argList);
		else
		{
			LONG val;
			struct EasyStruct es;

			// Format the gadgettext without underscores
			RawDoFmt(easy->es_GadgetFormat,nd,(void(*)())SR_CopyFuncNoUnderscore,newGadgetText);

			es.es_StructSize = sizeof(struct EasyStruct);
			es.es_Flags = easy->es_Flags;
			es.es_Title = easy->es_Title;
			es.es_TextFormat = easy->es_TextFormat;
			es.es_GadgetFormat = newGadgetText;

			val = EasyRequestArgs(wnd,&es,idcmp,argList);
			FreeVec( newGadgetText );
			return val;
		}
	}

	// Alloc an EasyCTRStruct
	ctr = (struct EasyCTRStruct*)AllocVec( sizeof(struct EasyCTRStruct),MEMF_CLEAR);
	if(ctr)
	{
		LONG val;

		// Fill in special things
		ctr->ectr_Title = easy->es_Title;
		ctr->ectr_RequesterType = reqType;
		ctr->ectr_RequesterType_FB =fallBackType;
		ctr->ectr_TimeOutmodes = NULL;//timeOutmodes;
		ctr->ectr_Screen = wnd?wnd->WScreen:NULL;

		// Is any additional Arg given?
		if( !argList )
		{
			// No - take the Body and Gadget strings like they are
			ctr->ectr_Body = easy->es_TextFormat;
			ctr->ectr_Gadgets = easy->es_GadgetFormat;
		}	else
		{
			// Yes
			ULONG len=0;

			// Calculate the length of the formatted body string
			APTR nd = RawDoFmt(easy->es_TextFormat,argList,(void(*)())SR_LenFunc,&len);
			if( len )
			{
				STRPTR body = (STRPTR)AllocVec( len+2, 0 );

				// Perform final text formatting
				if( body ) RawDoFmt(easy->es_TextFormat,argList,(void(*)())SR_CopyFunc,body);
				ctr->ectr_Body = body;
			}

			len=0;

			// Calculate the length of the formatted gadget string
			RawDoFmt(easy->es_GadgetFormat,nd,(void(*)())SR_LenFunc,&len);
			if( len )
			{
				STRPTR gadget = (STRPTR)AllocVec( len+2, 0 );

				// Perform final gadget formatting
				if( gadget ) RawDoFmt(easy->es_GadgetFormat,nd,(void(*)())SR_CopyFunc,gadget);
				ctr->ectr_Gadgets = gadget;
			}
		}

		// Check for IDCMPs
		if( idcmp )
		{
			if( *idcmp & IDCMP_DISKINSERTED ) ctr->ectr_MonitorDiskInserted = TRUE;
			if( *idcmp & IDCMP_DISKREMOVED )  ctr->ectr_MonitorDiskRemoved = TRUE;

			// Clear the *idcmp variable
			*idcmp = 0;
		}

		// And now the nice requester
		val = EasyCTRequest(ctr);
		if( val < -1 )
		{
			DisplayBeep(NULL);
			val = 0;
		}	else
		{
			if( val == -1 && idcmp )
			{
				if( ctr->ectr_MonitorDiskInserted ) *idcmp |= IDCMP_DISKINSERTED;
				if( ctr->ectr_MonitorDiskRemoved ) *idcmp |= IDCMP_DISKREMOVED;
			}
		}

		// ArgList given?
		if( argList )
		{
			// Yes - free the allocated memory
			if( ctr->ectr_Gadgets ) FreeVec(ctr->ectr_Gadgets);
			if( ctr->ectr_Body ) FreeVec(ctr->ectr_Body);
		}

		// Free the EasyCTRStruct
		FreeVec(ctr);
		return val;
	}
}

//-------------------------------------
LONG SimpleRequest( struct Window *wnd, struct EasyStruct *easy, ULONG *idcmp, STRPTR reqType, STRPTR fallBackType, APTR args,... )
{
	return SimpleRequestArgs( wnd, easy, idcmp, reqType, fallBackType, &args );
}
//-------------------------------------

//-------------------------------------
VOID sprintf( STRPTR buf, const STRPTR fmt,...)
{
	RawDoFmt((const STRPTR)fmt,(const STRPTR*)&fmt+1,(void(*)())SR_CopyFunc,buf);
}

//-------------------------------------
ULONG StrLen( const STRPTR str )
{
	if( !str) return 0;
	return strlen(str);
}
//-------------------------------------
STRPTR StrCopy( const STRPTR str )
{
	STRPTR dst;
	if( !str ) return NULL;
	if( !*str) return NULL;

	dst = (STRPTR)AllocVec(strlen(str)+1,0);
	if(dst) strcpy(dst,str);
	return dst;
}
//-------------------------------------
STRPTR NameOfLock( BPTR lock )
{
	STRPTR n;
	BOOL again;
	ULONG bufSize = 127;
	if( !lock ) return NULL;

	do
	{
		again = FALSE;
		if((n = (STRPTR)AllocVec(bufSize, 0x10000 )))
		{
			if( NameFromLock( lock, n, bufSize-1 ) == DOSFALSE )
			{
				if( IoErr() == ERROR_LINE_TOO_LONG )
				{
					bufSize += 127;
					again = TRUE;
				}
				FreeVec(n);
				n = NULL;
			}
		}
	}	while(again);

	return n;
}
//-------------------------------------
STRPTR WBArg2String( struct WBArg *arg, LONG numArgs )
{
	struct WBArgNode
	{
		struct MinNode node;
		STRPTR dirname;
		STRPTR filename;
	};

	struct MinList list;
	ULONG len=0;
	STRPTR buf;
	UWORD i;

	NewList((struct List*)&list );

	for( i=0; i<numArgs; i++)
	{
		struct WBArgNode *node = (struct WBArgNode*)AllocVec(sizeof(struct WBArgNode),0x10000);
		if(node)
		{
			STRPTR n = NameOfLock(arg[i].wa_Lock);
			node->dirname = n;
			node->filename = arg[i].wa_Name;

			len += StrLen( n )+StrLen(arg[i].wa_Name)+8;
			AddTail((struct List*)&list, (struct Node*)node );
		}
	}

	buf = (STRPTR)AllocVec(len+2,0x10000);
	if(buf)
	{
		struct WBArgNode *node;
		STRPTR bufPtr = buf;

		while(( node = (struct WBArgNode*)RemHead((struct List*)&list)))
		{
			STRPTR n = node->dirname;
			if( !n ) continue;

			if( n[0] ) strcpy(bufPtr,n );
			AddPart(bufPtr,node->filename,len-(bufPtr-buf));

			bufPtr+=strlen(bufPtr);
			if(list.mlh_Head->mln_Succ)
				*bufPtr++=' ';

			FreeVec(n);
			FreeVec(node);
		}
		*bufPtr = 0;
	}
	return buf;
}
//-------------------------------------
STRPTR GetFullPath( const STRPTR drw, const STRPTR file )
{
	ULONG length = StrLen(drw)+StrLen(file)+4;
	STRPTR fp = (STRPTR)AllocVec( length+1,0 );
	if( fp )
	{
		strcpy( fp, drw );

		if( AddPart( fp, file, length ))	return fp;
		FreeVec( fp );
	}
	return NULL;
}
//-------------------------------------
STRPTR BuildIconName( const STRPTR name )
{
	ULONG namelen=strlen(name);
	STRPTR infoname = AllocVec(namelen+8,0);
	if(infoname)
	{
		strcpy(infoname,name);
		strcpy(infoname+namelen,".info");
	}
	return infoname;
}
//-------------------------------------

ULONG deleteAllProt;

STRPTR delprot_body = "is protected against deletion.\n\nDelete anyway?";
STRPTR couldnt_body = "couldn't be deleted:";

//-------------------------------------
// Readsupport
struct DirList
{
	struct MinList list;
	BPTR lock;
	ULONG read;
};

struct DirNode
{
	struct MinNode node;
	STRPTR name;
	struct DirList list;
	struct DirList *parentList;
	ULONG icon;
};

struct FileNode
{
	struct MinNode node;
	STRPTR name;
	ULONG nameLen;
	ULONG icon;
};
	
//-------------------------------------
ULONG PrintError( const ULONG ioerr, const STRPTR name, const STRPTR button )
{
	struct EasyStruct easy;
	char buffer[80];

	easy.es_StructSize = sizeof(struct EasyStruct);
	easy.es_Flags = 0;
	easy.es_Title = "Scalos";
	easy.es_TextFormat = "%s %s\n%s";
	easy.es_GadgetFormat = "%s";

	Fault( ioerr, NULL, buffer,80);

	return (ULONG)SimpleRequest( NULL, &easy, NULL, ECTR_TYPE_WARNING, NULL, name, couldnt_body, buffer,button);
}
//-------------------------------------
BOOL IsDeletable( const STRPTR name, ULONG prot)
{
	if(( prot & FIBF_DELETE ))
	{
		BOOL del=deleteAllProt;
		if( !deleteAllProt)
		{
			struct EasyStruct easy;
			easy.es_StructSize = sizeof(struct EasyStruct);
			easy.es_Flags = 0;
			easy.es_Title = "Scalos";
			easy.es_TextFormat = "%s\n%s";
			easy.es_GadgetFormat = "%s";

			switch( SimpleRequest( NULL, &easy, NULL, ECTR_TYPE_WARNING, NULL, name, delprot_body, "Delete|DeleteAllProtect|Skip"))
			{
				case	0:
							break;

				case	1:
							del=TRUE;
							break;

				case	2:
							del=deleteAllProt=TRUE;
							break;
			}
		}
		return del;
	}
	return TRUE;
}

//-------------------------------------
ULONG Unprotect( const STRPTR name, const ULONG icon, ULONG prot)
{
	if(( prot & FIBF_DELETE ))
	{
		BOOL ok;
		if(SetProtection(name,NULL) != DOSFALSE ) ok=TRUE;
		else
		{
			LONG ioerr = IoErr();
			if( ioerr == ERROR_OBJECT_NOT_FOUND ) ok = TRUE;
			else
			{
				ok = FALSE;
				return PrintError(ioerr,name,"Skip|Abort");
			}
		}

		if(ok && icon )
		{
			if( icon )
			{
				STRPTR infoname = BuildIconName(name);
				if(infoname)
				{
					if(SetProtection(infoname,NULL)== DOSFALSE )
					{
						LONG ioerr = IoErr();
						if( ioerr == ERROR_OBJECT_NOT_FOUND ) ok = TRUE;
						else return PrintError(ioerr,name,"Skip|Abort");
					}

					FreeVec(infoname);
				}
			}
		}
	}
	return 2;
}
//-------------------------------------
BOOL DeleteTheFile( const STRPTR name, const ULONG icon )
{
	BOOL goon = TRUE;
	struct TextExtent te;
	ULONG len = strlen(name);
	ULONG nc;
	LONG ioerr;
	BOOL onlyicon = FALSE;

	struct IntuiMessage *imsg;

	Move(wnd->RPort,offx+2,offy+drinfo->dri_Font->tf_Baseline+2+fonty+1);
	nc = TextFit(wnd->RPort,(const STRPTR)name+len,len,&te,NULL,-1,wndwidth-2*offx-4,fonty);
	Text(wnd->RPort,(const STRPTR)name+len-nc,nc);

	if( te.te_Width < drawerwidth )
	{
		EraseRect(wnd->RPort,wnd->RPort->cp_x,offy+3+fonty,offx+2+drawerwidth,offy+2+2*fonty);
	}

	if(wnd)
	{
		while((imsg = (struct IntuiMessage*)GT_GetIMsg(wnd->UserPort)))
		{
			ULONG cl = imsg->Class;

			if( cl == IDCMP_SIZEVERIFY )
			{
				if(glistAttached) RemoveGList( wnd, glist, -1 );
				glistAttached = FALSE;
				GT_ReplyIMsg(imsg);
				continue;
			}

			GT_ReplyIMsg(imsg);

			switch(cl)
			{
				case	IDCMP_GADGETUP:
				case	IDCMP_CLOSEWINDOW:
							return FALSE;
							break;

				case	IDCMP_NEWSIZE:
							if(glistAttached) RemoveGList( wnd, glist, -1 );
							wndwidth = wnd->Width;
							wndheight = wnd->Height;
							EraseRect( wnd->RPort, wnd->BorderLeft, wnd->BorderTop+fonty+3,
																			wnd->Width-wnd->BorderRight-1, wnd->Height-wnd->BorderBottom-1 );
							if(CreateGadgets())
							{
								AddGList( wnd, glist, -1,-1, NULL );
								RefreshGList( glist, wnd, NULL, -1 );
								glistAttached = TRUE;
								SetFont(wnd->RPort,drinfo->dri_Font);
							}
							break;
			}
		}
	}

	drawerwidth = te.te_Width;

	if(DeleteFile(name)==DOSFALSE)
	{
		ioerr = IoErr();
		if( ioerr == ERROR_OBJECT_NOT_FOUND)
		{
			if(!DeleteDiskObject(name))
			{
				ioerr=IoErr();
				if(ioerr==ERROR_OBJECT_NOT_FOUND) ioerr=0;
			}
			else ioerr=0;
			onlyicon=TRUE;
		}
	}	else ioerr=0;

	if( ioerr )
	{
		switch( ioerr)
		{
			case	ERROR_DELETE_PROTECTED:
						{
							BOOL del=deleteAllProt;
							if( !deleteAllProt)
							{
								struct EasyStruct easy;

								easy.es_StructSize = sizeof(struct EasyStruct);
								easy.es_Flags = 0;
								easy.es_Title = "Scalos";
								easy.es_TextFormat = "%s\n%s";
								easy.es_GadgetFormat = "%s";

								switch( SimpleRequest( NULL, &easy, NULL, ECTR_TYPE_WARNING, NULL, name, delprot_body, "Delete|DeleteAllProtect|Skip"))
								{
									case	0:
												del=FALSE;
												break;

									case	1:
												del=TRUE;
												break;

									case	2:
												del=deleteAllProt=TRUE;
												break;
								}
								
							}
							if(del)
							{
								BOOL ok = onlyicon;
								if( !ok ) if(SetProtection(name,NULL) != DOSFALSE ) ok=TRUE;

								if(ok)//nlyicon || SetProtection(name,NULL) != DOSFALSE )
								{
									BOOL error;

									if( icon || onlyicon )
									{
										ULONG namelen=strlen(name);
										STRPTR infoname = AllocVec(namelen+8,0);
										error = TRUE;
										if(infoname)
										{
											strcpy(infoname,name);
											strcpy(infoname+namelen,".info");

											if(SetProtection(infoname,NULL) != DOSFALSE )
												error = FALSE;

											FreeVec(infoname);
										}	else SetIoErr(103);
									}	else error=FALSE;

									if( !error )
									{
										if( onlyicon )
										{
											if(DeleteDiskObject(name)) return TRUE;
										}	else
										{
											if(DeleteFile(name) != DOSFALSE )
											{
												if(icon)
												{
													if(DeleteDiskObject(name)) return TRUE;
												}	else return TRUE;
											}
										}
									}
								}
								ioerr = IoErr();
							} else break;
						}

			default:
/*						{
							struct EasyStruct easy;
							char buffer[80];

							easy.es_StructSize = sizeof(struct EasyStruct);
							easy.es_Flags = 0;
							easy.es_Title = "Scalos";
							easy.es_TextFormat = "%s %s\n%s";
							easy.es_GadgetFormat = "%s";

							Fault( ioerr, NULL, buffer,80);

							return (BOOL)SimpleRequest( NULL, &easy, NULL, ECTR_TYPE_WARNING, NULL, name, couldnt_body, buffer,"Skip|Abort");
						}*/
						return (BOOL)PrintError(ioerr,name,"Skip|Abort");
						break;
		}
	}	else
	{
		if( icon )
		{
			DeleteDiskObject(name);
		}
	}

	return goon;
}
//-------------------------------------
BOOL ReadInDir( struct DirList *list, APTR dirPool )
{
	BOOL retval = TRUE;
	STRPTR lockName;

	if( !list->lock )
	{
		Printf("No Lock!\n");
		return FALSE;
	}

	if( list->read )
	{
		PutStr("Already read!\n");
		return TRUE;
	}

	lockName = NameOfLock(list->lock);
	if( lockName )
	{
		ULONG lockSize = strlen(lockName);
		APTR filePool = CreatePool( 0,4096,4096);
		if( filePool )
		{
			struct ExAllControl *eac;
			eac = (struct ExAllControl*)AllocDosObject(DOS_EXALLCONTROL,NULL);
			if( eac )
			{
				struct ExAllData *ead;
				ead = (struct ExAllData*)AllocVec( 1032, 0x10000 );
				if(ead)
				{
					LONG more;
					struct MinList fileList;
					NewList((struct List*)&fileList);

					eac->eac_LastKey = 0;
					eac->eac_MatchString = NULL;
		
					list->read = TRUE;
		
					do
					{
						struct ExAllData *ed = ead;
						more = ExAll( list->lock, ed, 1024, ED_TYPE, eac );
						if((!more) &&  (IoErr() != ERROR_NO_MORE_ENTRIES ))
						{
							retval = FALSE;
							break;
						}
		
						if( eac->eac_Entries )
						{
							do
							{
								ULONG nlen = strlen(ed->ed_Name);
								ULONG len = nlen+8+lockSize;
								if( ed->ed_Type > 0 )
								{
									struct DirNode *dir = (struct DirNode*)AllocPooled(dirPool,sizeof(struct DirNode));
									if( dir )
									{
										STRPTR name = (STRPTR)AllocPooled(dirPool,len);
										if(name)
										{
											strcpy(name,lockName);
											AddPart(name,ed->ed_Name,len);
	
											dir->name = name;
											dir->list.lock = Lock( name, ACCESS_READ);
											dir->list.read = FALSE;
											dir->parentList = list;
											dir->icon = FALSE;
											NewList((struct List*)&dir->list);
				
											AddTail((struct List*)list, (struct Node*)dir);
										}
									}
								}	else
								{
									struct FileNode *file = (struct FileNode*)AllocPooled(filePool,sizeof(struct FileNode));
									if( file )
									{
										STRPTR name = (STRPTR)AllocPooled(filePool,len);
										if( name )
										{
											strcpy(name,lockName);
											AddPart(name,ed->ed_Name,len);

											file->name = name;
											file->nameLen = strlen(name);
											file->icon = FALSE;
											AddTail((struct List*)&fileList, (struct Node*)file);
										}
									}
								}
								ed = ed->ed_Next;
							} while(ed);
						}
					} while(more);
		
					FreeVec(ead);

					// Icons aussortieren
					if( !IsListEmpty((struct List*)&fileList))
					{
						struct FileNode *node;
						node = (struct FileNode*)fileList.mlh_Head;
						while(node->node.mln_Succ && retval)
						{
							if( node->nameLen > 4 )
							{
								if( !Stricmp(".info",node->name+node->nameLen-5))
								{
									struct FileNode *fnode = (struct FileNode*)fileList.mlh_Head;
									BOOL iconfound=FALSE;
									node->name[node->nameLen-5]=0;	
									while(fnode->node.mln_Succ)
									{
										if( fnode != node )
										{
											if( !Stricmp(node->name,fnode->name))
											{
												fnode->icon = iconfound = TRUE;
												break;
											}
										}
										fnode = (struct FileNode*)fnode->node.mln_Succ;
									}

									if( !iconfound )
									{
										struct DirNode *dnode = (struct DirNode*)list->list.mlh_Head;
										while(dnode->node.mln_Succ)
										{
											if( !Stricmp(node->name,dnode->name))
											{
												dnode->icon = iconfound = TRUE;
												break;
											}

											dnode = (struct DirNode*)dnode->node.mln_Succ;
										}
									}

									if( iconfound )
									{
										struct FileNode *tfn = node;
										node = (struct FileNode*)node->node.mln_Succ;
										Remove((struct Node*)tfn);
										continue;
									}
								}
							}
							node = (struct FileNode*)node->node.mln_Succ;
						}

						node = (struct FileNode*)fileList.mlh_Head;
						while(node->node.mln_Succ && retval)
						{
							retval = DeleteTheFile(node->name,node->icon);
							node = (struct FileNode*)node->node.mln_Succ;
						}
					}
				}
		
				FreeDosObject(DOS_EXALLCONTROL,eac);
			}
			DeletePool(filePool);
		}
		FreeVec(lockName);
	}
	return retval;
}
//-------------------------------------
BOOL DeleteDir( struct DirList *root, APTR dirPool )
{
	struct DirList *cl = root;
	BOOL goon = TRUE;
	while(cl)
	{
//		Printf("Readdir: %s  0x%lx\n",NameOfLock(cl->lock),cl);
		if(goon)
		{
			if(!ReadInDir(cl,dirPool)) goon=FALSE;
		}

		if( !IsListEmpty((struct List*)cl))
		{
			struct DirNode *cn = (struct DirNode*)cl->list.mlh_Head;
			cl = (struct DirList*)&cn->list;
		}	else
		{
			if(cl != root )
			{
				struct DirNode *cn = (struct DirNode*)(((ULONG*)cl)-3);
				struct DirNode *ncn = (struct DirNode*)cn->node.mln_Succ;

				if( !ncn->node.mln_Succ )
				{
					do
					{
						if( cl == root ) return TRUE;

						cl = cn->parentList;
						if( !cl )
						{
							PutStr("Ready...\n");
							return TRUE;
						}

						if( IsListEmpty((struct List*)cl))
						{
							PutStr("Error!!\n");
							return FALSE;
						}

						ncn = (struct DirNode*)RemHead((struct List*)cl);
						cn = (struct DirNode*)(((ULONG*)cl)-3);

						UnLock(ncn->list.lock);
						ncn->list.lock = NULL;
						if( goon )
						{
							goon = DeleteTheFile(ncn->name,ncn->icon);
/*							{
								cl = NULL;
								break;
							}*/
						}
					}	while( IsListEmpty((struct List*)cl));
					if( !cl ) break;

					ncn = (struct DirNode*)cl->list.mlh_Head;
				}	else
				{
					Remove((struct Node*)cn);
					UnLock(cn->list.lock);

					if( goon ) goon=DeleteTheFile(cn->name,cn->icon);// break;
				}

				cl = (struct DirList*)&ncn->list;
			}	else return FALSE;
		}
	}
	return FALSE;
}
//-------------------------------------

//-------------------------------------
STATIC BOOL SetupScreen(void)
{
	if((scr = LockPubScreen(NULL)))
	{
		if((drinfo = GetScreenDrawInfo(scr)))
		{
			if((visualinfo = GetVisualInfoA(scr,NULL)))
			{
				Object *o = (Object*)NewObject( NULL, SYSICLASS,
															SYSIA_DrawInfo,drinfo,
															SYSIA_Which, SIZEIMAGE,
															TAG_DONE);
				if(o)
				{
					if(!GetAttr( IA_Height,o,&sizeheight)) sizeheight = 11;
					DisposeObject(o);
				}	else sizeheight = 11;
				return TRUE;
			}
		}
	}
	return FALSE;
}
//-------------------------------------
STATIC VOID FreeScreen(void)
{
	if( visualinfo ) FreeVisualInfo(visualinfo);
	if( drinfo ) FreeScreenDrawInfo(scr,drinfo);
	if( scr ) UnlockPubScreen(NULL,scr);
}
//-------------------------------------
STATIC VOID Render(void)
{
	Move(wnd->RPort,offx+2,offy+drinfo->dri_Font->tf_Baseline+2);
	SetABPenDrMd(wnd->RPort,drinfo->dri_Pens[TEXTPEN],drinfo->dri_Pens[BACKGROUNDPEN],JAM2);
	Text(wnd->RPort,"Deleting...",11);
}
//-------------------------------------
STATIC BOOL CreateGadgets(void)
{
	struct Gadget *g;

	if(glist)
	{
		FreeGadgets(glist);
		glist = NULL;
	}

	g = CreateContext( &glist );
	if( g )
	{
		struct NewGadget ng;

		ng.ng_VisualInfo = visualinfo;
		ng.ng_TextAttr = NULL;
		ng.ng_GadgetText = "_Abort";
		ng.ng_GadgetID = 1;
		ng.ng_Flags = PLACETEXT_IN;
		ng.ng_UserData = NULL;
		ng.ng_LeftEdge = offx+2;
		ng.ng_TopEdge = offy+2*fonty+4;
		ng.ng_Width = wndwidth-2*ng.ng_LeftEdge;
		ng.ng_Height = fonty+6;

		g = CreateGadget( BUTTON_KIND, g, &ng,
														GT_Underscore,'_',
														TAG_DONE);
		if(g) return TRUE;

		FreeGadgets(glist);
		glist=NULL;
	}
	return FALSE;
}
//-------------------------------------
STATIC VOID FreeGUI(void)
{
	if(wnd) CloseWindow(wnd);
	if(glist) FreeGadgets(glist);
}
//-------------------------------------
STATIC BOOL SetupGUI(void)
{
	fonty = drinfo->dri_Font->tf_YSize;
	offx = scr->WBorLeft;
	offy = scr->WBorTop + fonty + 1;

	wndwidth = 300;
	wndheight = offy + 2*fonty + sizeheight + 20;

	if( CreateGadgets())
	{
		wnd = OpenWindowTags( NULL,
								WA_Left,(scr->Width-wndwidth)/2,
								WA_Top,(scr->Height-wndheight)/2,
								WA_Width, wndwidth,
								WA_Height, wndheight,
								WA_IDCMP, IDCMP_CLOSEWINDOW|IDCMP_NEWSIZE|BUTTONIDCMP|IDCMP_SIZEVERIFY|IDCMP_VANILLAKEY,
								WA_DragBar,TRUE,
								WA_SizeGadget, TRUE,
								WA_DepthGadget,TRUE,
								WA_CloseGadget, TRUE,
								WA_Title,"Delete",
								WA_RMBTrap, TRUE,
								WA_Gadgets, glist,
								WA_SizeBBottom,TRUE,
								WA_MaxWidth,-1,
								WA_Activate, TRUE,
								WA_PubScreen,scr,
								WA_NoCareRefresh,TRUE,
								TAG_DONE );
		if( wnd )
		{
			glistAttached = TRUE;
			SetFont(wnd->RPort,drinfo->dri_Font);
			Render();
			return TRUE;
		}
		glistAttached = FALSE;
	}
	FreeGUI();
	return FALSE;
}
//-------------------------------------
/*STATIC BOOL HandleWnd(void)
{
	struct IntuiMessage *imsg;
	BOOL retVal = FALSE;

	while((imsg = GT_GetIMsg(wnd->UserPort)))
	{
		ULONG cl = imsg->Class;
		UWORD code = imsg->Code;
		APTR iaddress = imsg->IAddress;

		if( cl == IDCMP_SIZEVERIFY )
		{
			if(glistAttached) RemoveGList( wnd, glist, -1 );
			glistAttached = FALSE;
			GT_ReplyIMsg(imsg);
			continue;
		}

		GT_ReplyIMsg(imsg);

		switch( cl )
		{
			case	IDCMP_CLOSEWINDOW:
						retVal = TRUE;
						break;

			case	IDCMP_VANILLAKEY:
						if( code == 10 ) retVal = TRUE;
						else if( code == 27 )
						{
							retVal = TRUE;
						}
						break;

			case	IDCMP_NEWSIZE:
						if(glistAttached) RemoveGList( wnd, glist, -1 );
						wndwidth = wnd->Width;
						wndheight = wnd->Height;
						EraseRect( wnd->RPort, wnd->BorderLeft, wnd->BorderTop+fonty+3,
																		wnd->Width-wnd->BorderRight-1, wnd->Height-wnd->BorderBottom-1 );
						if(CreateGadgets())
						{
							AddGList( wnd, glist, -1,-1, NULL );
							RefreshGList( glist, wnd, NULL, -1 );
							glistAttached = TRUE;
							SetFont(wnd->RPort,drinfo->dri_Font);
						}	else 
						{
							glistAttached = FALSE;
							retVal = TRUE;
						}
						break;

			case	IDCMP_GADGETUP:
						switch( ((struct Gadget*)iaddress)->GadgetID )
						{
							default:
										break;
						}
						break;
		}
	}
	return retVal;
}
//-------------------------------------
*/
STRPTR warning_body = "Warning! You cannot get back what you delete!\nOK to delete:";
STRPTR files_body = "file(s) and";
STRPTR drawers_body = "drawer(s) (and there contents)?";

//-------------------------------------
void main(void)
{
/*	if(SetupScreen())
	{
		if( SetupGUI())
		{
			BOOL ready = FALSE;
			ULONG wndsig = 1UL<<wnd->UserPort->mp_SigBit;
			ULONG appsig;

			if( appMsgPort ) appsig = 1UL<<appMsgPort->mp_SigBit;
			else appsig = 0UL;

			ActivateGadget(commandString,wnd,NULL);

			while( ready == FALSE )
			{
				ULONG signal = Wait(wndsig|appsig|4096);

				if( signal & 4096 ) ready = TRUE;
				if( signal & wndsig ) ready = HandleWnd();
				if( signal & appsig ) HandleApp();
			}
			FreeGUI();

			if( commandStr )
			{
				STRPTR conName = BuildConName("Scalos Output Window",scr);
				if( conName )
				{
					SystemTags( commandStr,
							SYS_Asynch, TRUE,
							SYS_Output, Open(conName,MODE_OLDFILE),
							SYS_Input, NULL,
							SYS_UserShell, TRUE,
							stackNum>=4096?NP_StackSize:TAG_IGNORE,stackNum,
							TAG_DONE);

					FreeVec(conName);
				}
				SetVar("Scalos_last_cmd",commandStr,strlen(commandStr),GVF_GLOBAL_ONLY);
				FreeVec(commandStr);
				commandStr = NULL;
			}
		}
		FreeScreen();
	}

	if(WorkbenchBase)CloseLibrary(WorkbenchBase);*/
}
//-------------------------------------
void wbmain(struct WBStartup *wbs )
{
	BOOL replied=FALSE;
	if( wbs->sm_NumArgs > 1 )
	{
		if((IconBase = OpenLibrary("icon.library",37)))
		{
			LONG numArgs=wbs->sm_NumArgs;
			struct WBArg *wbarg;

			main();

			wbarg = (struct WBArg*)AllocVec((numArgs+2)*sizeof(struct WBArg),0x10000);
			if( wbarg )
			{
				ULONG num_files=0;
				ULONG num_drws=0;
				ULONG i;
				BOOL error=FALSE;

				for( i=1; i<numArgs; i++)
				{
					if(!(wbarg[i-1].wa_Lock = /*((Lock("RAM:Unnamed1",ACCESS_READ)))/ */DupLock(wbs->sm_ArgList[i].wa_Lock)))
					{
						error = TRUE;
						break;
					}	else
					{
						if(wbs->sm_ArgList[i].wa_Name)
						{
							if(*wbs->sm_ArgList[i].wa_Name)
							{
								if(!(wbarg[i-1].wa_Name = StrCopy(wbs->sm_ArgList[i].wa_Name)))
								{
									error = TRUE;
									break;
								}
								num_files++;
							}
							else num_drws++;
						}
						else num_drws++;
					}
				}

				if( !error )
				{
					ReplyMsg((struct Message*)wbs);
					wbs=NULL;
					replied=TRUE;
					numArgs--;

					if( SetupScreen())
					{
						struct EasyStruct easy;
				
						easy.es_StructSize = sizeof(struct EasyStruct);
						easy.es_Flags = 0;
						easy.es_Title = "Scalos";
						easy.es_TextFormat = "%s\n%ld %s %ld %s";
						easy.es_GadgetFormat = "%s";
			
						if( SimpleRequest( NULL, &easy, NULL, ECTR_TYPE_WARNING, NULL, warning_body, num_files, files_body, num_drws, drawers_body, "OK|Cancel"))
						{
							if(SetupGUI())
							{
								BOOL ready = FALSE;
								for( i=0; i<numArgs && !ready; i++)
								{
									BOOL drawer = TRUE;

									if(wbarg[i].wa_Name)
									{
										if(*wbarg[i].wa_Name) drawer = FALSE;
									}
				
									if( drawer )
									{
										APTR dirPool = CreatePool(NULL,4096,4096);
										if( dirPool )
										{
											struct DirList list;
											STRPTR name=NameOfLock(wbarg[i].wa_Lock);
											if(name)
											{
												struct FileInfoBlock *fib = (struct FileInfoBlock*)AllocDosObject(DOS_FIB,NULL);
												if(fib)
												{
													if(Examine( wbarg[i].wa_Lock,fib)!=DOSFALSE)
													{
														if(IsDeletable(name,fib->fib_Protection))
														{
															NewList((struct List*)&list);
								
															list.lock = wbarg[i].wa_Lock;//Lock("RAM:",ACCESS_READ);
															list.read = FALSE;

															DeleteDir(&list,dirPool);

															UnLock(wbarg[i].wa_Lock);
															wbarg[i].wa_Lock=NULL;

															switch(Unprotect(name,TRUE,fib->fib_Protection))
															{
																case 0: ready=TRUE;break;
																case 2: if(!DeleteTheFile(name,TRUE))ready=TRUE;
															}
														}
													}	else PrintError(IoErr(),name,"OK");
												}
												FreeVec(name);
											}
											DeletePool(dirPool);
										}
									}	else
									{
										STRPTR dname=NameOfLock(wbarg[i].wa_Lock);
										if(dname)
										{
											ULONG len=strlen(dname)+8+strlen(wbarg[i].wa_Name);
											STRPTR name = (STRPTR)AllocVec(len,0);
											if(name)
											{
												strcpy(name,dname);
												AddPart(name,wbarg[i].wa_Name,len);
												if(!DeleteTheFile(name,TRUE))
												{
													ready=TRUE;
												}
											}
										}
									}
								}
								FreeGUI();
							}
						}

						FreeScreen();
					}
				}

				for( i=0; i<numArgs; i++)
				{
					if(wbarg[i].wa_Lock) UnLock(wbarg[i].wa_Lock);
					if(wbarg[i].wa_Name) FreeVec(wbarg[i].wa_Name);
				}
				FreeVec(wbarg);
			}
			CloseLibrary(IconBase);
		}
	}
	return replied;
}
//-------------------------------------
