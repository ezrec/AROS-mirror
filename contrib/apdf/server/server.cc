//========================================================================
//
// server.cc
//
// Copyright 1999-2002 Emmanuel Lesueur
//
//========================================================================

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <exec/types.h>
#include <dos/dos.h>
#undef Object
#include "apdf/AComm.h"
#include "Object.h"
#include "AOutputDev.h"
#include "apdf/io_client.h"

#define DB(x) //x

#undef malloc
#undef realloc
#undef calloc
#undef free
#undef new
#undef delete

extern volatile int *abortFlagPtr;
extern volatile int *drawStatePtr;
extern int lastDrawState;
extern struct Error *errors;

void signal_msg();

int init(int errors,volatile int*,volatile int*);
void use_prefs(int,const char*,size_t,size_t);
int get_page_size(PDFDoc*,int page,int* width,int* height,int* rotate);
PDFDoc* create_doc(BPTR,const char* filename,int*,const char*,const char*);
void delete_doc(PDFDoc*);
AOutputDev* create_output_dev(UBYTE *map,int size);
void delete_output_dev(AOutputDev*);
int simple_find(AOutputDev* aout,const char* str,int top,int* xmin,int* ymin,int* xmax,int* ymax);
int init_find(const char*);
int find(PDFDoc* doc,int start,int stop,int* xmin,int* ymin,int* xmax,int* ymax,int* page);
void end_find();
//int chklink(PDFDoc* doc,AOutputDev* aout,int x,int y,Action** action,char* str,size_t len);
//int doaction(PDFDoc* doc,AOutputDev* aout,Action** actionp,int* state,int* page,int* x,int* y,char* buf,size_t len);
OutputDev* init_write(PDFDoc* doc,const char* filename,int type,int first_page,int last_page,int zoom,int rotate);
int writefile(PDFDoc* doc,OutputDev* sv,int first_page,int last_page,int zoom,int rotate);
void end_write(OutputDev* sv);
char* gettext(AOutputDev*,int,int,int,int);
int show_page(AOutputDev*,PDFDoc*,int,int,int,unsigned char*,int,int,int,int,int,int,int,int);
void add_fontmap(PDFDoc*,const char*,const char*,int);
void clear_fontmap(PDFDoc*);
FontOutputDev* init_scan();
int scan_fonts(PDFDoc*,FontOutputDev*,int,int);
int scan_default_fonts();
void end_scan_fonts(FontOutputDev*);
int get_font(FontOutputDev*,int,char*,int,char*,int,int*,int*);
int has_outlines(PDFDoc* doc);
int get_outlines_info(PDFDoc* doc,int* num,size_t* bufsize);
void get_outlines(PDFDoc* doc,OutlinesInfo* outlines,size_t bufsize);
char* get_doc_info(PDFDoc* doc,size_t*,int*,int*,int*,int*);
void get_version(char*,size_t);
void set_AA_levels(AOutputDev*,int,int,int);
void clear_font_path();
void add_font_path(const char *);
void get_page_label(PDFDoc*,int,char *,size_t);
const char** get_page_labels(PDFDoc*,size_t*,int*);
int get_page_index(PDFDoc*,const char *);
int get_error(char*,size_t);
int get_num_annots(PDFDoc*,int,size_t*);
int get_annots(PDFDoc*,AOutputDev*,int,AnnotInfo*,int);
Action* set_annot_state(PDFDoc*,Annot*,int);
Action* doaction(PDFDoc*,AOutputDev*,Action*,size_t*);
int get_events(AOutputDev*,struct ActionInfo*);
size_t set_textfield(TextField*,const char*);
size_t set_choicefield(ChoiceField*,int,const char*);
int import_fdf(PDFDoc*,const char*);
int export_fdf(PDFDoc*,const char*);

#ifdef DEBUGMEM
const char* memalloc_file;
int memalloc_line;
#endif

#ifndef NO_CUSTOM_MEM
void* myAllocMem(size_t);
void myFreeMem(void*,size_t);

struct memchunk {
    memchunk* next;
    memchunk* prev;
    size_t size;
    size_t free;
};

#ifdef DEBUGMEM
static const int begwall=8;
static const int endwall=16;
#else
static const int begwall=0;
static const int endwall=0;
#endif

struct memblock;

struct free_node {
    free_node* next;
    free_node* prev;

    void remove() {
	next->prev=prev;
	prev->next=next;
#ifdef DEBUGMEM
	next=NULL;
	prev=NULL;
#endif
    }
};

struct free_list {
    free_node* head;
    free_node* tail;
    free_node* tailpred;

