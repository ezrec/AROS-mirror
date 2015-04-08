//========================================================================
//
// Apdfcommon.cc
//
// Copyright 1999-2002 Emmanuel Lesueur
//
// Some parts come from xpdf.cc
//
//========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#define Object ZZObject
#include <dos/dos.h>
#ifdef HAVE_POWERUP
#   define strlen ZZstrlen
#   include <powerup/ppcproto/exec.h>
#   include <powerup/ppcproto/dos.h>
#   undef strlen
#else
#   include <proto/exec.h>
#   include <proto/dos.h>
#endif
#undef Object
#undef Allocate
#include "parseargs.h"
#include "GString.h"
#include "gmem.h"
#include "Object.h"
#include "Stream.h"
#include "Array.h"
#include "Dict.h"
#include "ErrorCodes.h"
#include "XRef.h"
#include "Catalog.h"
#include "Page.h"
#include "Annotations.h"
#include "Action.h"
#include "AcroForm.h"
#include "Outline2.h"
#include "PDFDoc.h"
#include "AOutputDev.h"
#include "TextOutputDev.h"
#include "PSOutputDev.h"
#include "ImageOutputDev.h"
#include "FontOutputDev.h"
#include "Error.h"
#include "config.h"
#include "PageLabel.h"
#include "UnicodeMap.h"
#include "apdf/AComm.h"
#include "apdf/io_client.h"
#include "server/OutputFontCache.h"

#define DB(x) //x

#define TIME 0

#ifdef __SASC
#   undef  try
#   undef  catch
#   define try
#   define catch(x)    if(const char* msg=NULL)
#endif

extern "C" void dprintf(const char *, ...);
//#define printf dprintf

#define LINKAGE

int maxColors=16;
volatile int *abortFlagPtr;
volatile int *drawStatePtr;
int lastDrawState;

static int show_errors;
struct Error {
    Error* next;
    char msg[1];
};
Error *errors;
static Error **last_errorp=&errors;


LINKAGE void clear_font_path();
LINKAGE void clear_errors();


void cleanup2() {
    DEBUG_INFO
    Object::memCheck(stderr);
    delete globalParams;
    /*gMemReport(stderr);*/
    clear_errors();
}

LINKAGE int init(int errors,volatile int* afp,volatile int* dsp) {
    DEBUG_INFO
    int ok=1;
    abortFlagPtr=afp;
    drawStatePtr=dsp;
    DB(printf("init\n");)
    try {
	atexit(&cleanup2);
	show_errors=errors;
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
	ok=0;
    }
    catch(...) {
	printf("Exception caught.\n");
	ok=0;
    }
    DB(printf("init=%d\n",ok);)
    return ok;
}

void CDECL error(int pos,char *msg,...) {
    if(show_errors) {
	va_list args;
	char buf[256];
	char *p=buf;
	int len;
	if (pos >= 0) {
	    len=sprintf(p,"Error (%d): ",pos);
	} else {
	    len=sprintf(p,"Error: ");
	}
	va_start(args,msg);
	len+=vsprintf(p+len,msg,args);
	va_end(args);
	DB(printf("ERR=\"%s\"\n",buf);)
	if(Error* e=(Error*)malloc(sizeof(*e)+len)) {
	    e->next=NULL;
	    strcpy(e->msg,buf);
	    *last_errorp=e;
	    last_errorp=&e->next;
	} else
	    puts(buf);
    }
}

LINKAGE int get_error(char* buf,size_t len) {
    if(Error* e=errors) {
	strncpy(buf,e->msg,len-1);
	buf[len-1]='\0';
	if(last_errorp==&e->next)
	    last_errorp=&errors;
	errors=e->next;
	free(e);
	return 1;
    } else
	return 0;
}

LINKAGE void clear_errors() {
    while (errors) {
      Error* nxt = errors->next;
      free(errors);
      errors = nxt;
    }
    last_errorp=&errors;
}

