//========================================================================
//
// mystdio.cc
//
// Copyright 1999-2002 Emmanuel Lesueur
//
//========================================================================

#define DB(x) //x

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdarg.h>
#include "mystdio.h"
#include "gmem.h"
#include "PartsVec.h"
#include "apdf/io_client.h"
#ifdef __SASC
#   define throw(x)
#endif

/*class no_input_t : public input_t {
  public:
    virtual size_t read(void* buf,int pos,size_t sz) {
	return -1;
    }
};*/


class file_input_t : public input_t {
  public:
    file_input_t(const char*);

    virtual ~file_input_t() { ioClose(handle); }
    virtual size_t size() { return size1; }
    virtual void preread(int pos,size_t sz) { ioPreRead(handle, pos, sz); }
    virtual size_t read(void* buf,int pos,size_t sz) { return ioRead(handle, buf, pos, sz); }

  private:
    IoHandle handle;
    size_t size1;
};

file_input_t::file_input_t(const char *filename) {
    handle=ioOpen(filename,&size1);
    if(!handle)
	throw "Can't open file";
}


size_t myFILE::max_buf=32768;
size_t myFILE::glb_chunk_size=4096;

void myFILE::bufsizes(size_t sz1,size_t sz2) {
    if(sz1>1<<20)
	sz1=1<<20;
    else if(sz1==0)
	sz1=1;
    if(sz2>1<<20)
	sz2=1<<20;
    sz1*=1024;
    sz2*=1024;
    if(sz2==0) {
	max_buf=0x7fffffff;
	glb_chunk_size=0x7fffffff;
    } else {
	if(sz1<4096)
	    sz1=4096;
	glb_chunk_size=sz1;
	if(sz2<2*sz1)
	    sz2=2*sz1;
	max_buf=sz2;
    }
}

myFILE::myFILE(input_t* input1)
    : input(input1),memory_file(false),parts(NULL) {
    total_size=input1->size();
    if(total_size<max_buf) {
	chunk_size=total_size;
	max_loaded_chunks=2;
    } else {
	chunk_size=glb_chunk_size;
	max_loaded_chunks=max_buf/chunk_size;
    }
    num_chunks=(total_size+chunk_size-1)/chunk_size;
    chunks=new chunk [num_chunks];
    pos=0;
    num_loaded_chunks=0;
    loaded_chunks=NULL;
    oldest_chunk=NULL;
}

myFILE::myFILE(const void* p,size_t sz)
    : input(NULL),memory_file(true),parts(NULL) {
    total_size=sz;
    chunk_size=sz;
    chunks=new chunk [1];
    chunks->buf=(unsigned char*)p;
    chunks->prev=chunks->next=NULL;
    num_chunks=1;
    pos=0;
    max_loaded_chunks=2;
    num_loaded_chunks=1;
    loaded_chunks=NULL;
    oldest_chunk=NULL;
}

myFILE::~myFILE() {
    delete input;
    if(memory_file)
	chunks->buf=NULL;
    delete parts;
    delete [] chunks;
}

