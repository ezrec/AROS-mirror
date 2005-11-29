#include <exec/memory.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <libraries/iffparse.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <utility/tagitem.h>
#include "/fs/packets.h"
#include "/fs/query.h"
#include <stdio.h>

/* Note to people who wish to use this source:
   -------------------------------------------

   This code was created in just a few hours mainly to help debugging
   the Defragmenter visually.  No great effort has been made to make
   it completely stable or OS conform, so beware.

   There's definitely more work to be done :-) */

struct Window *mywindow;
struct RastPort *rastport;
UWORD dx=10;
UWORD dy=10;

UWORD uw;      /* UnitWidth */
UWORD uh;      /* UnitHeight */
ULONG ppu;     /* Pixels per Unit */
UWORD uhor;    /* Units horizontally */
UWORD uver;    /* Units vertically */
ULONG units;   /* Total number of Units */
ULONG bpu;     /* Blocks per Unit */
ULONG dw=900, dh=600;  /* Defrag width & height */

ULONG *bitmap;

static const char version[]={"\0$VER: SFSdefragmentGUI 1.2 " __AMIGADATE__ "\r\n"};

struct DefragmentStep {
  ULONG id;       // id of the step ("MOVE", "DONE" or 0)
  ULONG length;   // length in longwords (can be 0)
  ULONG data[0];  // size of this array is determined by length.
};

ULONG steps[200];

ULONG lastread[200/5];
ULONG lastwritten[200/5];
ULONG lastblocks[200/5];

LONG bmffo(ULONG *bitmap,LONG longs,LONG bitoffset);
LONG bmffz(ULONG *bitmap,LONG longs,LONG bitoffset);
LONG bmset(ULONG *bitmap,LONG longs,LONG bitoffset,LONG bits);
LONG bmclr(ULONG *bitmap,LONG longs,LONG bitoffset,LONG bits);
void render(ULONG block, ULONG blocks, WORD pen);
void recalc(ULONG blocks_total, WORD w, WORD h);
void drawrect(UWORD x, UWORD y, UWORD w, UWORD h, WORD pen);
void initfield(ULONG blocks_total);

