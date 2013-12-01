#include <string.h>
#include <dos.h>

#include <exec/lists.h>
#include <exec/memory.h>

#include <dos/exall.h>
#include <dos/dostags.h>
#include <intuition/imageclass.h>
#include <workbench/startup.h>
#include <scalos/scalos.h>

#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/icon_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/utility_protos.h>
#include <clib/locale_protos.h>
//#include <clib/scalos_protos.h>

#include <pragmas/exec_sysbase_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/icon_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/gadtools_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/utility_pragmas.h>
#include <pragmas/locale_pragmas.h>
//#include <pragmas/scalos_pragmas.h>

__stdargs void NewList( struct List *list );

const STRPTR VerString = "$VER: delete.module 1.0 "__AMIGADATE__;

#define	ECTR_TYPE_WARNING	0

IMPORT struct Library *GadToolsBase;
IMPORT struct Library *IntuitionBase;
IMPORT struct Library *SysBase;
IMPORT struct Library *GfxBase;
IMPORT struct Library *DOSBase;
IMPORT struct Library *UtilityBase;
IMPORT struct Library *LocaleBase;

struct Library *IconBase;
//struct Library *ScalosBase;

struct Catalog *Cat;

struct Screen *scr;
struct DrawInfo *drinfo;
APTR visualinfo;
ULONG offx,offy,fonty,sizeheight;
ULONG wndwidth,wndheight,drawerwidth;

struct Window *wnd;
struct Gadget *glist;

BOOL glistAttached;
BOOL sizeVerify;
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
	"_Abort",
	"Building Filelist...",
	"Not Enough Memory",
	"Error",
	"couldn't be deleted",
	"Skip it|Abort",
	"is protected against deletion.\n\nDelete anywhy?",
	"Delete|Skip it",
	"Delete|DeleteAllProtect|Skip"
};

UWORD abort_key;

STRPTR locText[18];

static void wbmain(struct WBStartup *wbs );
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

//-------------------------------------
LONG SimpleRequest( struct Window *wnd, struct EasyStruct *easy, ULONG *idcmp, STRPTR reqType, STRPTR fallBackType, APTR args,... )
{
	return SimpleRequestArgs( wnd, easy, idcmp, reqType, fallBackType, &args );
}
//-------------------------------------

//-------------------------------------
VOID sprintf( STRPTR buf, STRPTR fmt,...)
{
	RawDoFmt(fmt,(&fmt+1),(void(*)())SR_CopyFunc,buf);
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
ULONG FindUnderscoredToLower(STRPTR text)
{
	ULONG c;
	while(( c = *text++))
	{
		if( c == '_' ) return ToLower(*text);
	}
	return 0;
}
//-------------------------------------

ULONG deleteAllProt;

//-------------------------------------
STATIC VOID InitStrings(void)
{
	ULONG i;

	if( LocaleBase )
	{
		for(i=0; i<18; i++)
		{
			locText[i] = GetCatalogStr( Cat, i, engLocText[i]);
		}
	}	else
	{
		for(i=0; i<18; i++)
		{
			locText[i] = engLocText[i];
		}
	}

	abort_key = FindUnderscoredToLower(locText[9]);
}
//-------------------------------------

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
	ULONG prot;
};

struct FileNode
{
	struct MinNode node;
	STRPTR name;
	ULONG nameLen;
	ULONG icon;
};
	