void myFILE::preread(int pos1,size_t sz) {
#if 0
    if(parts && input) {
	int offset1=pos1;
	int size1=sz;
	while(parts->getFirstMissingPart(&offset1,&size1)) {
	    input->preread(offset1,size1);
	    offset1+=size1;
	    size1=pos1+sz-offset1;
	}
    }
#else
    if(!parts)
	return;
    //printf("preread(%p,%d,%d)\n",input?(void*)input:(void*)this,pos1,sz);
    if(sz>total_size-pos1)
	sz=total_size-pos1;
    size_t r=sz;
    unsigned n=pos1/chunk_size;
    unsigned offs=pos1%chunk_size;
    //bool xx=false;

    while(sz) {
	//printf("sz=%u pos=%u, chunk=%u, offs=%u\n",sz,pos,n,offs);
	chunk* c=chunks+n;
	size_t sz1=chunk_size;
	if(!c->buf) {
	    /*if(!xx) {
		printf("read(%u)\n",sz);
		xx=true;
	    }*/
	    unsigned char* buf=NULL;
	    if(num_loaded_chunks==max_loaded_chunks) {
		if(parts)
		    parts->remove((oldest_chunk-chunks)*chunk_size,chunk_size);
		buf=oldest_chunk->buf;
		oldest_chunk->buf=NULL;
		oldest_chunk=oldest_chunk->prev;
		oldest_chunk->next=NULL;
	    } else {
		++num_loaded_chunks;
		buf=new unsigned char [chunk_size];
	    }
	    c->buf=buf;
	    c->next=loaded_chunks;
	    c->prev=NULL;
	    if(loaded_chunks)
		loaded_chunks->prev=c;
	    else
		oldest_chunk=c;
	    loaded_chunks=c;
	    if(!parts) {
		sz1=input->read(buf,n*chunk_size,chunk_size); /* error handling... */
		if(sz1==-1) {
		    r-=sz;
		    break;
		}
	    }
	} else if(pos1-offs+chunk_size>total_size)
	    sz1=total_size-pos1+offs;

	const unsigned char* s=c->buf+offs;
	if(sz1<=offs) {
	    r-=sz;
	    break;
	}
	size_t len=sz1-offs;
	size_t sz2=sz<=len?sz:len;

	if(parts) {
	    int offset1=pos1;
	    int size1=sz2;
	    int bstart=n*chunk_size;
	    bool changed=false;
	    while(parts->getFirstMissingPart(&offset1,&size1)) {
		input->read(c->buf+offs+offset1-pos1,offset1,size1);
		offset1+=size1;
		size1=pos1+sz2-offset1;
		changed=true;
	    }
	    if(changed)
		parts->add(pos1,sz2);
	}

	if(c->prev) {
	    c->prev->next=c->next;
	    if(c->next)
		c->next->prev=c->prev;
	    else {
		oldest_chunk=c->prev;
		oldest_chunk->next=NULL;
	    }
	    if(loaded_chunks)
		loaded_chunks->prev=c;
	    c->next=loaded_chunks;
	    c->prev=NULL;
	    loaded_chunks=c;
	}

	sz-=sz2;
	pos1+=sz2;
	++n;
	offs=0;
    }
    //printf("->read=%u\n",r);
#endif
}

size_t myFILE::read(void* p,size_t sz) {
    //printf("read(%p,%p,%u)\n",input?(void*)input:(void*)this,p,sz);
    if(sz>total_size-pos)
	sz=total_size-pos;
    size_t r=sz;
    unsigned n=pos/chunk_size;
    unsigned offs=pos%chunk_size;
    unsigned char* q=static_cast<unsigned char*>(p);
    //bool xx=false;

    while(sz) {
	//printf("sz=%u pos=%u, chunk=%u, offs=%u\n",sz,pos,n,offs);
	chunk* c=chunks+n;
	size_t sz1=chunk_size;
	if(!c->buf) {
	    /*if(!xx) {
		printf("read(%u)\n",sz);
		xx=true;
	    }*/
	    if(r>sz) {
		r-=sz;
		break;
	    }
	    unsigned char* buf=NULL;
	    if(num_loaded_chunks==max_loaded_chunks) {
		if(parts)
		    parts->remove((oldest_chunk-chunks)*chunk_size,chunk_size);
		buf=oldest_chunk->buf;
		oldest_chunk->buf=NULL;
		oldest_chunk=oldest_chunk->prev;
		oldest_chunk->next=NULL;
	    } else {
		++num_loaded_chunks;
		buf=new unsigned char [chunk_size];
	    }
	    c->buf=buf;
	    c->next=loaded_chunks;
	    c->prev=NULL;
	    if(loaded_chunks)
		loaded_chunks->prev=c;
	    else
		oldest_chunk=c;
	    loaded_chunks=c;
	    if(!parts) {
		sz1=input->read(buf,n*chunk_size,chunk_size); /* error handling... */
		if(sz1==-1) {
		    r-=sz;
		    break;
		}
	    }
	} else if(pos-offs+chunk_size>total_size)
	    sz1=total_size-pos+offs;

	const unsigned char* s=c->buf+offs;
	if(sz1<=offs) {
	    r-=sz;
	    break;
	}
	size_t len=sz1-offs;
	size_t sz2=sz<=len?sz:len;
	size_t sz3=sz2;

	if(parts) {
	    int offset1=pos;
	    int size1=sz2;
	    int bstart=n*chunk_size;
	    if(parts->getFirstMissingPart(&offset1,&size1)) {
		if(r+offset1-pos>sz) {
		    sz2=offset1-pos;
		} else {
		    while(parts->getFirstMissingPart(&offset1,&size1)) {
			input->read(c->buf+offs+offset1-pos,offset1,size1);
			offset1+=size1;
			size1=pos+sz2-offset1;
		    }
		    parts->add(pos,sz2);
		}
	    }
	}

	if(c->prev) {
	    c->prev->next=c->next;
	    if(c->next)
		c->next->prev=c->prev;
	    else {
		oldest_chunk=c->prev;
		oldest_chunk->next=NULL;
	    }
	    if(loaded_chunks)
		loaded_chunks->prev=c;
	    c->next=loaded_chunks;
	    c->prev=NULL;
	    loaded_chunks=c;
	}
	if(sz2)
	    memcpy(q,s,sz2);
	q+=sz2;
	sz-=sz2;
	pos+=sz2;
	if(sz2<sz3) {
	    r-=sz;
	    break;
	}
	++n;
	offs=0;
    }
    //printf("->read=%u, pos=%u\n",r,pos);
    return r;
}

