/*************************************************************************\
 *                                                                       *
 * MakeConfig.c                                                          *
 *                                                                       *
 * Copyright 1999-2001 Emmanuel Lesueur                                  *
 *                                                                       *
 * This is an external utility used by the installer script to build     *
 * a config file. It is not intended for any other use.                  *
 *                                                                       *
\*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <dos/dos.h>
#include <libraries/iffparse.h>
#include <libraries/mui.h>
#include <prefs/prefhdr.h>
#define NO_INLINE_STDARG
#include <proto/muimaster.h>
#undef NO_INLINE_STDARG
#include <proto/exec.h>
#include <proto/dos.h>
#define NO_INLINE_STDARG
#include <proto/intuition.h>
#undef NO_INLINE_STDARG
#include <proto/iffparse.h>
struct comm_info;
#include "prefs.h"

char ver[]="$VER:MakeConfig 1.0 (18.11.99)";

#define TEMPLATE    "CONFIG,FONTDIR/K,FONTMAP/K,MODULESDIR/K"
#define NARGS       4
#define CONFIG      ((const char *)args[0])
#define FONTDIR     ((const char *)args[1])
#define FONTMAP     ((const char *)args[2])
#define MODULESDIR  ((const char *)args[3])

LONG args[NARGS];


struct IntuitionBase *IntuitionBase;
struct Library *MUIMasterBase;
struct Library *IFFParseBase;

static Object *config;
static struct RDArgs *rda;

void cleanup(void) {
    if(config)
	MUI_DisposeObject(config);

    CloseLibrary(IFFParseBase);
    CloseLibrary(MUIMasterBase);
    CloseLibrary((struct Library *)IntuitionBase);

    FreeArgs(rda);
} 

BOOL load_prefs(Object *dt,const char *filename) {
    struct IFFHandle *iff;
    BOOL ok=FALSE;
    if(iff=AllocIFF()) {
	if(iff->iff_Stream=Open((char*)filename,MODE_OLDFILE)) {
	    InitIFFasDOS(iff);
	    if(!OpenIFF(iff,IFFF_READ)) {
		if(!StopChunk(iff,ID_PREF,MAKE_ID('A','P','D','F'))) {
		    while(TRUE) {
			struct ContextNode *cn;
			LONG err=ParseIFF(iff,IFFPARSE_SCAN);
			if(err==IFFERR_EOC)
			    continue;
			else if(err)
			    break;
			cn=CurrentChunk(iff);
			if(cn && cn->cn_Type==ID_PREF && cn->cn_ID==MAKE_ID('A','P','D','F')) {
			    if(!DoMethod(dt,MUIM_Dataspace_ReadIFF,iff,ID_PREF,ID_FORM))
				ok=TRUE;
			    break;
			}
		    }
		}
		CloseIFF(iff);
	    }
	    Close(iff->iff_Stream);
	}
	FreeIFF(iff);
    }
    return ok;
}

BOOL save_prefs(Object *dt,const char *filename) {
    struct IFFHandle *iff;
    BOOL ok=FALSE;
    if(iff=AllocIFF()) {
	if(iff->iff_Stream=Open((char*)filename,MODE_NEWFILE)) {
	    InitIFFasDOS(iff);
	    if(!OpenIFF(iff,IFFF_WRITE)) {
		struct PrefHeader hdr={1,0,0};
		if(!PushChunk(iff,ID_PREF,ID_FORM,IFFSIZE_UNKNOWN) &&
		   !PushChunk(iff,0,ID_PRHD,IFFSIZE_UNKNOWN) &&
		   WriteChunkBytes(iff,&hdr,sizeof(hdr))==sizeof(hdr) &&
		   !PopChunk(iff) &&
		   !DoMethod(dt,MUIM_Dataspace_WriteIFF,iff,0,MAKE_ID('A','P','D','F')) &&
		   !PopChunk(iff))
		    ok=TRUE;
		CloseIFF(iff);
	    }
	    Close(iff->iff_Stream);
	}
	FreeIFF(iff);
    }
    return ok;
}


int main() {

    atexit(&cleanup);

    IntuitionBase=(struct IntuitionBase*)OpenLibrary("intuition.library",39);
    IFFParseBase=OpenLibrary("iffparse.library",39);
    if(!IntuitionBase || !IFFParseBase) {
	printf("Requires AmigaOS 3.0+\n");
	exit(EXIT_FAILURE);
    }
    MUIMasterBase=OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN);
    if(!MUIMasterBase) {
	printf("Can't open \"muimaster.library\".\n");
	exit(EXIT_FAILURE);
    }

    if(!(rda=ReadArgs(TEMPLATE,args,NULL))) {
	PrintFault(IoErr(),"Error: ");
	exit(EXIT_FAILURE);
    }

    if(!(config=MUI_NewObjectA(MUIC_Dataspace,NULL))) {
	printf("Can't create config object.\n");
	exit(EXIT_FAILURE);
    }

    load_prefs(config,CONFIG?CONFIG:"Apdf.config");

    if(FONTDIR) {
	ULONG d=DoMethod(config,MUIM_Dataspace_Find,MYCFG_Apdf_FontPath);
	int n=0;
	size_t sz=0;
	size_t sz2;
	char *buf;
	if(d) {
	    int k;
	    char* p;
	    n=*(int*)d;
	    p=(char*)d+sizeof(int);
	    for(k=0;k<n;++k) {
		size_t s=strlen(p)+1;
		sz+=s;
		p+=s;
	    }
	}
	sz2=strlen(FONTDIR)+1;
	if(buf=malloc(sizeof(int)+sz+sz2)) {
	    char *q=buf+sizeof(int);
	    *(int*)buf=n+1;
	    if(d) {
		memcpy(q,(char*)d+sizeof(int),sz);
		q+=sz;
	    }
	    memcpy(q,FONTDIR,sz2);
	    DoMethod(config,MUIM_Dataspace_Add,buf,sizeof(int)+sz+sz2,MYCFG_Apdf_FontPath);
	    free(buf);
	}
    }

    if(FONTMAP) {
	ULONG d=DoMethod(config,MUIM_Dataspace_Find,MYCFG_Apdf_FontMap);
	int n=0;
	size_t sz=0;
	size_t sz2;
	char *buf;
	const char *pdffont;
	const char *font;
	const char *end;
	char type='\0';
	if(d) {
	    int k;
	    char *p=(char*)d+sizeof(int);
	    n=*(int*)d;
	    for(k=0;k<n;++k) {
		size_t s=strlen(p)+1;
		sz+=s;
		p+=s;
		s=strlen(p)+1+2;
		sz+=s;
		p+=s;
	    }
	}
	{
	    const char *p=pdffont=FONTMAP;
	    const char *r;
	    while(*p && *p!='/')
		++p;
	    if(*p=='/' && p!=pdffont) {
		font=++p;
		while(*p && *p!='[')
		    ++p;
		if(*p=='[' && p!=font) {
		    sz2=p-pdffont+3;
		    end=p;
		    if(*++p=='T' && (*++p=='1' || *p=='T') &&
		       *++p==']')
			type=p[-1];
		}
	    }
	}
	if(!type)
	    printf("Invalid fontmap entry.\n",FONTMAP);
	else {
	    if(buf=malloc(sizeof(int)+sz+sz2)) {
		char *q=buf+sizeof(int);
		*(int*)buf=n+1;
		if(d) {
		    memcpy(q,(char*)d+sizeof(int),sz);
		    q+=sz;
		}
		memcpy(q,pdffont,end-pdffont);
		q[font-pdffont-1]='\0';
		q[end-pdffont]='\0';
		q[end-pdffont+1]=type;
		q[end-pdffont+2]='\0';
		DoMethod(config,MUIM_Dataspace_Add,buf,sizeof(int)+sz+sz2,MYCFG_Apdf_FontMap);
		free(buf);
	    }
	}
    }

    if(MODULESDIR)
	DoMethod(config,MUIM_Dataspace_Add,MODULESDIR,strlen(MODULESDIR)+1,MYCFG_Apdf_ModulesDir);

    save_prefs(config,CONFIG?CONFIG:"Apdf.config");

    return 0;
}