//-------------------------------------
STATIC ULONG PrintError( const ULONG ioerr, const STRPTR name, const STRPTR button )
{
	struct EasyStruct easy;
	char buffer[80];

	easy.es_StructSize = sizeof(struct EasyStruct);
	easy.es_Flags = 0;
	easy.es_Title = (STRPTR) "Scalos";
	easy.es_TextFormat = (STRPTR) "%s %s\n%s";
	easy.es_GadgetFormat = (STRPTR) "%s";

	Fault( ioerr, NULL, buffer,80);

	return (ULONG)SimpleRequest( NULL, &easy, NULL, ECTR_TYPE_WARNING, NULL, name, locText[13], buffer,button);
}
//-------------------------------------
STATIC BOOL IsDeletable( const STRPTR name, ULONG prot)
{
	if(( prot & FIBF_DELETE ))
	{
		BOOL del=deleteAllProt;
		if( !deleteAllProt)
		{
			struct EasyStruct easy;
			easy.es_StructSize = sizeof(struct EasyStruct);
			easy.es_Flags = 0;
			easy.es_Title = (STRPTR) "Scalos";
			easy.es_TextFormat = (STRPTR) "%s\n%s";
			easy.es_GadgetFormat = (STRPTR) "%s";

			switch( SimpleRequest( NULL, &easy, NULL, ECTR_TYPE_WARNING, NULL, name, locText[15], locText[17]))
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
STATIC ULONG Unprotect( const STRPTR name, const ULONG icon, ULONG prot)
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
//				ok = FALSE;
				return PrintError(ioerr,name, locText[14]);
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
						if( ioerr == ERROR_OBJECT_NOT_FOUND ) /* ok = TRUE */;
						else return PrintError(ioerr,name, locText[14]);
					}

					FreeVec(infoname);
				}
			}
		}
	}
	return 2;
}
//-------------------------------------
STATIC BOOL DeleteIcon(const STRPTR name, const ULONG update )
{
	if(update) return DeleteDiskObject(name);
	else
	{
		STRPTR iname = BuildIconName(name);
		if(iname)
		{
			if(DeleteFile(iname)==DOSFALSE) return FALSE;
			else return TRUE;

			FreeVec(iname);
		}
	}
}
//-------------------------------------
STATIC BOOL DeleteTheFile( const STRPTR name, const ULONG icon, const ULONG update )
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
							if( !sizeVerify && glistAttached)
							{
								if( wndwidth == wnd->Width || wndheight != wnd->Height ) break;
							}

							if(glistAttached) RemoveGList( wnd, glist, -1 );

							if( wnd->Width < wndwidth ) RefreshWindowFrame(wnd);

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
			if(!DeleteIcon(name,update))
			{
				ioerr=IoErr();
				if(ioerr==ERROR_OBJECT_NOT_FOUND) ioerr=0;
			}
			else ioerr=0;
			onlyicon=TRUE;
		}	else
		{
			if( ioerr == ERROR_OBJECT_IN_USE )
			{
				if(DeleteFile(name)==DOSFALSE) ioerr = IoErr();
				else ioerr = 0;
			}
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
								easy.es_Title = (STRPTR) "Scalos";
								easy.es_TextFormat = (STRPTR) "%s\n%s";
								easy.es_GadgetFormat = (STRPTR) "%s";

								switch( SimpleRequest( NULL, &easy, NULL, ECTR_TYPE_WARNING, NULL, name,  locText[15], locText[17]))
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
											if(DeleteIcon(name,update)) return TRUE;
										}	else
										{
											if(DeleteFile(name) != DOSFALSE )
											{
												if(icon)
												{
													if(DeleteIcon(name,update)) return TRUE;
												}	else return TRUE;
											}
										}
									}
								}
								ioerr = IoErr();
							} else break;
						}

			default:
						return (BOOL)PrintError(ioerr,name, locText[14]);
						break;
		}
	}	else
	{
		if( icon )
		{
			DeleteIcon(name,update);
		}
	}

	return goon;
}
//-------------------------------------
STATIC BOOL ReadInDir( struct DirList *list, APTR dirPool )
{
	BOOL retval = TRUE;
	STRPTR lockName;

	if( !list->lock )
	{
//		Printf("No Lock!\n");
		return FALSE;
	}

	if( list->read )
	{
//		PutStr("Already read!\n");
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
						more = ExAll( list->lock, ed, 1024, ED_PROTECTION, eac );
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
									STRPTR name = (STRPTR)AllocPooled(dirPool,len);
									if(name)
									{
										struct DirNode *dir = (struct DirNode*)AllocPooled(dirPool,sizeof(struct DirNode));
										if( dir )
										{
											strcpy(name,lockName);
											AddPart(name,ed->ed_Name,len);
	
											dir->name = name;
											dir->list.lock = Lock( name, ACCESS_READ);
											dir->list.read = FALSE;
											dir->parentList = list;
											dir->icon = FALSE;
											dir->prot = ed->ed_Prot;
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

					ExAllEnd( list->lock, ead, 1024, ED_PROTECTION, eac );
					FreeVec(ead);

					if( !IsListEmpty((struct List*)&fileList))
					{
						struct FileNode *node;
						struct DirNode *dnode;

						node = (struct FileNode*)fileList.mlh_Head;
						while(node->node.mln_Succ && retval)
						{
							// Icons aussortieren
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
										dnode = (struct DirNode*)list->list.mlh_Head;
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
							retval = DeleteTheFile(node->name,node->icon,FALSE);
							node = (struct FileNode*)node->node.mln_Succ;
						}

						dnode = (struct DirNode*)list->list.mlh_Head;
						while(dnode->node.mln_Succ)
						{
							struct DirNode *help = dnode;
							dnode = (struct DirNode*)dnode->node.mln_Succ;
							if( !IsDeletable(help->name,help->prot))
							{
								UnLock(help->list.lock);
								Remove((struct Node*)help);
							}
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
STATIC BOOL DeleteDir( struct DirList *root, APTR dirPool )
{
	struct DirList *cl = root;
	BOOL goon = TRUE;
	while(cl)
	{
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
//							PutStr("Ready...\n");
							return TRUE;
						}

						if( IsListEmpty((struct List*)cl))
						{
							return FALSE;
						}

						ncn = (struct DirNode*)RemHead((struct List*)cl);
						cn = (struct DirNode*)(((ULONG*)cl)-3);

						if(ncn->list.lock)
						{
							UnLock(ncn->list.lock);
							ncn->list.lock = NULL;
						}

						if( goon )
						{
							Unprotect(ncn->name,ncn->icon,ncn->prot);
							goon = DeleteTheFile(ncn->name,ncn->icon,FALSE);
						}
					}	while( IsListEmpty((struct List*)cl));
					if( !cl ) break;

					ncn = (struct DirNode*)cl->list.mlh_Head;
				}	else
				{
					Remove((struct Node*)cn);

					if(cn->list.lock)
					{
						UnLock(cn->list.lock);
						cn->list.lock = NULL;
					}

					if( goon )
					{
						Unprotect(cn->name,cn->icon,cn->prot);
						goon=DeleteTheFile(cn->name,cn->icon,FALSE);
					}
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
	Text(wnd->RPort,locText[7],strlen(locText[7]));
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
		ng.ng_GadgetText = locText[9];
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
	wndheight = offy + 3*fonty + sizeheight + 12;

	if( CreateGadgets())
	{
		ULONG idcmp = IDCMP_CLOSEWINDOW|IDCMP_NEWSIZE|BUTTONIDCMP|IDCMP_VANILLAKEY;
		if( sizeVerify ) idcmp |= IDCMP_SIZEVERIFY;

		wnd = OpenWindowTags( NULL,
								WA_Left,(scr->Width-wndwidth)/2,
								WA_Top,(scr->Height-wndheight)/2,
								WA_Width, wndwidth,
								WA_Height, wndheight,
								WA_IDCMP, idcmp,
								WA_DragBar,TRUE,
								WA_SizeGadget, TRUE,
								WA_DepthGadget,TRUE,
								WA_CloseGadget, TRUE,
								WA_Title,locText[3],
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

//-------------------------------------
void main(void)
{
	wbmain(WBenchMsg);
}
//-------------------------------------
static void wbmain(struct WBStartup *wbs )
{
	if( wbs->sm_NumArgs > 1 )
	{
		if((IconBase = OpenLibrary("icon.library",37)))
		{
			LONG numArgs=wbs->sm_NumArgs-1;
			struct WBArg *wbarg = wbs->sm_ArgList+1;
			BPTR lock;

//			ScalosBase = OpenLibrary("scalos.library",0);

			lock = Lock("ENV:Scalos/NoSizeVerify",ACCESS_READ);
			if(lock)
			{
				sizeVerify = FALSE;
				UnLock(lock);
//				lock = NULL;
			}	else sizeVerify = TRUE;


			if(LocaleBase) Cat = OpenCatalogA( NULL, (STRPTR) "Scalos/Scalos_Delete.catalog",NULL);
			InitStrings();

			if( SetupScreen())
			{
				struct EasyStruct easy;

				ULONG num_files=0;
				ULONG num_drws=0;
				ULONG i;

				for( i=0; i<numArgs; i++)
				{
					if(wbarg[i].wa_Name)
					{
						if(*wbarg[i].wa_Name) num_files++;
						else num_drws++;
					} else num_drws++;
				}
		
				easy.es_StructSize = sizeof(struct EasyStruct);
				easy.es_Flags = 0;
				easy.es_Title = (STRPTR) "Scalos";
				easy.es_TextFormat = (STRPTR) "%s\n%ld %s %ld %s";
				easy.es_GadgetFormat = (STRPTR) "%s|%s";
	
				if( SimpleRequest( NULL, &easy, NULL, ECTR_TYPE_WARNING, NULL, locText[0], num_files, locText[4], num_drws, locText[5],  locText[1], locText[2]))
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
														case 2: if(!DeleteTheFile(name,TRUE,TRUE))ready=TRUE;
													}
												}
											}	else PrintError(IoErr(),name, (STRPTR) "OK");
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
										if(!DeleteTheFile(name,TRUE,TRUE))
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
			if( Cat ) CloseCatalog( Cat );
//			if( ScalosBase ) CloseLibrary(ScalosBase);
			CloseLibrary(IconBase);
		}
	}
}
//-------------------------------------
