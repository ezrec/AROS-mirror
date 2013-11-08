
// Rar_Extractor 0.2.1. See unrar_license.txt.

/****************************************************************************
 *  This file is part of PPMd project                                       *
 *  Written and distributed to public domain by Dmitry Shkarin 1997,        *
 *  1999-2000                                                               *
 *  Contents: interface to memory allocation routines                       *
 ****************************************************************************/
#ifndef _SUBALLOC_H_
#define _SUBALLOC_H_

const int N1=4, N2=4, N3=4, N4=(128+3-1*N1-2*N2-3*N3)/4;
const int N_INDEXES=N1+N2+N3+N4;

#if defined(__GNUC__)
#define _PACK_ATTR __attribute__ ((packed))
#else
#define _PACK_ATTR
#endif /* defined(__GNUC__) */

#pragma pack(1)
struct RAR_MEM_BLK 
{
  ushort Stamp, NU;
  RAR_MEM_BLK* next, * prev;
  void insertAt(RAR_MEM_BLK* p) 
  {
    next=(prev=p)->next;
    p->next=next->prev=this;
  }
  void remove() 
  {
    prev->next=next;
    next->prev=prev;
  }
} _PACK_ATTR;
#pragma pack()

struct RAR_NODE
{
  RAR_NODE* next;
};

class SubAllocator
{
  private:
    void InsertNode(void* p,int indx);
    void* RemoveNode(int indx);
    uint U2B(int NU);
    void SplitBlock(void* pv,int OldIndx,int NewIndx);
    uint GetUsedMemory();
    void GlueFreeBlocks();
    void* AllocUnitsRare(int indx);

    long SubAllocatorSize;
    byte Indx2Units[N_INDEXES], Units2Indx[128], GlueCount;
    byte *HeapStart,*LoUnit, *HiUnit;
    struct RAR_NODE FreeList[N_INDEXES];
  public:
    SubAllocator();
    ~SubAllocator() {StopSubAllocator();}
    void Clean();
    bool StartSubAllocator(int SASize);
    void StopSubAllocator();
    void  InitSubAllocator();
    void* AllocContext();
    void* AllocUnits(int NU);
    void* ExpandUnits(void* ptr,int OldNU);
    void* ShrinkUnits(void* ptr,int OldNU,int NewNU);
    void  FreeUnits(void* ptr,int OldNU);
    long GetAllocatedMemory() {return(SubAllocatorSize);};

    byte *pText, *UnitsStart,*HeapEnd,*FakeUnitsStart;
};

#endif /* !defined(_SUBALLOC_H_) */