    void addtail(free_node* n) {
	n->prev=tailpred;
	n->next=(free_node*)&tail;
	tailpred->next=n;
	tailpred=n;
    }
};

struct memblock {
    size_t size;
    memblock* pred;
#ifdef DEBUGMEM
    memblock* next;
    memblock* prev;
    const char* file;
    int line;
    size_t alloc_size;
    int pad;
#endif

    bool is_free() const { return !(size&1); }
    bool is_first() const {
	return pred==NULL;
    }
    bool is_last() const {
	return *(size_t*)((char*)(this+1)+begwall+endwall+(size&~1))==0;
    }
    memblock* succ() const {
	if(!is_last())
	    return (memblock*)((char*)(this+1)+begwall+endwall+(size&~1));
	else
	    return NULL;
    }
    void adjust_successors_pred() {
	if(memblock* p=succ())
	    p->pred=this;
    }
    void unlink_free() {
	((free_node*)(this+1))->remove();
    }
    void add_to_freelist(free_list& l) {
	l.addtail((free_node*)(this+1));
    }
    void add_to_freelist(free_node& n) {
	free_node* p=(free_node*)(this+1);
	p->prev=n.prev;
	p->next=&n;
	n.prev->next=p;
	n.prev=p;
    }
};

memblock* get_memblock(free_node& n) {
    return (memblock*)&n-1;
}

#ifdef DEBUGMEM
extern memblock tail;
memblock head={0,0,&tail,NULL};
memblock tail={0,0,NULL,&head};

int sizes[1000];
int bigger;

bool check(memblock* p) {
    bool err=false;
    unsigned char* t=(unsigned char*)(p+1);
    for(int k=0;k<begwall;++k)
	if(*t++!=0x55) {
	    printf("##### block %p (%u,%s,%d): bad header: %02x\n",p,p->size,p->file?p->file:"?",p->line,t[-1]);
	    err=true;
	}
    t+=p->alloc_size;
    size_t sz=endwall+(p->size&~1)-p->alloc_size;
    for(int k=0;k<sz;++k)
	if(*t++!=0xaa) {
	    printf("##### block %p (%u,%s,%d): bad trailer: %02x\n",p,p->size,p->file?p->file:"?",p->line,t[-1]);
	    err=true;
	}
    return err;
}

void checkall() {
    printf("checking memory\n");
    size_t sz=0;
    int n=0;
    for(memblock* p=head.next;p->next;p=p->next) {
	sz+=p->size&~1;
	++n;
	check(p);
    }
    printf("%u bytes allocated, %d blocks\n",sz,n);
}

extern "C" void printunfreed() {
    printf("Unfreed memory:\n");
    for(memblock* p=head.next;p->next;p=p->next)
	printf("%x\t%d\t%d\t%s\n",(char*)(p+1)+begwall,p->alloc_size,p->line,p->file?p->file:"?");
    for(int k=0;k<1000;++k)
	if(sizes[k]) {
	    printf("sz=%d: %d\n",(k+1)*8,sizes[k]);
	}
    printf("bigger: %d\n",bigger);
}
#endif

struct memlist {
    memchunk* head;
    memchunk* tail;
    memchunk* tailpred;
};

static const int num_small_blocks=1000;
static const int num_small_chunks=8;
static const size_t block_quantum=8;
static const size_t tresh_size=32768;
static const size_t puddle_size=128*1024;
static const size_t small_puddle_size=16*1024;
static free_list small_blocks[num_small_blocks];
static free_list big_blocks={(free_node*)&big_blocks.tail,NULL,(free_node*)&big_blocks.head};
static memlist chunks={(memchunk*)&chunks.tail,NULL,(memchunk*)&chunks.head};
static memlist small_chunks[num_small_chunks];

extern "C" int meminit() {
    for(int k=0;k<num_small_blocks;++k) {
	small_blocks[k].head=(free_node*)&small_blocks[k].tail;
	small_blocks[k].tailpred=(free_node*)&small_blocks[k].head;
    }
    for(int k=0;k<num_small_chunks;++k) {
	small_chunks[k].head=(memchunk*)&small_chunks[k].tail;
	small_chunks[k].tailpred=(memchunk*)&small_chunks[k].head;
    }
    return 0;
}

extern "C" void memcleanup() {
    memchunk* q=chunks.head;
    while(q->next) {
	memchunk* t=q->next;
	myFreeMem(q,q->size);
	q=t;
    }
    for(int k=0;k<num_small_chunks;++k) {
	memchunk* q=small_chunks[k].head;
	while(q->next) {
	    memchunk* t=q->next;
	    myFreeMem(q,q->size);
	    q=t;
	}
    }
}