void main() {
  struct RDArgs *readarg;
  UBYTE template[]="DEVICE/A,DEBUG/S\n";
  WORD exit=FALSE;

  struct {char *name;
          ULONG debug;} arglist={NULL};

  DOSBase=(struct DosLibrary *)OpenLibrary("dos.library",37);
  IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",37);
  GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",37);

  if((readarg=ReadArgs(template,(LONG *)&arglist,0))!=0) {
    struct MsgPort *msgport;
    struct DosList *dl;
    UBYTE *devname=arglist.name;

    while(*devname!=0) {
      if(*devname==':') {
        *devname=0;
        break;
      }
      devname++;
    }

    dl=LockDosList(LDF_DEVICES|LDF_READ);
    if((dl=FindDosEntry(dl, arglist.name, LDF_DEVICES))!=0) {
      LONG errorcode;

      msgport=dl->dol_Task;
      UnLockDosList(LDF_DEVICES|LDF_READ);

      {
        struct TagItem tags[]={ASQ_TOTAL_BLOCKS, 0,
                               ASQ_VERSION, 0,
                               TAG_END, 0};

        if((errorcode=DoPkt(msgport, ACTION_SFS_QUERY, (LONG)&tags, 0, 0, 0, 0))!=DOSFALSE) {
          ULONG blocks_total=tags[0].ti_Data;

          if(tags[1].ti_Data >= (1<<16) + 83) {

            if((errorcode=DoPkt(msgport, ACTION_SFS_DEFRAGMENT_INIT, 0, 0, 0, 0, 0))!=DOSFALSE) {
              if((bitmap=AllocVec(blocks_total / 8 + 32, MEMF_CLEAR))!=0) {
                if((errorcode=DoPkt(msgport, ACTION_SFS_READ_BITMAP, (LONG)bitmap, 0, blocks_total, 0, 0))!=DOSFALSE) {
                  if((mywindow=OpenWindowTags(0, WA_Width, dw+16,
                                                 WA_Height, dh+16,
                                                 WA_MinWidth, 100,
                                                 WA_MinHeight, 100,
                                                 WA_MaxWidth, 10000,
                                                 WA_MaxHeight, 10000,
                                                 WA_SizeGadget, TRUE,
                                                 WA_DepthGadget, TRUE,
                                                 WA_CloseGadget, TRUE,
                                                 WA_DragBar, TRUE,
                                                 WA_Title, "SFS defragmentation window",
                                                 WA_Activate, TRUE,
                                                 WA_GimmeZeroZero, TRUE,
                                                 WA_IDCMP, IDCMP_MOUSEBUTTONS|IDCMP_NEWSIZE|IDCMP_CLOSEWINDOW,
                                                 TAG_DONE))!=0) {
                    BYTE defragmented=FALSE;

                    rastport=mywindow->RPort;

                    initfield(blocks_total);

                    lastread[0]=0;
                    lastwritten[0]=0;
                    lastblocks[0]=0;

                    do {
                      struct IntuiMessage *msg;
                      UWORD class,code,qualifier;

                      if(defragmented==FALSE) {
                        if((errorcode=DoPkt(msgport, ACTION_SFS_DEFRAGMENT_STEP, (LONG)steps, 190, 0, 0, 0))!=DOSFALSE) {
                          struct DefragmentStep *ds=(struct DefragmentStep *)steps;
                          WORD e;

                          e=0;

                          while(lastblocks[e]!=0) {
                            render(lastread[e], lastblocks[e], 0);
                            render(lastwritten[e], lastblocks[e], 1);
                            e++;
                          }

                          e=0;

                          while(ds->id!=0) {
                            if(ds->id==MAKE_ID('M','O','V','E') && ds->length==3) {
                              render(ds->data[1], ds->data[0], 2);
                              render(ds->data[2], ds->data[0], 3);
                              bmclr(bitmap, (blocks_total+31)/32, ds->data[2], ds->data[0]);
                              bmset(bitmap, (blocks_total+31)/32, ds->data[1], ds->data[0]);

                              if(arglist.debug!=0) {
                                printf("Moved %ld blocks from %ld to %ld\n", ds->data[0], ds->data[1], ds->data[2]);
                              }

                              lastread[e]=ds->data[1];
                              lastwritten[e]=ds->data[2];
                              lastblocks[e]=ds->data[0];
                              e++;
                            }
                            else if(ds->id==MAKE_ID('D','O','N','E')) {
                              defragmented=TRUE;
                              break;
                            }
                            ds=(struct DefragmentStep *)((ULONG *)ds + 2 + ds->length);
                          }

                          lastblocks[e]=0;

                          if(arglist.debug!=0 && e!=0) {
                            printf("\n");
                          }
                        }
                      }
                      else {
                        WaitPort(mywindow->UserPort);
                      }

                      while((msg=(struct IntuiMessage *)GetMsg(mywindow->UserPort))!=0) {

                        class=msg->Class;
                        code=msg->Code;
                        qualifier=msg->Qualifier;

                        ReplyMsg((struct Message *)msg);

                        if(class==IDCMP_CLOSEWINDOW) {
                         // if(code==IECODE_LBUTTON) {
                            exit=TRUE;
                         // }
                        }
                        else if(class==IDCMP_NEWSIZE) {
                          initfield(blocks_total);
                        }
                      }
                    } while(exit==FALSE);


                    CloseWindow(mywindow);
                  }
                }

                FreeVec(bitmap);
              }
            }
          }
          else {
            PutStr("Version of SmartFilesystem must be 1.83 or newer.  You're using an older version.\n");
          }
        }
      }
    }
    else {
      VPrintf("Couldn't find device '%s:'.\n",&arglist.name);
      UnLockDosList(LDF_DEVICES|LDF_READ);
    }

    FreeArgs(readarg);
  }

  CloseLibrary((struct Library *)GfxBase);
  CloseLibrary((struct Library *)IntuitionBase);
  CloseLibrary((struct Library *)DOSBase);
}



void recalc(ULONG blocks_total, WORD w, WORD h) {
  UBYTE pixw[17]={ 1, 2, 3, 2, 3, 4, 3, 5, 4, 5, 4, 6, 5, 6, 5, 6, 6};
  UBYTE pixh[17]={ 1, 1, 1, 2, 2, 2, 3, 2, 3, 3, 4, 3, 4, 4, 5, 5, 6};
  //               1  2  3  4  6  8  9 10 12 15 16 18 20 24 25 30 36

  WORD i;

  for(i=16; i>=0; i--) {
    uw    = pixw[i];
    uh    = pixh[i];
    ppu   = uw * uh;
    uhor  = w/uw;
    uver  = h/uh;
    units = uhor * uver;
    bpu   = (blocks_total+units-1) / units;

    if(bpu<=1) {
      break;
    }
  }

  if(bpu==0) {
    bpu=1;
  }
}



