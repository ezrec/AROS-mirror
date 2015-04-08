//========================================================================
//
// mystdio.h
//
// Copyright 1999-2001 Emmanuel Lesueur
//
//========================================================================

#ifndef MYSTDIO_H
#define MYSTDIO_H

class PartsVec;

#undef getc
#undef tell
#undef rewind

class input_t {
  public:
    virtual ~input_t() {}
    virtual size_t size() =0;
    virtual void preread(int pos,size_t sz) {}
    virtual size_t read(void* buf,int pos,size_t sz) =0;
};

class myFILE {
  public:
    myFILE(input_t*);
    myFILE(const void*,size_t);
    ~myFILE();
    void preread(int,size_t);
    size_t read(void*,size_t);
    int getc();
    void* gets(void*,size_t);
    int seek(long,int);
    long tell() const { return pos; }
    void rewind() { pos=0; }
    size_t size() const { return total_size; }
    void add_parts();
    void remove_parts();
    static void bufsizes(size_t chunk_size,size_t max_size);
  private:
    class chunk {
      public:
	chunk() : buf(NULL) {}
	~chunk() { delete [] buf; }
	chunk* next;
	chunk* prev;
	unsigned char* buf;
    };
    input_t* input;
    PartsVec* parts;
    size_t pos;
    chunk* chunks;
    unsigned num_chunks;
    size_t chunk_size;
    size_t total_size;
    int max_loaded_chunks;
    int num_loaded_chunks;
    chunk* loaded_chunks;
    chunk* oldest_chunk;
    bool memory_file;
    static size_t glb_chunk_size;
    static size_t max_buf;
};

myFILE* myfopen(const char*,const char*);
inline myFILE* myfopen(const void* p,size_t sz) {
    return new myFILE(p,sz);
}
inline int myfclose(myFILE* f) { delete f; }
inline size_t myfread(void* buf,size_t bsize,size_t n,myFILE* f) {
    return f->read(buf,n*bsize)/bsize;
}
inline int myfgetc(myFILE* f) { return f->getc(); }
inline int myfseek(myFILE* f,long pos,int mode) { return f->seek(pos,mode); }
inline long myftell(myFILE* f) { return f->tell(); }
inline void myrewind(myFILE* f) { return f->rewind(); }
inline void* myfgets(void* buf,size_t size,myFILE* f) {
    return f->gets(buf,size);
}

#endif