static void free_block(memblock* p) {
#ifdef DEBUGMEM
    if(!p->is_free()) {
	printf("##### block %p (%u,%s,%d): not free\n",p,p->size,p->file?p->file:"?",p->line);
	return;
    }
#endif
    if(p->size<=num_small_chunks*block_quantum) {
	p->add_to_freelist(small_blocks[(p->size-1)/block_quantum]);
    } else {
	if(memblock* q=p->pred) {
	    if(q->is_free()) {
		q->unlink_free();
		q->size+=p->size+sizeof(memblock)+begwall+endwall;
		p=q;
		p->adjust_successors_pred();
	    }
	}
	if(memblock* q=p->succ()) {
	    if(q->is_free()) {
		q->unlink_free();
		p->size+=q->size+sizeof(memblock)+begwall+endwall;
		p->adjust_successors_pred();
	    }
	}
	if(p->is_first() && p->is_last()) {
	    memchunk* q=(memchunk*)p-1;
	    q->next->prev=q->prev;
	    q->prev->next=q->next;
	    myFreeMem(q,q->size);
	} else {
	    if(p->size<=num_small_blocks*block_quantum) {
		p->add_to_freelist(small_blocks[(p->size-1)/block_quantum]);
	    } else {
		free_node* q=big_blocks.head;
		while(q->next && get_memblock(*q)->size<p->size)
		    q=q->next;
		p->add_to_freelist(*q);
	    }
	}
    }
}

extern "C" void* malloc(size_t sz) {
    size_t sz2=(sz+7)&~7;
    memblock* p=NULL;
    if(sz2==0)
	sz2=8;
    if (sz2<=num_small_chunks*block_quantum) {
	size_t k=(sz2-1)/block_quantum;
	free_node* q=small_blocks[k].head;
	if(q->next) {
	    p=get_memblock(*q);
	    p->unlink_free();
	} else {
	    memchunk* q=small_chunks[k].tailpred;
	    size_t sz3=sz2+sizeof(memblock)+begwall+endwall;
	    if (!q->prev || sz3>q->free) {
		const size_t chunk_size=small_puddle_size;
		q=(memchunk*)myAllocMem(chunk_size);
		if(!q)
		    return NULL;
		q->next=(memchunk*)&small_chunks[k].tail;
		q->prev=small_chunks[k].tailpred;
		small_chunks[k].tailpred->next=q;
		small_chunks[k].tailpred=q;
		q->size=chunk_size;
		q->free=q->size-((sizeof(memchunk)+7)&~7);
	    }
	    p=(memblock*)((char*)q+q->size-q->free);
	    p->size=sz2;
#ifdef DEBUGMEM
	    p->pred=(memblock*)0xdeadbeef;
#endif
	    q->free-=sz3;
	}
    } else {
	if(sz2<=num_small_blocks*block_quantum) {
	    size_t k=(sz2-1)/block_quantum;
	    do {
		free_node* q=small_blocks[k].head;
		if(q->next) {
		    p=get_memblock(*q);
		    p->unlink_free();
		    break;
		}
		++k;
	    } while(k<num_small_blocks);
	}
	if(!p) {
	    free_node* t=big_blocks.head;
	    while(t->next && get_memblock(*t)->size<sz2)
		t=t->next;
	    if(t->next) {
		p=get_memblock(*t);
		p->unlink_free();
	    } else {
		size_t chunk_size=sz2+((sizeof(memblock)+begwall+endwall+sizeof(memchunk)+sizeof(size_t)+7)&~7);
		if(chunk_size<tresh_size)
		    chunk_size=puddle_size;
		memchunk* q=(memchunk*)myAllocMem(chunk_size);
		if(!q)
		    return NULL;
		q->next=(memchunk*)&chunks.tail;
		q->prev=chunks.tailpred;
		chunks.tailpred->next=q;
		chunks.tailpred=q;
		q->size=chunk_size;
		q->free=0;
		chunk_size-=(sizeof(memchunk)+sizeof(size_t)+7)&~7;
		*(size_t*)((char*)(q+1)+chunk_size)=0;
		p=(memblock*)(q+1);
		p->size=chunk_size-sizeof(memblock)-begwall-endwall;
		p->pred=NULL;
	    }
	}
	if(p->size-sz2>=sizeof(memblock)+begwall+endwall+block_quantum) {
	    memblock* q=(memblock*)((char*)(p+1)+begwall+endwall+sz2);
	    q->size=p->size-sz2-sizeof(memblock)-begwall-endwall;
	    q->pred=p;
	    p->size=sz2;
	    q->adjust_successors_pred();
	    p->size|=1;
	    free_block(q);
	}
    }
    p->size|=1;
#ifdef DEBUGMEM
    if(sz>p->size-1) {
	printf("##### invalid block size\n");
	return NULL;
    }
    p->alloc_size=sz;
    if(sz2<=8000) {
	if(sz2==0)
	    sz2=1;
	++sizes[(sz2-1)>>3];
    } else
	++bigger;
    p->next=&tail;
    p->prev=tail.prev;
    p->prev->next=p;
    tail.prev=p;
    p->file=memalloc_file;
    p->line=memalloc_line;
    memalloc_file=NULL;
    unsigned char* q=(unsigned char*)(p+1);
    for(int k=0;k<begwall;++k)
	*q++=0x55;
    for(int k=0;k<sz;++k)
	*q++=0xbb;
    size_t sz3=endwall+(p->size&~1)-sz;
    for(int k=0;k<sz3;++k)
	*q++=0xaa;
#endif
    return (char*)(p+1)+begwall;
}