void render(ULONG block, ULONG blocks, WORD pen) {
  ULONG firstunit=block/bpu;
  ULONG lastunit=(block+blocks-1)/bpu;
  ULONG line, offset;
  ULONG endline, endoffset;
  UWORD i;

  SetAPen(rastport, pen);

  line=firstunit / uhor;
  offset=firstunit % uhor;

  endline=lastunit / uhor;
  endoffset=lastunit % uhor;

  for(;;) {
    if(line==endline) {
      i=uh;

      while(i-->0) {
        Move(rastport, dx + offset * uw, dy + line * uh + i);
        Draw(rastport, dx + endoffset * uw + uw-1, dy + line * uh + i);
      }
      break;
    }
    else {
      i=uh;

      while(i-->0) {
        Move(rastport, dx + offset * uw, dy + line * uh + i);
        Draw(rastport, dx + (uhor * uw) - 1, dy + line * uh + i);
      }
    }

    line++;
    offset=0;
  }
}



WORD bfffo(ULONG data,WORD bitoffset) {
  ULONG bitmask=1<<(31-bitoffset);

  /** Finds first set bit in /data/ starting at /bitoffset/.  This function
      considers the MSB to be the first bit. */

  do {
    if((data & bitmask)!=0) {
      return(bitoffset);
    }
    bitoffset++;
    bitmask>>=1;
  } while(bitmask!=0);

  return(-1);
}



WORD bfffz(ULONG data,WORD bitoffset) {
  ULONG bitmask=1<<(31-bitoffset);

  /** Finds first zero bit in /data/ starting at /bitoffset/.  This function
      considers the MSB to be the first bit. */

  do {
    if((data & bitmask)==0) {
      return(bitoffset);
    }
    bitoffset++;
    bitmask>>=1;
  } while(bitmask!=0);

  return(-1);
}



LONG bmffo(ULONG *bitmap,LONG longs,LONG bitoffset) {
  ULONG *scan=bitmap;
  ULONG longoffset;
  WORD bit;

  /* This function finds the first set bit in a region of memory starting
     with /bitoffset/.  The region of memory is /longs/ longs long.  It
     returns the bitoffset of the first set bit it finds. */

  longoffset=bitoffset>>5;
  longs-=longoffset;
  scan+=longoffset;

  bitoffset=bitoffset & 0x1F;

  if(bitoffset!=0) {
    if((bit=bfffo(*scan,bitoffset))>=0) {
      return(bit+((scan-bitmap)<<5));
    }

    scan++;
    longs--;
  }

  while(longs-->0) {
    if(*scan++!=0) {
      return(bfffo(*--scan,0)+((scan-bitmap)<<5));
    }
  }

  return(-1);
}



LONG bmffz(ULONG *bitmap,LONG longs,LONG bitoffset) {
  ULONG *scan=bitmap;
  ULONG longoffset;
  WORD bit;

  /* This function finds the first unset bit in a region of memory starting
     with /bitoffset/.  The region of memory is /longs/ longs long.  It
     returns the bitoffset of the first unset bit it finds. */

  longoffset=bitoffset>>5;
  longs-=longoffset;
  scan+=longoffset;

  bitoffset=bitoffset & 0x1F;

  if(bitoffset!=0) {
    if((bit=bfffz(*scan,bitoffset))>=0) {
      return(bit+((scan-bitmap)<<5));
    }

    scan++;
    longs--;
  }

  while(longs-->0) {
    if(*scan++!=0xFFFFFFFF) {
      return(bfffz(*--scan,0)+((scan-bitmap)<<5));
    }
  }

  return(-1);
}



ULONG bfset(ULONG data,WORD bitoffset,WORD bits) {
  ULONG mask;

  /** Sets /bits/ bits starting from /bitoffset/ in /data/.
      /bits/ must be between 1 and 32. */

  /* we want a mask which sets /bits/ bits starting from bit 31 */

  mask=~((1<<(32-bits))-1);
  mask>>=bitoffset;

  return(data | mask);
}