int myFILE::seek(long p,int mode) {
    //printf("seek(%p,%u,%d) pos=%u sz=%u\n",input?(void*)input:(void*)this,p,mode,pos,total_size);
    switch(mode) {
      case SEEK_SET:
	pos=p;
	break;
      case SEEK_CUR:
	pos+=p;
	break;
      case SEEK_END:
	pos=total_size+p;
	break;
      default:
	return -1;
    }
    if(pos>total_size) {
	pos=total_size;
	return -1;
    } else
	return 0;
}

int myFILE::getc() {
    if(num_chunks!=1) {
	delete [] chunks;
	chunks=NULL;
	delete parts;
	parts=NULL;
	chunks=new chunk[1];
	chunk_size=total_size;
	num_chunks=1;
	max_loaded_chunks=2;
	num_loaded_chunks=0;
	loaded_chunks=NULL;
	oldest_chunk=NULL;
    }
    if (num_loaded_chunks==0) {
	if (!chunks->buf)
	    chunks->buf=new unsigned char [total_size];
	total_size=input->read(chunks->buf,0,total_size);
	num_loaded_chunks=1;
    }
    if(total_size<=pos)
	return EOF;
    unsigned char *t=chunks->buf+pos;
    return chunks->buf[pos++];
}

void *myFILE::gets(void *p,size_t sz) {
    if(num_chunks!=1) {
	delete [] chunks;
	chunks=NULL;
	delete parts;
	parts=NULL;
	chunks=new chunk[1];
	chunk_size=total_size;
	num_chunks=1;
	max_loaded_chunks=2;
	num_loaded_chunks=0;
	loaded_chunks=NULL;
	oldest_chunk=NULL;
    }
    if (num_loaded_chunks==0) {
	if (!chunks->buf)
	    chunks->buf=new unsigned char [total_size];
	total_size=input->read(chunks->buf,0,total_size);
	num_loaded_chunks=1;
    }
    unsigned char *q=(unsigned char *)p;
    unsigned char *t=chunks->buf+pos;
    --sz;
    if(sz>total_size-pos)
	sz=total_size-pos;
    if(sz==0)
	return NULL;
    do {
	*q=*t++;
    } while(*q++!='\n' && --sz!=0);
    *q='\0';
    pos=t-chunks->buf;
    return p;
}

void myFILE::add_parts() {
    if (!memory_file)
	parts=new PartsVec;
}

void myFILE::remove_parts() {
    if(!parts)
	return;
    for(int k=0;k<num_chunks;++k) {
	chunk& c=chunks[k];
	if(c.buf) {
	    int offset=k*chunk_size;
	    int size=chunk_size;
	    if(parts->getFirstMissingPart(&offset,&size)) {
		--num_loaded_chunks;
		delete c.buf;
		c.buf=NULL;
		if(c.next)
		    c.next->prev=c.prev;
		else {
		    oldest_chunk=c.prev;
		    if(oldest_chunk)
			oldest_chunk->next=NULL;
		}
		if(c.prev)
		    c.prev->next=c.next;
		else {
		    loaded_chunks=c.next;
		    if(loaded_chunks)
			loaded_chunks->prev=NULL;
		}
	    }
	}
    }
    delete parts;
    parts=NULL;
}

myFILE* myfopen(const char* name,const char* mode) {
    if(*mode!='r')
	throw("invalid open mode.");
    myFILE* f=NULL;
    try {
	/* kludge... */
	if(name[0]=='\001' && name[1]==':' && name[2]==':') {
	    unsigned x=0;
	    name+=3;
	    int n = 0;
	    while (*name != ':') {
		x |= *name++ - '0' << n;
		n += 3;
	    }
	    unsigned y=0;
	    ++name;
	    n = 0;
	    while (*name != '\0') {
		y |= *name++ - '0' << n;
		n += 3;
	    }
	    f=new myFILE((void*)x,y);
	} else
	    f=new myFILE(new file_input_t(name));
    }
    catch(...) {
    }
    return f;
}