extern "C" void* calloc(size_t n,size_t sz) {
    sz*=n;
    void* p=malloc(sz);
    if(p && sz)
	memset(p,0,sz);
    return p;
}

extern "C" void* realloc(void* t,size_t sz) {
    if(!t)
	return malloc(sz);
    memblock* p=(memblock*)((char*)t-begwall)-1;
#ifdef DEBUGMEM
    check(p);
#endif
    size_t oldsz=p->size&~1;
    size_t sz2=(sz+7)&~7;
    if(sz2==0)
	sz2=8;
    if(sz2>num_small_chunks*block_quantum && oldsz>num_small_chunks*block_quantum) {
	if(sz2>oldsz) {
	    if(memblock* q=p->succ()) {
		if(q->is_free() &&
			q->size+sizeof(memblock)+begwall+endwall>=sz2-oldsz) {
		    q->unlink_free();
		    oldsz+=q->size+sizeof(memblock)+begwall+endwall;
		    p->size=oldsz;
		    p->adjust_successors_pred();
		    p->size|=1;
#ifdef DEBUGMEM
		    unsigned char* t=(unsigned char*)(p+1)+begwall+p->alloc_size;
		    for(int k=0;k<sz-p->alloc_size;++k)
			*t++=0xbb;
		    p->alloc_size=sz;
		    size_t sz3=endwall+oldsz-sz;
		    for(int k=0;k<sz3;++k)
			*t++=0xaa;
#endif
		}
	    }
	}
	if(sz2<=oldsz) {
	    if(oldsz-sz2>=sizeof(memblock)+begwall+endwall+block_quantum) {
		memblock* q=(memblock*)((char*)(p+1)+begwall+endwall+sz2);
		q->size=oldsz-sz2-sizeof(memblock)-begwall-endwall;
		q->pred=p;
		p->size=sz2|1;
		q->adjust_successors_pred();
		free_block(q);
	    }
#ifdef DEBUGMEM
	    p->alloc_size=sz;
	    unsigned char* q=(unsigned char*)(p+1)+begwall+sz;
	    size_t sz3=endwall+(p->size&~1)-sz;
	    for(int k=0;k<sz3;++k)
		*q++=0xaa;
#endif
	    return (char*)(p+1)+begwall;
	}
    }
    void* q=malloc(sz);
    if(q) {
	if(oldsz && sz)
	    memcpy(q,t,sz<oldsz?sz:oldsz);
	free(t);
#if 0// def DEBUGMEM
	unsigned char* r=(unsigned char*)q+oldsz;
	size_t sz4=sz-oldsz;
	for(int k=0;k<sz4;++k)
	    *r++=0xbb;
#endif
    }
    return q;
}

extern "C" void free(void* p) {
    if(p) {
	memblock* q=(memblock*)((char*)p-begwall)-1;
#ifdef DEBUGMEM
	if(q->is_free() || q->next==NULL || q->prev==NULL) {
	    printf("##### free called at %s/%d\n",memalloc_file,memalloc_line);
	    printf("##### block %p (%u,%s,%d): already freed\n",q,q->size,q->file?q->file:"?",q->line);
	    checkall();
	    memalloc_file=NULL;
	    return;
	}
#endif
	q->size&=~1;
#ifdef DEBUGMEM
	if(check(q))
	    checkall();
	unsigned char* t=(unsigned char*)(q+1)+begwall;
	for(int k=0;k<q->alloc_size;++k)
	    *t++=0xdd;
	q->next->prev=q->prev;
	q->prev->next=q->next;
	q->next=NULL;
	q->prev=NULL;
#endif
	free_block(q);
    }
#ifdef DEBUGMEM
    memalloc_file=NULL;
#endif
}