ULONG bfclr(ULONG data,WORD bitoffset,WORD bits) {
  ULONG mask;

  /* we want a mask which sets /bits/ bits starting from bit 31 */

  mask=~((1<<(32-bits))-1);
  mask>>=bitoffset;

  return(data & ~mask);
}



LONG bmclr(ULONG *bitmap,LONG longs,LONG bitoffset,LONG bits) {
  ULONG *scan=bitmap;
  ULONG longoffset;
  LONG orgbits=bits;

  /* This function clears /bits/ bits in a region of memory starting
     with /bitoffset/.  The region of memory is /longs/ longs long.  If
     the region of memory is too small to clear /bits/ bits then this
     function exits after having cleared all bits till the end of the
     memory region.  In any case it returns the number of bits which
     were actually cleared. */

  longoffset=bitoffset>>5;
  longs-=longoffset;
  scan+=longoffset;

  bitoffset=bitoffset & 0x1F;

  if(bitoffset!=0) {
    if(bits<32) {
      *scan=bfclr(*scan,bitoffset,bits);
    }
    else {
      *scan=bfclr(*scan,bitoffset,32);
    }

    scan++;
    longs--;
    bits-=32-bitoffset;
  }

  while(bits>0 && longs-->0) {
    if(bits>31) {
      *scan++=0;
    }
    else {
      *scan=bfclr(*scan,0,bits);
    }
    bits-=32;
  }

  if(bits<=0) {
    return(orgbits);
  }
  return(orgbits-bits);
}



LONG bmset(ULONG *bitmap,LONG longs,LONG bitoffset,LONG bits) {
  ULONG *scan=bitmap;
  ULONG longoffset;
  LONG orgbits=bits;

  /* This function sets /bits/ bits in a region of memory starting
     with /bitoffset/.  The region of memory is /longs/ longs long.  If
     the region of memory is too small to set /bits/ bits then this
     function exits after having set all bits till the end of the
     memory region.  In any case it returns the number of bits which
     were actually set. */

  longoffset=bitoffset>>5;
  longs-=longoffset;
  scan+=longoffset;

  bitoffset=bitoffset & 0x1F;

  if(bitoffset!=0) {
    if(bits<32) {
      *scan=bfset(*scan,bitoffset,bits);
    }
    else {
      *scan=bfset(*scan,bitoffset,32);
    }

    scan++;
    longs--;
    bits-=32-bitoffset;
  }

  while(bits>0 && longs-->0) {
    if(bits>31) {
      *scan++=0xFFFFFFFF;
    }
    else {
      *scan=bfset(*scan,0,bits);
    }
    bits-=32;
  }

  if(bits<=0) {
    return(orgbits);
  }
  return(orgbits-bits);
}


void drawrect(UWORD x, UWORD y, UWORD w, UWORD h, WORD pen) {
  SetAPen(rastport, pen);

  Move(rastport, x, y);
  Draw(rastport, x, y+h-1);
  Draw(rastport, x+w-1, y+h-1);
  Draw(rastport, x+w-1, y);
  Draw(rastport, x, y);
}


void drawbox(UWORD x, UWORD y, UWORD w, UWORD h, WORD pen) {
  SetAPen(rastport, pen);
  RectFill(rastport, x, y, x+w-1, y+h-1);
}



void initfield(ULONG blocks_total) {
  ULONG start,end;
  ULONG current=0;
  UWORD fieldw, fieldh;

  drawbox(0, 0, mywindow->GZZWidth, mywindow->GZZHeight, 0);

  dw=mywindow->GZZWidth-20;
  dh=mywindow->GZZHeight-20;

  recalc(blocks_total, dw, dh);

  fieldw=uhor*uw;
  fieldh=(((blocks_total+bpu-1) / bpu + uhor-1) / uhor) * uh;

  drawrect(dx-3, dy-3, fieldw+6, fieldh+6, -1);
  drawrect(dx-2, dy-2, fieldw+4, fieldh+4, -1);

  // printf("%ld pixel = %ld blocks\n", uh*uw, bpu);

  while(current<blocks_total) {
    start=bmffz(bitmap, (blocks_total+31)/32, current);
    end=bmffo(bitmap, (blocks_total+31)/32, start);

    if(end==-1) {
      end=blocks_total;
    }

    render(start, end-start, 1);
    current=end;
  }
}