LINKAGE void use_prefs(int colors,const char* gzip,size_t cachesz,size_t cacheblocsz) {
    DB(printf("use_prefs(%d,%d,%d)\n",colors,cachesz,cacheblocsz);)
    try {
	myFILE::bufsizes(cacheblocsz,cachesz);

	maxColors=colors;

	delete globalParams;
	globalParams=NULL;
	globalParams=new GlobalParams(".xpdfrc");
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
}

LINKAGE void get_version(char* buf,size_t) {
    strcpy(buf,"Based on xpdf "xpdfVersion"\n"
	       xpdfCopyright"\n"
	       "derekn@foolabs.com\n"
	       "http://www.foolabs.com/xpdf/\n\n"
	       "Amiga/MorphOS specific parts are copyright © 1999-2005 Emmanuel Lesueur\n"
	       "lesueur@club-internet.fr\n"
	       "http://elesueur.free.fr/Apdf\n\n"
	       "Supports PDF version "supportedPDFVersionStr"\n"
	       "The PDF data structures, operators, and specifications\n"
	       "are copyright 1995 Adobe Systems Inc.\n\n"
	       "Apdf uses parts of the FreeType Project,\n"
	       "Copyright © 1996-2000 by David Turner, Robert Wilhelm and Werner Lemberg\n"
	       "http://www.freetype.org\n");
}

LINKAGE PDFDoc* create_doc(BPTR dir,const char* filename,int* num_pages,
			   const char *ownerpw, const char *userpw) {
    DEBUG_INFO
    DB(printf("create_doc(%s,%s,%s)\n",filename,ownerpw,userpw);)
    IoDir olddir=ioCurrentDir(dir);
    PDFDoc* doc=NULL;
    *num_pages=0;
    try {
	GString opw(ownerpw);
	GString upw(userpw);

	doc=new PDFDoc(new GString((char*)filename),ownerpw[0]?&opw:NULL,userpw[0]?&upw:NULL);
	if(!doc->isOk()) {
	    if(doc->getErrorCode() == errEncrypted) {
	      clear_errors();
	      *num_pages=-1;
	    }
	    delete doc;
	    doc=NULL;
	} else {
		if (doc->getNumPages() == 0)
		{
			error(-1, "Document contains no pages");
			*num_pages = 0;
			delete doc;
			doc = NULL;
		}
		else
			*num_pages=doc->getNumPages();
	}
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
    ioCurrentDir(olddir);
    DB(printf("create_doc=%x\n",doc);)
    return doc;
}

LINKAGE void delete_doc(PDFDoc* doc) {
    DEBUG_INFO
    DB(printf("delete_doc(%x)\n",doc);)
    delete doc;
}

static size_t latin1Size(GString *s) {
    UnicodeMap *map=globalParams->getTextEncoding();
    int len=s->getLength();
    const unsigned char *p=(const unsigned char*)s->getCString();
    GBool isUnicode=gFalse;
    int sz=0;
    char buf[16];

    if(len>=2 && p[0]==0xfe && p[1]==0xff) {
	isUnicode=gTrue;
	p+=2;
	len-=2;
    }
    while(len>0) {
	Unicode u=*p++;
	--len;
	if(isUnicode) {
	    u=(u<<8)|*p++;
	    --len;
	}
	sz+=map->mapUnicode(u,buf,sizeof(buf));
    }
    return sz;
}

GString *toLatin1(GString *s) {
    UnicodeMap *map=globalParams->getTextEncoding();
    int len=s->getLength();
    const unsigned char *p=(const unsigned char *)s->getCString();
    GString *r=new GString;
    r->setLength(latin1Size(s));
    GBool isUnicode=gFalse;
    char *t=r->getCString();
    int len2=r->getLength();

    if(len>=2 && p[0]==0xfe && p[1]==0xff) {
	isUnicode=gTrue;
	p+=2;
	len-=2;
    }
    while(len>0) {
	Unicode u=*p++;
	--len;
	if(isUnicode) {
	    u=(u<<8)|*p++;
	    --len;
	}
	int n=map->mapUnicode(u,t,len2);
	t+=n;
	len2-=n;
    }
    *t='\0';
    return r;
}

AOutputDev* create_output_dev(unsigned char* map, int size) {
    DEBUG_INFO
    AOutputDev* AOut=NULL;
    try {
	AOut=new AOutputDev(map, size);
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
    DB(printf("create_outputdev=%x\n",AOut);)
    return AOut;
}

LINKAGE void delete_output_dev(AOutputDev* AOut) {
    DEBUG_INFO
    DB(printf("delete_outputdev(%x)\n",AOut);)
    delete AOut;
}

LINKAGE void set_AA_levels(AOutputDev* AOut,
			   int text,int stroke, int fill) {
    if(AOut)
	AOut->setAALevels(text,stroke,fill);
}

LINKAGE int get_page_size(PDFDoc* doc,int page,int* width,int* height,int* rotate) {
    DEBUG_INFO
    DB(printf("get_page_size(%x,%d)\n",doc,page);)
    doc->activate();
    *width=int(doc->getPageMediaWidth(page)+0.5);
    if(*width<=0)
	*width=1;
    *height=int(doc->getPageMediaHeight(page)+0.5);
    if(*height<=0)
	*height=1;
    *rotate=doc->getPageRotate(page);
    return 1;
}

LINKAGE int simple_find(AOutputDev *AOut,const char* str,int top,int* xmin,int* ymin,int* xmax,int* ymax) {
    DEBUG_INFO
    int ret=0;
    Unicode *u=NULL;
    try {
	int len=strlen(str);
	u=new Unicode [len];
	for(int k=0;k<len;++k)
	    u[k]=(unsigned char)str[k];
	if(AOut->findText(u,len,top,gTrue,gFalse,gFalse,xmin,ymin,xmax,ymax))
	    ret=1;
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
    delete [] u;
    return ret;
}

static Unicode* find_str;
static int find_str_len;
static TextOutputDev* find_dev;

LINKAGE void end_find() {
    DEBUG_INFO
    delete find_dev;
    find_dev=NULL;
    delete [] find_str;
    find_str=NULL;
}

LINKAGE int init_find(const char* str) {
    DEBUG_INFO
    try {
	find_str_len=strlen(str);
	find_str=new Unicode [find_str_len];
	for(int k=0;k<find_str_len;++k)
	    find_str[k]=(unsigned)str[k];
	find_dev=new TextOutputDev(NULL,gFalse,gFalse,gFalse);
	if(find_dev->isOk())
	    return 1;
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
    end_find();
    return 0;
}

LINKAGE int find(PDFDoc* doc,int start,int stop,int* xmin,int* ymin,int* xmax,int* ymax,int* page) {
    DEBUG_INFO
    int r=0;
    try {
	if(find_dev && find_str) {
	    doc->activate();
	    for(int pg=start;pg<stop;++pg) {
		double x1,x2,y1,y2;
		doc->displayPage(find_dev,pg,72,72,0,gFalse,gFalse,gFalse);
		if(find_dev->findText(find_str,find_str_len,gTrue,gTrue,gFalse,gFalse,gFalse,gFalse,&x1,&y1,&x2,&y2)) {
		    r=1;
		    *page=pg;
		    break;
		}
	    }
	}
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
    return r;
}


LINKAGE void end_write(OutputDev* sv) {
    DEBUG_INFO
    delete sv;
}

LINKAGE OutputDev* init_write(PDFDoc* doc,const char* filename,int type,int first_page,int last_page,int zoom,int rotate) {
    DEBUG_INFO
    OutputDev* sv=NULL;
    try {
	doc->activate();
	switch(type) {
	  case 0: // Postscript
	  case 1: // Postscript Level 1
	    //if(doc->okToPrint()) {
		//psOutLevel1=type==1;
		sv=new PSOutputDev((char*)filename,doc->getXRef(),doc->getCatalog(),
				   first_page,last_page,psModePS);
	    //}
	    break;
	  case 2: // Text
	    sv=new TextOutputDev((char*)filename,gFalse,gFalse,gFalse);
	    break;
	  case 3: // PBM/PPM images
	  case 4: // JPEG images
	    sv=new ImageOutputDev((char*)filename,type==4);
	    break;
	}
	if(sv && !sv->isOk()) {
	    delete sv;
	    sv=NULL;
	}
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
    return sv;
}

LINKAGE int writefile(PDFDoc* doc,OutputDev* sv,int first_page,int last_page,int zoom,int rotate) {
    DEBUG_INFO
    int r=0;
    try {
	doc->activate();
	doc->displayPages(sv,first_page,last_page-1,zoom,zoom,rotate,gFalse,gFalse,gFalse);
	r=1;
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
    return r;
}

LINKAGE char* gettext(AOutputDev* AOut,int minx,int miny,int maxx,int maxy) {
    DEBUG_INFO
    char* r=NULL;
    GString* s=NULL;
    try {
	s=AOut->getText(minx,miny,maxx,maxy);
	size_t sz=s->getLength();
	if(r=(char*)malloc(sz+1))
	    strcpy(r,s->getCString());
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
    delete s;
    return r;
}

#if TIME
static void get_time(unsigned long long& time)
{
    register unsigned long hi,lo,hi2;

#if __GNUC_PATCHLEVEL__ > 2
    hi = lo = hi2 = 0;
#endif
  __asm(
".loop:\n"
"	mftbu %1\n"
"	mftb  %2\n"
"	mftbu %3\n"
"	cmpw %1, %3\n"
"	bne .loop\n"
"	stw %1, 0(%0)\n"
"	stw %2, 4(%0)\n"
        :
        : "b" (&time), "r" (hi), "r" (lo), "r" (hi2)
        : "cc");
}
#endif

LINKAGE int show_page(AOutputDev* aout,PDFDoc* doc,
		      int page,int zoom,int rotate,unsigned char* bitmap,
		      int xoffset,int yoffset,int width,int height,
		      int modulo,int bpp,int pixfmt,int level) {
    DEBUG_INFO
    int r=level;
    if(doc && aout) {
	try {
	    DB(printf("show_page(%x,%x,%d)\n",doc,aout,level);)
#if TIME
	    unsigned long long t1, t2;
	    get_time(t1);
#endif
	    doc->activate();
	    aout->init(bitmap,xoffset,yoffset,width,height,modulo,bpp,pixfmt);
	    aout->startDoc(doc->getXRef());
	    aout->setDrawLevel(level);
	    //doc->displayPageSlice(aout,page,zoom,zoom,rotate,gFalse,gFalse,gTrue,xoffset,yoffset,width,height);
	    doc->displayPage(aout,page,zoom,zoom,rotate,gTrue,gFalse,gTrue);
	    r=aout->getDrawLevel();
	    aout->endPage(); // in case rendering is interrupted while a
			     // clipping region is installed
	    aout->cleanup();
#if TIME
	    get_time(t2);
	    t2-=t1;
	    printf("time: 0x%lx%08lx\n",long(t2>>32),long(t2));
#endif
	    DB(printf("=%d\n",r);)
	}
	catch(const char* msg) {
	    printf("Exception: %s\n",msg);
	}
	catch(...) {
	    printf("Exception caught.\n");
	}
    }
    return r;
}

LINKAGE void add_fontmap(PDFDoc *doc,
			 const char* pdffont,const char* font,
			 int type) {
    try {
	DB(printf("add_fontmap(%x,%s,%s,%d)\n",doc,pdffont,font,type);)
	FontMap *fontMap=doc ? doc->getFontMap() : &globalFontMap;
	fontMap->add(pdffont,font,GfxFontType(type));
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
}

LINKAGE void clear_fontmap(PDFDoc *doc) {
    DEBUG_INFO
    if (doc) {
	DB(printf("clear_fontmap(%x)\n",doc);)
	doc->getFontMap()->clear();
	doc->getGfxFontCache()->clear();
    } else {
	DB(printf("clear_global_fontmap()\n");)
	globalFontMap.clear();
	if (baseFontCache) {
	    baseFontCache->clear();
	}
	/***/
    }
}


LINKAGE void end_scan_fonts(FontOutputDev* fOut) {
    DEBUG_INFO
    DB(printf("end_scan_fonts(%x)\n",fOut);)
    delete fOut;
}

LINKAGE FontOutputDev* init_scan() {
    DEBUG_INFO
    FontOutputDev* fOut=NULL;
    try {
	fOut=new FontOutputDev;
	if(fOut && !fOut->isOk()) {
	    delete fOut;
	    fOut=NULL;
	}
	DB(printf("init_scan()=%lx\n",fOut);)
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
    return fOut;
}

LINKAGE int scan_fonts(struct PDFDoc *doc,struct FontOutputDev *fOut,
		       int first_page,int last_page) {
    DEBUG_INFO
    int r=0;
    if(doc && fOut) {
	DB(printf("scan_fonts(%lx,%d,%d)\n",fOut,first_page,last_page);)
	try {
	    doc->activate();
	    doc->displayPages(fOut,first_page,last_page-1,72,72,0,gFalse,gFalse,gFalse);
	    r=fOut->size();
	}
	catch(const char* msg) {
	    printf("Exception: %s\n",msg);
	}
	catch(...) {
	    printf("Exception caught.\n");
	}
	DB(printf("done.\n"));
    }
    return r;
}

#if 0
LINKAGE int scan_default_fonts() {
    int r=0;
    try {
	if(fOut) {
	    delete fOut;
	    fOut=NULL;
	}
	fOut=new DefaultFontOutputDev;
	if(fOut->isOk())
	    r=fOut->size();
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
    if(r==0) {
	delete fOut;
	fOut=NULL;
    }
    return r;
}
#endif

LINKAGE int get_font(FontOutputDev *fOut,int n,
		     char* pdffont,int pdffontlen,
		     char* font,int fontlen,int* type,
		     int* flags) {
    DEBUG_INFO
    if(fOut) {
	const char* p;
	const char* q;
	DB(printf("get_font(%lx)\n",fOut));
	GfxFontType type1;
	fOut->get(n,p,q,type1,*flags);
	if(p) {
	    *type=type1;
	    strncpy(pdffont,p,pdffontlen-1);
	    pdffont[pdffontlen-1]='\0';
	    strncpy(font,q,fontlen-1);
	    font[fontlen-1]='\0';
	    return 1;
	} else
	    return 0;
    } else
	return 0;
}

static void add_outlines_sizes(OutlineTree& tree,int& num,size_t& bufsize,int level) {
    DEBUG_INFO
    int tot=tree.getNumChildren();
    num+=tot;
    for(int n=0;n<tot;++n) {
	Outline* outline=tree.getOutline(n);
	bufsize+=latin1Size(outline->getTitle())+3;
	if(OutlineTree* subtree=outline->getChildren()) {
	    ++bufsize;
	    add_outlines_sizes(*subtree,num,bufsize,level+1);
	}
    }
}

LINKAGE int has_outlines(PDFDoc* doc) {
    DEBUG_INFO
    DB(printf("has_outlines(%x)\n",doc);)
    doc->activate();
    return doc->getCatalog()->hasOutlines();
}

LINKAGE int get_outlines_info(PDFDoc* doc,int* num,size_t* bufsize) {
    DEBUG_INFO
    DB(printf("get_outlines_info(%x)\n",doc);)
    *num=0;
    *bufsize=0;
    try {
	doc->activate();
	if(OutlineTree* tree=doc->getCatalog()->getOutlines()) {
	    add_outlines_sizes(*tree,*num,*bufsize,0);
	    //printf("outlines size=%d (%d)\n",*bufsize,*num);
	    return 1;
	}
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
    return 0;
}

static void add_outlines(OutlineTree& tree,const char**& titles,Action**& actions,char*& strbuf,int level) {
    DEBUG_INFO
    int tot=tree.getNumChildren();
    for(int n=0;n<tot;++n) {
	Outline* outline=tree.getOutline(n);
	*titles++=strbuf;
	*strbuf++=char('a'+level);
	GString* s=toLatin1(outline->getTitle());
	OutlineTree* subtree=outline->getChildren();
	if(subtree)
	    *strbuf++=subtree->isOpened()?'T':'t';
	*strbuf++='\0';
	size_t sz=s->getLength()+1;
	memcpy(strbuf,s->getCString(),sz);
	strbuf+=sz;
	*actions++=outline->getAction();
	delete s;
	if(subtree)
	    add_outlines(*subtree,titles,actions,strbuf,level+1);
    }
}

LINKAGE int get_outlines(PDFDoc* doc,OutlinesInfo* outlines,size_t) {
    DEBUG_INFO
    DB(printf("get_outlines\n");)
    doc->activate();
    OutlineTree* tree=doc->getCatalog()->getOutlines();
    const char** titles=outlines->titles;
    Action** actions=outlines->actions;
    char* strbuf=outlines->buf;
    add_outlines(*tree,titles,actions,strbuf,0);
//printf("outlines actual size: %d (%d)\n",strbuf-outlines->buf,titles-outlines->titles);
    *titles=NULL;
    *actions=NULL;
}

static void appendEntry(Object& dict,GString& str,const char* entry) {
    DEBUG_INFO
    Object obj;
    if(dict.dictLookup((char*)entry,&obj)->isString()) {
	GString* s=toLatin1(obj.getString());
	str.append(s);
	delete s;
    }
    str.append('\0');
    obj.free();
}

static void appendDate(Object& dict,GString& str,const char* entry) {
    DEBUG_INFO
    Object obj;
    if(dict.dictLookup((char*)entry,&obj)->isString()) {
	GString* s=toLatin1(obj.getString());
	char buf[32];
	char* q=buf;
	int l=s->getLength();
	const char* p=s->getCString();
	if(p[0]=='D' && p[1]==':') {
	    p+=2;
	    l-=2;
	    *q++='D';
	    *q++=':';

	    if(l>=8) {
		*q++=p[6];
		*q++=p[7];
	    } else
		*q++='1';
	    *q++='-';
	    if(l>=6) {
		*q++=p[4];
		*q++=p[5];
	    } else
		*q++='1';
	    *q++='-';
	    if(l>=4) {
		//*q++=p[0];
		//*q++=p[1];
		*q++=p[2];
		*q++=p[3];
	    } else
		*q++='0';
	    *q++='\0';
	    if(l>=10) {
		*q++=p[8];
		*q++=p[9];
		*q++=':';
		if(l>=12) {
		    *q++=p[10];
		    *q++=p[11];
		} else {
		    *q++='0';
		    *q++='0';
		}
		*q++=':';
		if(l>=14) {
		    *q++=p[12];
		    *q++=p[13];
		} else {
		    *q++='0';
		    *q++='0';
		}
	    }
	    /*if((l>=15 && p[14]=='Z') ||
	       (l>=18 && p[17]=='\'')) {
		*q++=' ';
		*q++='G';
		*q++='M';
		*q++='T';
		if(p[14]=='+' || p[14]=='-') {
		    *q++=p[14];
		    *q++=p[15];
		    *q++=p[16];
		    if(l>=21 && p[20]=='\'') {
			*q++=':';
			*q++=p[18];
			*q++=p[19];
		    }
		}
	    }*/
	    *q='\0';
	    str.append(buf,q-buf+1);
	} else
	    str.append((char*)p,s->getLength()+1);
	delete s;
    } else
	str.append('\0');
    obj.free();
}

LINKAGE char* get_doc_info(PDFDoc* doc,size_t* sz,int* linearized,int* encrypted,int* oktoprint,int* oktocopy) {
    DEBUG_INFO
    DB(printf("get_doc_info\n");)
    char* r=NULL;
    try {
	doc->activate();
	Object info;
	doc->getDocInfo(&info);
	GString str;
	if(info.isDict()) {
	    appendEntry(info,str,"Title");
	    appendEntry(info,str,"Author");
	    appendEntry(info,str,"Subject");
	    appendEntry(info,str,"Creator");
	    appendEntry(info,str,"Producer");
	    appendEntry(info,str,"Keywords");
	    appendDate(info,str,"CreationDate");
	    appendDate(info,str,"ModDate");
	} else {
	  str.append("\0\0\0\0\0\0\0",8);
	}
	*linearized=doc->isLinearized();
	*encrypted=doc->isEncrypted();
	*oktoprint=doc->okToPrint();
	*oktocopy=doc->okToCopy();
	r=(char*)malloc(*sz=str.getLength()+1);
	if(r)
	    memcpy(r,str.getCString(),*sz);
	info.free();
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
    return r;
}

LINKAGE void clear_font_path() {
    DEBUG_INFO
    DB(printf("clear_font_path()\n");)
    globalParams->clearFontDirs();
}

LINKAGE void add_font_path(const char *path) {
    DEBUG_INFO
    DB(printf("add_font_path(%s)\n", path);)
    globalParams->addFontDir(new GString(path));
}

LINKAGE const char** get_page_labels(PDFDoc* doc,size_t* szp,int* pagesp) {
    DEBUG_INFO
    const char** r=NULL;
    try {
	if(doc) {
	    doc->activate();
	    if(PageLabels* labels=doc->getCatalog()->getPageLabels()) {
		int pages=doc->getNumPages();
		GString** labs=new GString* [pages];
		size_t sz=pages*sizeof(const char*);
		for(int k=0;k<pages;++k) {
		    if(!(labs[k]=labels->getPageLabel(k)))
			labs[k]=GString::fromInt(k);
		    sz+=labs[k]->getLength()+1;
		}
		r=(const char**)gmalloc(sz);
		*szp=sz;
		const char** q=r;
		char* p=(char*)(r+pages);
		*pagesp=pages;
		for(int k=0;k<pages;++k) {
		    *q++=p;
		    strncpy(p,labs[k]->getCString(),labs[k]->getLength()+1);
		    p+=labs[k]->getLength()+1;
		    delete labs[k];
		}
		delete [] labs;
	    }
	}
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
    return r;
}

LINKAGE int get_page_index(PDFDoc* doc,const char* label) {
    DEBUG_INFO
    int r=-1;
    try {
	if(label[0]=='#')
	    r=atoi(label+1);
	else if(doc) {
	    doc->activate();
	    if(PageLabels *labels=doc->getCatalog()->getPageLabels()) {
		r=labels->getPageIndex(label);
		if(r>=0)
		    ++r;
	    } else
		r=atoi(label);
	}
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
    return r;
}

LINKAGE int get_num_annots(PDFDoc* doc,int page,size_t* szp) {
    DEBUG_INFO
    int r=0;
    size_t sz=0;
    int n=0;
    int l=0;
    try {
	doc->activate();
	Annots* annots=doc->getAnnots(page);
	if(annots)
	    l=annots->getNumAnnots();
	for(int k=0;k<l;++k) {
	    Annot* a=annots->getAnnot(k);
	    if(!a->isReadOnly() && !a->isHidden()) {
		++n;
		if(char *s=a->getDescr()) {
		    GString str(s);
		    sz+=latin1Size(&str)+1;
		}
	    }
	}
	sz+=n*sizeof(AnnotInfo);
	r=n;
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
    *szp=sz;
    return r;
}

LINKAGE int get_annots(PDFDoc* doc,AOutputDev *AOut,int page,struct AnnotInfo* buf,int num) {
    DEBUG_INFO
    if(!doc || !AOut)
	return 0;
    int r=0;
    try {
	doc->activate();
	Annots* annots=doc->getAnnots(page);
	if(annots) {
	    char* p=(char*)(buf+num);
	    int num1=annots->getNumAnnots();
	    for(int k=0;k<num1;++k) {
		Annot* a=annots->getAnnot(k);
		if(!a->isReadOnly() && !a->isHidden()) {
		    double x1,y1,x2,y2;
		    int xa1,ya1,xa2,ya2;
		    a->getRect(&x1,&y1,&x2,&y2);
		    AOut->cvtUserToDev(x1,y1,&xa1,&ya1);
		    AOut->cvtUserToDev(x2,y2,&xa2,&ya2);
		    if(xa1>xa2) {
			int t=xa1; xa1=xa2; xa2=t;
		    }
		    if(ya1>ya2) {
			int t=ya1; ya1=ya2; ya2=t;
		    }
		    buf->annot=a;
		    buf->x=xa1;
		    buf->y=ya1;
		    buf->width=xa2-xa1+1;
		    buf->height=ya2-ya1+1;
//printf("Annot(%lx,%d,%d,%d,%d)\n",a,xa1,ya1,xa2-xa1+1,ya2-ya1+1);
		    char* s=a->getDescr();
		    if(s) {
			GString str(s),*t;
			buf->title=p;
			t=toLatin1(&str);
			memcpy(p,t->getCString(),t->getLength()+1);
			p+=t->getLength()+1;
			delete t;
		    } else
			buf->title=NULL;
		    ++buf;
		}
	    }
	    r=1;
	}
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
    return r;
}


class MyEventHandler : public EventHandler {
  public:
    class event {
	friend class MyEventHandler;
      public:
	event() : next(NULL) {}
	virtual ~event() {}
	virtual int get_id() const =0;
	virtual size_t get_size() =0;
	virtual void write(AOutputDev*,void*) =0;
      private:
	event* next;
    };

  private:

    class opendoc_event : public event {
      public:
	opendoc_event(GString* s,GBool f) : str(s),new_win(f) {}
	virtual int get_id() const { return actionOpenDoc; }
	virtual size_t get_size() { return (sizeof(mActionOpenDoc)+str.getLength()+4)&~3; }
	virtual void write(AOutputDev*,void* buf) {
	    mActionOpenDoc* p=static_cast<mActionOpenDoc*>(buf);
	    p->new_window=new_win;
	    memcpy(p->file,str.getCString(),str.getLength()+1);
	    DB(printf("opendoc(%s)\n",str.getCString());)
	}
      private:
	GString str;
	GBool new_win;
    };

    class goto_event : public event {
      public:
	goto_event(int n,PDFRectangle* r,FitMode m)
	    : page(n),x1(r->x1),y1(r->y1),x2(r->x2),y2(r->y2),mode(m) {}
	virtual int get_id() const { return actionGoTo; }
	virtual size_t get_size() { return sizeof(mActionGoTo); }
	virtual void write(AOutputDev* AOut,void* buf) {
	    mActionGoTo* r=static_cast<mActionGoTo*>(buf);
	    int xa1,ya1,xa2,ya2;
	    DB(printf("goto(%d,%f,%f,%f,%f,%d)\n",page,x1,y1,x2,y2,mode);)
	    AOut->cvtUserToDev(x1,y1,&xa1,&ya1);
	    AOut->cvtUserToDev(x2,y2,&xa2,&ya2);
	    DB(printf("->goto(%d,%d,%d,%d,%d,%d)\n",page,xa1,ya1,xa2,ya2,mode);)
	    r->page=page;
	    r->x1=x1<-9999?-1:xa1;
	    r->y1=y1<-9999?-1:ya1;
	    r->x2=xa2;
	    r->y2=ya2;
	    r->mode=mode;
	}
      private:
	int page, mode;
	double x1,y1,x2,y2;
    };

    class show_text_event : public event {
      public:
	show_text_event(GString* s) : str(s) {}
	virtual ~show_text_event() {}
	virtual int get_id() const { return actionShowText; }
	virtual size_t get_size() { return (str.getLength()+4)&~3; }
	virtual void write(AOutputDev*,void* buf) {
	    memcpy(buf,str.getCString(),str.getLength()+1);
/*            char* q=static_cast<char*>(buf);
	    const char* r=str->getCString();
	    int len=str->getLength();
printf("showtext=");
while(--len>=0) printf("%02lx ",*r++);
len=str->getLength();
r=str->getCString();
	    while(--len>=0) {
		*q++=*r=='\r'?'\n':*r;
		++r;
	    }
	    *q++='\0';
printf("\nshow(%s)\n",buf);*/
	}
      private:
	GString str;
    };

    class refresh_event : public event {
      public:
	virtual int get_id() const { return actionRefresh; }
	virtual size_t get_size() { return 0; }
	virtual void write(AOutputDev* AOut,void* buf) {}
    };

    class rect_update_event : public event {
      public:
	rect_update_event(Annot *a) {
	    a->getRect(&x1,&y1,&x2,&y2);
	}
	virtual int get_id() const { return actionUpdateRect; }
	virtual size_t get_size() { return sizeof(mActionUpdateRect); }
	virtual void write(AOutputDev* AOut,void* buf) {
	    mActionUpdateRect* r=static_cast<mActionUpdateRect*>(buf);
	    int xa1,ya1,xa2,ya2;
	    AOut->cvtUserToDev(x1,y1,&xa1,&ya1);
	    AOut->cvtUserToDev(x2,y2,&xa2,&ya2);
	    if(xa1>xa2) {
		int t=xa1; xa1=xa2; xa2=t;
	    }
	    if(ya1>ya2) {
		int t=ya1; ya1=ya2; ya2=t;
	    }
	    r->x=xa1;
	    r->y=ya1;
	    r->width=xa2-xa1+1;
	    r->height=ya2-ya1+1;
	}
      private:
	double x1,y1,x2,y2;
    };

    class update_annots_event : public event {
      public:
	virtual int get_id() const { return actionUpdateAnnots; }
	virtual size_t get_size() { return 0; }
	virtual void write(AOutputDev* AOut,void* buf) {}
    };

    class launch_event : public event {
      public:
	launch_event(GString* s, GBool f) : str(s),new_win(f) {}
	virtual int get_id() const { return actionLaunch; }
	virtual size_t get_size() { return (sizeof(mActionLaunch)+str.getLength()+4)&~3; }
	virtual void write(AOutputDev*,void* buf) {
	    mActionLaunch* p=static_cast<mActionLaunch*>(buf);
	    p->new_window=new_win;
	    memcpy(p->cmd,str.getCString(),str.getLength()+1);
	}
      private:
	GString str;
	GBool new_win;
    };

    class URI_event : public event {
      public:
	URI_event(GString* s, GBool f) : str(s),new_win(f) {}
	virtual int get_id() const { return actionURI; }
	virtual size_t get_size() { return (sizeof(mActionURI)+str.getLength()+4)&~3; }
	virtual void write(AOutputDev*,void* buf) {
	    mActionURI* p=static_cast<mActionURI*>(buf);
	    p->new_window=new_win;
	    memcpy(p->uri,str.getCString(),str.getLength()+1);
	}
      private:
	GString str;
	GBool new_win;
    };

    class named_event : public event {
      public:
	named_event(int n) : id(n) {}
	virtual int get_id() const { return actionNamed; }
	virtual size_t get_size() { return sizeof(mActionNamed); }
	virtual void write(AOutputDev*,void* buf) {
	    mActionNamed* r=static_cast<mActionNamed*>(buf);
	    r->id=id;
	}
      private:
	int id;
    };

    class edit_event : public event {
      public:
	edit_event(TextField* field1)
	: multiline(field1->isMultiline()),password(field1->isPassword()),
	  str(field1->getValue()),maxlen(field1->getMaxLen()),
	  quadding(field1->getQuadding()),field(field1) {
	  field->getAnnot()->getRect(&x1,&y1,&x2,&y2);
	}
	virtual int get_id() const { return actionEditText; }
	virtual size_t get_size() { return (sizeof(mActionEditText)+str.getLength()+4)&~3; }
	virtual void write(AOutputDev* AOut,void* buf) {
	    mActionEditText* p=static_cast<mActionEditText*>(buf);
	    int xa1,ya1,xa2,ya2;
	    AOut->cvtUserToDev(x1,y1,&xa1,&ya1);
	    AOut->cvtUserToDev(x2,y2,&xa2,&ya2);
	    if(xa1>xa2) {
		int t=xa1; xa1=xa2; xa2=t;
	    }
	    if(ya1>ya2) {
		int t=ya1; ya1=ya2; ya2=t;
	    }
	    p->field=field;
	    p->x=xa1;
	    p->y=ya1;
	    p->width=xa2-xa1+1;
	    p->height=ya2-ya1+1;
	    p->multiline=multiline;
	    p->password=password;
	    p->quadding=quadding;
	    p->maxlen=maxlen;
	    memcpy(p->text,str.getCString(),str.getLength()+1);
	    /*char* q=p->text;
	    const char* r=str->getCString();
	    int len=str->getLength();
	    while(--len>=0) {
		*q++=*r=='\r'?'\n':*r;
		++r;
	    }
	    *q++='\0';*/
//printf("edit(%s)\n",p->text);
	}
      private:
	TextField* field;
	GString str;
	int multiline;
	int password;
	int maxlen;
	int quadding;
	double x1,y1,x2,y2;
    };

    class choice_event : public event {
      public:
	choice_event(ChoiceField* field1)
	: str(field1->getValue()),quadding(field1->getQuadding()),field(field1) {
	  field->getAnnot()->getRect(&x1,&y1,&x2,&y2);
	}
	virtual int get_id() const { return actionEditChoice; }
	virtual size_t get_size() {
	    size_t s=(sizeof(mActionEditChoice)+
		      sizeof(char*)*(field->getNumEntries()+1)+
		      str.getLength()+4)&~3;
	    int n=field->getNumEntries();
	    for(int k=0;k<n;++k) {
		if(GString* t=field->getEntry(k)) {
		    s+=t->getLength()+1;
		    delete t;
		}
	    }
	    return s;
	}
	virtual void write(AOutputDev* AOut,void* buf) {
	    mActionEditChoice* p=static_cast<mActionEditChoice*>(buf);
	    int xa1,ya1,xa2,ya2;
	    AOut->cvtUserToDev(x1,y1,&xa1,&ya1);
	    AOut->cvtUserToDev(x2,y2,&xa2,&ya2);
	    if(xa1>xa2) {
		int t=xa1; xa1=xa2; xa2=t;
	    }
	    if(ya1>ya2) {
		int t=ya1; ya1=ya2; ya2=t;
	    }
	    p->field=field;
	    p->x=xa1;
	    p->y=ya1;
	    p->width=xa2-xa1+1;
	    p->height=ya2-ya1+1;
	    p->quadding=quadding;
	    p->editflag=field->isEditable();
	    p->popupflag=field->isPopUp();
	    char* s=p->value;
	    memcpy(s,str.getCString(),str.getLength()+1);
	    s+=str.getLength()+1;
	    char** q=(char**)(p->value+((s-p->value+3)&~3));
	    int n=field->getNumEntries();
	    s=(char*)(q+n+1);
	    p->entries=q;
	    for(int k=0;k<n;++k) {
		if(GString* t=field->getEntry(k)) {
		    *q++=s;
		    memcpy(s,t->getCString(),t->getLength()+1);
		    s+=t->getLength()+1;
		    delete t;
		}
	    }
	    *q=NULL;
	}
      private:
	ChoiceField* field;
	GString str;
	int quadding;
	double x1,y1,x2,y2;
    };
  public:

    MyEventHandler()
	: events(NULL),last_event(&events),num(0),size(sizeof(ActionInfo)) {}

    virtual void openDoc(GString*,GBool);
    virtual void goToPage(int num, PDFRectangle*, FitMode);
    virtual void showText(GString*);
    virtual void refresh();
    virtual void refresh(Annot*);
    virtual void changeAnnotState(Annot*);
    virtual void launch(GString*,GBool);
    virtual void URI(GString*,GBool);
    virtual void namedAction(const char*);
    virtual void editText(TextField*);
    virtual void editChoice(ChoiceField*);

    int get_num() const { return num; }
    size_t get_size() const { return num?size:0; }
    void add(event* e);
    void remove_events(int id);

    event* pop();

  private:
    event* events;
    event** last_event;
    int num;
    size_t size;
};

void MyEventHandler::openDoc(GString* file,GBool f) {
    remove_events(actionRefresh);
    remove_events(actionUpdateRect);
    add(new opendoc_event(file,f));
}

void MyEventHandler::goToPage(int num, PDFRectangle* r, FitMode m) {
    add(new goto_event(num,r,m));
}

void MyEventHandler::showText(GString *str) {
    str=toLatin1(str);
    add(new show_text_event(str));
    delete str;
}

void MyEventHandler::refresh() {
    remove_events(actionRefresh);
    remove_events(actionUpdateRect);
    remove_events(actionUpdateAnnots);
    add(new refresh_event);
}

void MyEventHandler::refresh(Annot* a) {
    if(a)
	add(new rect_update_event(a));
}

void MyEventHandler::changeAnnotState(Annot*) {
    add(new update_annots_event);
}

void MyEventHandler::launch(GString* str,GBool f) {
    add(new launch_event(str,f));
}

void MyEventHandler::URI(GString* str,GBool f) {
    add(new URI_event(str,f));
}

void MyEventHandler::namedAction(const char* str) {
    int id;
    if(!strcmp(str,"FirstPage"))
	id=actionFirstPage;
    else if(!strcmp(str,"LastPage"))
	id=actionLastPage;
    else if(!strcmp(str,"NextPage"))
	id=actionNextPage;
    else if(!strcmp(str,"PrevPage"))
	id=actionPrevPage;
    else if(!strcmp(str,"GoBack"))
	id=actionGoBack;
    else {
	error(-1,"Unknown action: %s",str);
	return;
    }
    add(new named_event(id));
}

void MyEventHandler::editText(TextField* field) {
    add(new edit_event(field));
}

void MyEventHandler::editChoice(ChoiceField* field) {
    add(new choice_event(field));
}

void MyEventHandler::add(event* e) {
    *last_event=e;
    last_event=&e->next;
    ++num;
    size+=e->get_size()+sizeof(ActionInfo);
}

MyEventHandler::event* MyEventHandler::pop() {
    event* e=events;
    if(e) {
	events=e->next;
	if(!events)
	    last_event=&events;
	--num;
	size-=e->get_size()+sizeof(ActionInfo);
    }
    return e;
}

void MyEventHandler::remove_events(int id) {
    event **p=&events;
    while(*p) {
	if((*p)->get_id()==id) {
	    event* q=*p;
	    *p=(*p)->next;
	    delete q;
	    --num;
	} else
	    p=&(*p)->next;
    }
    last_event=p;
}

MyEventHandler myEventHandler;
ActionContext actionContext(&myEventHandler);

LINKAGE Action* set_annot_state(PDFDoc *doc,Annot *annot,int state) {
    DEBUG_INFO
    Action *a=NULL;
    try {
	if(annot && doc) {
/*static const char* msg[]={"Enter","Exit","Down","Up"};
printf("%s annot %x\n",msg[state],annot);*/
	    doc->activate();

	    if(AdditionalAction *aa=annot->getAdditionalAction())
		a=aa->getAction(ActionTrigger(state));
	    else if(state==trigUp && annot->getKind()==annotLink)
		a=((LinkAnnot*)annot)->getAction();
	    /*if(a) {
		ActionContext c(doc,&myEventHandler);
		a->execute(&c);
	    }*/

	    static const AnnotAppearance app[]={
		appearanceRollover,
		appearanceNormal,
		appearanceDown,
		appearanceRollover
	    };
	    if(state==3 && !annot->hasAppearance(appearanceRollover))
		state=1;
	    if(annot->hasAppearance(app[state])) {
		if(annot->setAppearance(app[state]))
		    myEventHandler.refresh(annot);
	    }
	}
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
//printf("set_annot_state=%d\n",myEventHandler.get_size());
    return a;//myEventHandler.get_size();
}

LINKAGE Action* doaction(PDFDoc* doc,AOutputDev* aout,Action* action,size_t* sz) {
    DEBUG_INFO
    DB(printf("doaction(%lx,%lx,%lx)\n",doc,aout,action);)
    if(action && doc && aout) {
	try {
	    doc->activate();
	    actionContext.setDoc(doc);
	    action=action->execute(&actionContext);
	}
	catch(const char* msg) {
	    printf("Exception: %s\n",msg);
	    action=NULL;
	}
	catch(...) {
	    printf("Exception caught.\n");
	    action=NULL;
	}
    } else
	action=NULL;
    *sz=myEventHandler.get_size();
    return action;
}

LINKAGE int get_events(AOutputDev *AOut,struct ActionInfo* buf) {
    int num=myEventHandler.get_num();
    if(!AOut || !num)
	return 0;
    int r=0;
    try {
	char* p=(char*)(buf+num+1);
	do {
	    MyEventHandler::event* e=myEventHandler.pop();
	    buf->id=e->get_id();
	    buf->action=p;
//printf("get_events(%d)\n",buf->id);
	    e->write(AOut,p);
	    p+=e->get_size();
	    ++buf;
	    delete e;
	} while(--num);
	buf->id=actionEnd;
//printf("get_events(0)\n");
	r=1;
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
    return r;
}

LINKAGE size_t set_textfield(TextField* field,const char* txt) {
    DEBUG_INFO
    try {
	field->update(&actionContext,new GString((char*)txt));
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
    return myEventHandler.get_size();
}

LINKAGE size_t set_choicefield(ChoiceField* field,int num,const char* txt) {
    DEBUG_INFO
    try {
	field->update(&actionContext,new GString(txt));
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
    return myEventHandler.get_size();
}

LINKAGE int import_fdf(PDFDoc* doc,const char* filename) {
    DEBUG_INFO
    int r=0;
    try {
	AcroForm* form=doc->getCatalog()->getAcroForm();
	if(form && form->importFDF(new GString(filename)))
	    r=1;
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
    return r;
}

LINKAGE int export_fdf(PDFDoc* doc,const char* filename) {
    DEBUG_INFO
    int r=0;
    try {
	AcroForm* form=doc->getCatalog()->getAcroForm();
	if(form && form->exportFDF(new GString(filename)))
	    r=1;
    }
    catch(const char* msg) {
	printf("Exception: %s\n",msg);
    }
    catch(...) {
	printf("Exception caught.\n");
    }
    return r;
}