void* operator new (size_t sz) {
    void* p=malloc(sz);
    if(!p)
	throw "Out of memory!";
    return p;
}

void operator delete (void* p) {
    free(p);
}

void* operator new [] (size_t sz) {
    void* p=malloc(sz);
    if(!p)
	throw "Out of memory!";
    return p;
}

void operator delete [] (void* p) {
    free(p);
}
#endif

extern int maxColors;

struct comm_info *(*create_comm_info)(size_t);
void (*delete_comm_info)(struct comm_info *);
int (*exchange_msg)(struct comm_info *,struct msg_base *,size_t,int);
//void (*send_msg)(struct comm_info *,void *,size_t,int);
void* (*get_msg)(struct server_info *,void **,size_t *,int *);
void (*reply_msg)(void *);

#if 0
class ServColorAllocator : public ColorAllocator {
  public:
    ServColorAllocator(int depth);
    ~ServColorAllocator();
    virtual Guchar allocate(Gulong);
    virtual void allocate(int,ColorEntry*);
  private:
    comm_info* ci;
};

ServColorAllocator::ServColorAllocator(int depth) {
    size_t sz=sizeof(ppcmsg_max);
    if(depth<=8) {
	size_t sz2=sizeof(msg_getcolors)+sizeof(ColorEntry)*maxColors;
	if(sz<sz2)
	    sz=sz2;
    }
    if(!(ci=create_comm_info(sz)))
	throw "Can't create message & port.";
}

ServColorAllocator::~ServColorAllocator() {
    delete_comm_info(ci);
}

Guchar ServColorAllocator::allocate(Gulong val) {
    msg_getcolor* m=(msg_getcolor*)ci->cmddata;
    m->r=((val>>16)&0xff)*0x01010101;
    m->g=((val>> 8)&0xff)*0x01010101;
    m->b=((val>> 0)&0xff)*0x01010101;
    exchange_msg(ci,&m->base,sizeof(*m),MSGID_GETCOLOR);
    return Guchar(m->base.success);
}

void ServColorAllocator::allocate(int num,ColorEntry* p) {
    if(num>maxColors) {
	printf("too many colors (%d/%d).\n",num,maxColors);
	return;
    }
    msg_getcolors* m=(msg_getcolors*)ci->cmddata;
    m->num=num;
    memcpy(m->table,p,num*sizeof(ColorEntry));
    exchange_msg(ci,&m->base,sizeof(*m)-sizeof(m->table[0])+num*sizeof(ColorEntry),MSGID_GETCOLORS);
    memcpy(p,m->table,num*sizeof(ColorEntry));
    /*unsigned long* q=m->table;
    while(--num>=0) {
	p->index=*q;
	++p;
	q+=4;
    }*/
}
#endif


void server(comm_info *ci) {
    bool running=true;
    IoServer io_server = NULL;
    bool init_ok=false;
    do {
	void* msgdat;
	size_t len;
	int id;
	void* msg=get_msg(NULL,&msgdat,&len,&id);
	DB(printf("-- msg = %d\n",id);)
	switch(id) {
	  case MSGID_INIT: {
	    msg_init* p=(msg_init*)msgdat;
	    p->base.success=init_ok=(p->version==VMAGIC && initClientIo(io_server=p->io_server) && init(p->errors,p->abort_flag_ptr,p->draw_state_ptr));
	    break;
	  }
	  case MSGID_QUIT:
	    running=false;
	    break;

	  case MSGID_GETERROR: {
	    msg_error* p=(msg_error*)msgdat;
	    p->base.success=get_error(p->msg,sizeof(p->msg));
	    break;
	  }
	  case MSGID_USEPREFS: {
	    msg_prefs* p=(msg_prefs*)msgdat;
	    use_prefs(p->colors,p->gzipcmd,p->cachesz,p->cacheblocsz);
	    p->base.success=1;
	    break;
	  }
	  case MSGID_CREATEDOC: {
	    msg_createdoc* m=(msg_createdoc*)msgdat;
	    m->doc=create_doc(m->dir,m->filename,&m->num_pages,m->ownerpw,m->userpw);
	    m->base.success=m->doc!=NULL;
	    break;
	  }
	  case MSGID_DELETEDOC: {
	    msg_deletedoc* m=(msg_deletedoc*)msgdat;
	    delete_doc(m->doc);
	    m->base.success=1;
	    break;
	  }
	  case MSGID_PAGESIZE: {
	    msg_pagesize* m=(msg_pagesize*)msgdat;
	    m->base.success=get_page_size(m->doc,m->page,&m->width,&m->height,&m->rotate);
	    break;
	  }
	  case MSGID_CREATEOUTPUTDEV: {
	    msg_create_output_dev* m=(msg_create_output_dev*)msgdat;
	    m->out=NULL;
	    //ColorAllocator* colorAllocator=NULL;
	    try {
		//colorAllocator=new ServColorAllocator(m->depth);
		//m->out=create_output_dev(colorAllocator);
		//colorAllocator=NULL;
		m->out=create_output_dev(m->map,16);
	    }
	    catch(const char* msg) {
		printf("Exception: %s\n", msg);
	    }
	    catch(...) {
		printf("Exception caught.");
	    }
	    //delete colorAllocator;
	    m->base.success=m->out!=NULL;
	    break;
	  }
	  case MSGID_DELETEOUTPUTDEV: {
	    msg_delete_output_dev* m=(msg_delete_output_dev*)msgdat;
	    delete_output_dev(m->out);
	    m->base.success=1;
	    break;
	  }
#if 0
	  case MSGID_SETAALEVELS: {
	    msg_setaalevels* m=(msg_setaalevels*)msgdat;
	    set_AA_levels(m->out,m->text,m->stroke,m->fill);
	    m->base.success=1;
	    break;
	  }
#endif
	  case MSGID_PAGE: {
	    msg_page* m=(msg_page*)msgdat;
	    PDFDoc* doc=m->doc;
	    AOutputDev* aout=m->aout;
	    int n=m->page_num;
	    int z=m->zoom;
	    int r=m->rotate;
	    int x=m->xoffset;
	    int y=m->yoffset;
	    int w=m->width;
	    int h=m->height;
	    int mod=m->modulo;
	    int bpp=m->bpp;
	    int pixfmt=m->pixfmt;
	    int lev=m->level;
	    m->base.success=1;
	    m->base.error=errors!=NULL;
	    *drawStatePtr=lastDrawState=0;
	    reply_msg(msg);
	    msg=get_msg(NULL,&msgdat,&len,&id);
	    DB(printf("-- abort=%d, level=%d\n-- show_page\n",*abortFlagPtr,lev);)
	    lev=show_page(aout,doc,n,z,r,
			  (unsigned char*)msgdat,
			  x,y,w,h,mod,bpp,pixfmt,lev);
	    DB(printf("-- abort=%d, level=%d\n",*abortFlagPtr,lev);)
	    reply_msg(msg);
	    signal_msg();
	    msg=get_msg(NULL,&msgdat,&len,&id);
	    m=(msg_page*)msgdat;
	    m->returned_level=*abortFlagPtr?-1:lev;
	    m->base.success=1;
	    break;
	  }
#if 0
	  case MSGID_CHKLINK: {
	    msg_chklink* m=(msg_chklink*)msgdat;
	    m->base.success=chklink(m->doc,m->aout,m->x,m->y,&m->action,m->str,sizeof(m->str));
	    break;
	  }
	  case MSGID_DOACTION: {
	    msg_doaction* m=(msg_doaction*)msgdat;
	    m->base.success=doaction(m->doc,m->aout,&m->action,&m->state,&m->page,&m->x,&m->y,m->str,sizeof(m->str));
	    break;
	  }
#endif
	  case MSGID_SIMPLEFIND: {
	    msg_find* m=(msg_find*)msgdat;
	    m->base.success=simple_find(m->aout,m->str,m->top,&m->xmin,&m->ymin,&m->xmax,&m->ymax);
	    break;
	  }
	  case MSGID_INITFIND: {
	    msg_find* m=(msg_find*)msgdat;
	    m->base.success=init_find(m->str);
	    break;
	  }
	  case MSGID_FIND: {
	    msg_find* m=(msg_find*)msgdat;
	    m->base.success=find(m->doc,m->top,m->bottom,&m->xmin,&m->ymin,&m->xmax,&m->ymax,&m->page);
	    break;
	  }
	  case MSGID_ENDFIND:
	    end_find();
	    break;
	  case MSGID_INITWRITE: {
	    msg_write* m=(msg_write*)msgdat;
	    m->savedev=init_write(m->doc,m->filename,m->type,m->first_page,m->last_page,m->zoom,m->rotate);
	    m->base.success=m->savedev!=NULL;
	    break;
	  }
	  case MSGID_WRITE: {
	    msg_write* m=(msg_write*)msgdat;
	    writefile(m->doc,m->savedev,m->first_page,m->last_page,m->zoom,m->rotate);
	    m->base.success=1;
	    break;
	  }
	  case MSGID_ENDWRITE: {
	    msg_write* m=(msg_write*)msgdat;
	    end_write(m->savedev);
	    m->base.success=1;
	    break;
	  }

	  case MSGID_GETTEXT: {
	    msg_gettext* m=(msg_gettext*)msgdat;
	    char* p=gettext(m->aout,m->xmin,m->ymin,m->xmax,m->ymax);
	    m->base.success=p!=NULL;
	    m->base.error=errors!=NULL;
	    if(m->base.success) {
		m->size=strlen(p);
		reply_msg(msg);
		msg=get_msg(NULL,&msgdat,&len,&id);
		char* q=(char*)msgdat;
		strncpy(q,p,len-1);
		q[len-1]='\0';
		free(p);
		msgdat=NULL;
	    }
	    break;
	  }
	  case MSGID_HASOUTLINES: {
	    msg_get_outlines_info* m=(msg_get_outlines_info*)msgdat;
	    m->base.success=has_outlines(m->doc);
	    break;
	  }
	  case MSGID_GETOUTLINESINFO: {
	    msg_get_outlines_info* m=(msg_get_outlines_info*)msgdat;
	    m->base.success=get_outlines_info(m->doc,&m->num,&m->bufsize);
	    break;
	  }
	  case MSGID_GETOUTLINES: {
	    msg_get_outlines* m=(msg_get_outlines*)msgdat;
	    PDFDoc* doc=m->doc;
	    m->base.success=1;
	    m->base.error=errors!=NULL;
	    reply_msg(msg);
	    msg=get_msg(NULL,&msgdat,&len,&id);
	    get_outlines(doc,(OutlinesInfo*)msgdat,0);
	    msgdat=NULL;
	    break;
	  }
	  case MSGID_GETDOCINFO: {
	    msg_getdocinfo* m=(msg_getdocinfo*)msgdat;
	    char* p=get_doc_info(m->doc,(size_t*)&m->size,&m->linearized,&m->encrypted,&m->oktoprint,&m->oktocopy);
	    m->base.success=p!=NULL;
	    m->base.error=errors!=NULL;
	    if(m->base.success) {
		reply_msg(msg);
		msg=get_msg(NULL,&msgdat,&len,&id);
		char* q=(char*)msgdat;
		memcpy(q,p,m->size>len-1?len-1:m->size);
		q[len-1]='\0';
		free(p);
		msgdat=NULL;
	    }
	    break;
	  }
	  case MSGID_ADDFONTMAP: {
	    msg_addfontmap* m=(msg_addfontmap*)msgdat;
	    add_fontmap(m->pdfdoc,m->pdffont,m->font,m->type);
	    m->base.success=1;
	    break;
	  }
	  case MSGID_CLEARFONTMAP: {
	    msg_addfontmap* m=(msg_addfontmap*)msgdat;
	    clear_fontmap(m->pdfdoc);
	    m->base.success=1;
	    break;
	  }
	  case MSGID_ADDFONTPATH: {
	    msg_addfontpath* m=(msg_addfontpath*)msgdat;
	    add_font_path(m->path);
	    m->base.success=1;
	    break;
	  }
	  case MSGID_CLEARFONTPATH:
	    clear_font_path();
	    break;
	  case MSGID_INITSCAN: {
	    msg_scan_fonts* m=(msg_scan_fonts*)msgdat;
	    m->out=init_scan();
	    m->base.success=m->out!=NULL;
	    break;
	  }
	  case MSGID_SCANFONTS: {
	    msg_scan_fonts* m=(msg_scan_fonts*)msgdat;
	    m->base.success=scan_fonts(m->doc,m->out,m->first_page,m->last_page);
	    break;
	  }
	  case MSGID_ENDSCANFONTS: {
	    msg_scan_fonts* m=(msg_scan_fonts*)msgdat;
	    end_scan_fonts(m->out);
	    m->base.success=1;
	    break;
	  }
	  case MSGID_GETFONT: {
	    msg_fontmap* m=(msg_fontmap*)msgdat;
	    m->base.success=get_font(m->out,m->m,m->pdffont,sizeof(m->pdffont),m->font,sizeof(m->font),&m->type,&m->flags);
	    break;
	  }
	  case MSGID_VERSION: {
	    msg_version* m=(msg_version*)msgdat;
	    get_version(m->version,sizeof(m->version));
	    m->base.success=1;
	    break;
	  }
#if 0
	  case MSGID_GETPAGELABEL: {
	    msg_pagelabel* m=(msg_pagelabel*)msgdat;
	    get_page_label(m->doc,m->index,m->label,sizeof(m->label));
	    m->base.success=1;
	    break;
	  }
#endif
	  case MSGID_GETPAGELABELS: {
	    msg_pagelabels* m=(msg_pagelabels*)msgdat;
	    size_t sz;
	    int n;
	    const char** p=get_page_labels(m->doc,&sz,&n);
	    m->size=p?sz:0;
	    m->base.success=1;
	    m->base.error=errors!=NULL;
	    if(m->size) {
		reply_msg(msg);
		msg=get_msg(NULL,&msgdat,&len,&id);
		if(msgdat) {
		    memcpy(msgdat,p,sz);
		    const char** q=(const char**)msgdat;
		    int offset=(char*)msgdat-(char*)p;
		    for(int k=0;k<n;++k)
			*q+++=offset;
		}
		gfree(p);
		msgdat=NULL;
	    }
	    break;
	  }
	  case MSGID_GETPAGEINDEX: {
	    msg_pagelabel* m=(msg_pagelabel*)msgdat;
	    m->index=get_page_index(m->doc,m->label);
	    m->base.success=m->index!=-1;
	    break;
	  }
	  case MSGID_GETANNOTS: {
	    msg_getannots* m=(msg_getannots*)msgdat;
	    PDFDoc* doc=m->doc;
	    AOutputDev* aout=m->aout;
	    int page=m->page;
	    int num=m->num=get_num_annots(doc,page,&m->size);
	    m->base.success=1;
	    m->base.error=errors!=NULL;
	    if(m->num) {
		reply_msg(msg);
		msg=get_msg(NULL,&msgdat,&len,&id);
		if(msgdat)
		    get_annots(doc,aout,page,(AnnotInfo*)msgdat,num);
		msgdat=NULL;
	    }
	    break;
	  }
	  case MSGID_SETANNOTSTATE: {
	    msg_setannotstate* m=(msg_setannotstate*)msgdat;
	    m->action=set_annot_state(m->pdfdoc,m->annot,m->state);
	    m->base.success=1;
	    break;
	  }
	  case MSGID_DOACTION: {
	    msg_doaction* m=(msg_doaction*)msgdat;
	    m->action=doaction(m->pdfdoc,m->aout,m->action,&m->size);
	    m->base.success=1;
	    break;
	  }
	  case MSGID_GETEVENTS: {
	    msg_getevents* m=(msg_getevents*)msgdat;
	    AOutputDev* aout=m->aout;
	    m->base.success=1;
	    m->base.error=errors!=NULL;
	    reply_msg(msg);
	    msg=get_msg(NULL,&msgdat,&len,&id);
	    if(msgdat)
		get_events(aout,(ActionInfo*)msgdat);
	    msgdat=NULL;
	    break;
	  }
	  case MSGID_SETTEXTFIELD: {
	    msg_settextfield* m=(msg_settextfield*)msgdat;
	    m->size=set_textfield(m->field,m->txt);
	    m->base.success=1;
	    break;
	  }
	  case MSGID_SETCHOICEFIELD: {
	    msg_setchoicefield* m=(msg_setchoicefield*)msgdat;
	    m->size=set_choicefield(m->field,m->num,m->txt);
	    m->base.success=1;
	    break;
	  }
	  case MSGID_IMPORTFDF: {
	    msg_fdf* m=(msg_fdf*)msgdat;
	    m->base.success=import_fdf(m->pdfdoc,m->filename);
	    break;
	  }
	  case MSGID_EXPORTFDF: {
	    msg_fdf* m=(msg_fdf*)msgdat;
	    m->base.success=export_fdf(m->pdfdoc,m->filename);
	    break;
	  }
	  default:
	    ((msg_base*)msgdat)->error=0;
	    ((msg_base*)msgdat)->success=0;
	    break;
	}
	if(msgdat)
	    ((msg_base*)msgdat)->error=errors!=NULL;
	DB(printf("-- replying\n");)
	reply_msg(msg);
    } while(running);

    if (init_ok)
	ioQuit();
    cleanupClientIo(io_server);
#ifndef NO_CUSTOM_MEM
#ifdef DEBUGMEM
    checkall();
#endif
#endif
}
