/* Soliton cardgames for Amiga computers
 * Copyright (C) 1997-2002 Kai Nickel and other authors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdlib.h>
#include <string.h>

#ifdef __AROS__
#define DT_V44_SUPPORT
#define HAVE_XAD 0
#else
#define HAVE_XAD 1
#endif

#include <clib/alib_protos.h>
#include <datatypes/pictureclass.h>
#include <exec/memory.h>
#include <proto/dos.h>
#include <proto/datatypes.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/graphics.h>
#include <proto/timer.h>
#include <proto/utility.h>
#if HAVE_XAD
#include <proto/xadmaster.h>
#endif

#include "Cardgame.h"
#include "Locales.h"

#include "IMG_Time.c"

struct Device *TimerBase;

struct Cardgame_Data;

static void drawBackground(struct Cardgame_Data *data, struct RastPort *rp, int x, int y, int w,
  int h, int offx, int offy);
static void drawCard(struct Cardgame_Data *data, struct RastPort *rp, int nr, int x, int y, int mode);
static void dragAbort(struct Cardgame_Data *data);
static int mousePileAcceptable(struct Cardgame_Data *data, int mx, int my); /* tolerated drop position or -1 */
static LONG mousePile(struct Cardgame_Data *data, int mx, int my, LONG *cnr); /* get pile# and #of cards or -1 */
static void dragDrop(struct Cardgame_Data *data, int to); /* do the drag */
static void dragInit(struct Cardgame_Data *data, int from, int size); /* start drag, create ghost */
static void dragGoXY(struct Cardgame_Data *data, int x, int y, BOOL fly); /* move ghost (with/without steps inbetween) */

struct Buffer
{
  int             width;
  int             height;
  int             depth;
  struct BitMap * bmp;          
  struct RastPort rp; 
};

#define OUTSIDE   -10000 /* Ghost invisible */
#define minVPart  4      /* maximum vertical and horizontal part of card */
#define minHPart  3      /* which is visible in case of overlapping */

struct Cardgame_Data
{
  LONG                        left;
  LONG                        top;
  LONG                        right;
  LONG                        bottom;
  LONG                        width;
  LONG                        height;
  struct RastPort *           rp;
  Object *                    obj;
  BOOL                        clipping;
  BOOL                        insetupcleanup;

  Object *                    pattern;
  LONG                        pattern_width;
  LONG                        pattern_height;
  struct BitMap *             pattern_bmp;
  STRPTR                      pattern_name;

  Object *                    cardset;
  struct BitMap *             cardset_bmp;
  STRPTR                      cardset_name;
  STRPTR                      tempcardset;
  UBYTE                       tempcardname[30];

  LONG                        cardWidth;
  LONG                        cardWidth2;
  LONG                        cardHeight;
  LONG                        cardHeight2;     /*  overlap size */

  LONG                        rasterpartx;     /* size of grid is this part */
  LONG                        rasterparty;     /* of cardsize */

  struct Buffer *             ghostBuffer;
  struct Buffer *             ghostBuffer2;
  struct Buffer *             ghostBufferOld;
  struct Buffer *             ghostBufferOld2;
  LONG                        ghostX;
  LONG                        ghostY;
  LONG                        ghostW;
  LONG                        ghostH;
  BOOL                        opaque;          /* opaque cards - off if buffer not allocatable */
  BOOL                        rekomode;
  BOOL			      norekoback;

  Object *                    statusWin;
  Object *                    statusText;
  Object *                    statusFile;

  struct Pile *               pile;            /* pile array */
  LONG                        pileSize;        /*  Number of piles */

  ULONG     lastsec, lastmic;         //  Double click flags
  int       lastx, lasty;             //  ...
  int       offx, offy;               //  Mausoffset bei "Griff"
  int       mausx, mausy;             //  um Dragvorgang festzustellen

  BOOL      dragndrop;                //  Drag'n'Drop running?
  struct Pile *     dragpile;                 //  Pile containing the drag
  int       dragfrom;                 //  Source of the drag

  BOOL      dragsimulated,            //  Falls die Karten nach drag nicht kopiert werden sollen
            oldopaque;

  int       animspeed;                

  /* Game-Timer */
  struct MsgPort *            port;
  struct timerequest *        req;
  struct MUI_InputHandlerNode ihnode;
  LONG                        secs;
  struct timeval              lasttick;
  LONG                        timer_blocked;
  /* Drawing */

  enum { MOVEGHOST, UPDATEPILE } update_mode;          //  Art des Drawupdates
  int  update_pile;                                    //  Übergabe eines Stacks an Update
};

static STRPTR stringCopy(STRPTR src)
{
  STRPTR dest;

  if(!src)
    src = "";

  if((dest = (STRPTR)AllocVec(strlen(src) + 1, MEMF_PUBLIC)))
    strcpy(dest, src);

  return dest;
}

/****************************************************************************************
  Buffer
****************************************************************************************/

static struct Buffer *bufferCreate(int width, int height, int depth, struct BitMap *frndbmp)
{
  struct BitMap* theBitMap;
  ULONG flags = 0;
  struct Buffer *b;

  width++;  /* security */
  height++;
  width = ((width + 15) / 16) * 16; /* expand */

  if(frndbmp && (GetBitMapAttr(frndbmp, BMA_FLAGS) & BMF_INTERLEAVED))
    frndbmp = NULL;
  if(frndbmp)
    flags |= BMF_MINPLANES;
  if(!(theBitMap = AllocBitMap(width, height, depth, flags, frndbmp)))
    return NULL;

  if((b = (struct Buffer *) AllocVec(sizeof(struct Buffer), MEMF_PUBLIC)))
  {
    b->width  = width;
    b->height = height;
    b->depth  = depth;
    b->bmp    = theBitMap;

    InitRastPort(&b->rp);
    b->rp.BitMap = theBitMap;
  }
  else
    FreeBitMap(theBitMap);

  return b;
}

static void bufferDispose(struct Buffer *b)
{
  if(b)
  {
    if(b->bmp)
      FreeBitMap(b->bmp);
    FreeVec(b);
  }
}


/****************************************************************************************
  Pile
****************************************************************************************/

struct Pile
{
  struct Cardgame_Data* data;         // wegen Kartenausmassen und rastport

  char  type;         /* T, V */

  int   x, y,         /* top left edge position */
        xgrid, ygrid; /* grid position */

  int   card[60],     /* max. 60 cards on a pile */
        cardSize;     /* number of cards */

  int   old_card[60], /* last drawn cards */
        old_cardSize; /* last size of cards */

  int   maxSize;      /* maximal number of cards (for V mode) */

  int   back;         /* image used for empty pile (0=empty, 1=back) */
};

#define PileEmpty(p)   (!((p)->cardSize))

static struct Pile *PileNew(int num)
{
  struct Pile *p;

  if((p = (struct Pile *) AllocVec(sizeof(struct Pile)*num, MEMF_PUBLIC)))
  {
    int i;
    for(i = 0; i < num; ++i)
    {
      p[i].maxSize = 60;
      p[i].back = 0;
      p[i].cardSize = 0;
    }
  }
  return p;
}

static void PileMoveTo(struct Pile *p, struct Pile *to, LONG num)
{
  LONG a;

  for(a = num; a; --a)
    to->card[to->cardSize++] = p->card[p->cardSize - a];
  p->cardSize -= num;
}

static void PileUpdate(struct Pile *p, struct RastPort* rp, LONG xx, LONG yy)
{
  LONG c, xxstart = xx, yystart = yy;

  switch(p->type)
  {
  /* A,B,C,D are stack spaces 1 to 4 (for REKO backcards) */
  case 'T': case 'A' : case 'B': case 'C': case 'D':
    if(PileEmpty(p))
    {
      if(p->back) c = 102;
      else if(p->type < 'T') c = 200+p->type-'A';
      else c = 1;
    }
    else 
      c = p->card[p->cardSize - 1];
    drawCard(p->data, rp, c, xx, yy, 0);
    break;
  case 'V':
    if(PileEmpty(p))
    {
      c = p->back ? 102 : 1;
      drawCard(p->data, rp, c, xx, yy, 0);
      yy += p->data->cardHeight;
    }
    else
    {
      LONG i;
      for(i = 0; i < p->cardSize - 1; i++)
      {
        if(i >= p->old_cardSize || p->old_card[i] != p->card[i])
        {
          drawCard(p->data, rp, p->card[i], xx, yy, 1);
          p->old_card[i] = p->card[i];
        }
        yy += p->data->cardHeight2;
      }
      if(p->cardSize != p->old_cardSize || p->old_card[i] != p->card[i])
      {
        drawCard(p->data, rp, p->card[i], xx, yy, 0);
        p->old_card[i] = p->card[i];
      }
      yy += p->data->cardHeight;
    }
    if(p->old_cardSize > p->cardSize && p->old_cardSize > 1) /* deletes too much */
    {
      drawBackground(p->data, rp, xx, yy, p->data->cardWidth,
        (p->old_cardSize - p->cardSize) * p->data->cardHeight2,
        p->data->pattern?((p->x + xx-xxstart) % p->data->pattern_width):0,
        p->data->pattern?((p->y + yy-yystart) % p->data->pattern_height):0);
    }
    p->old_cardSize = p->cardSize;
    break;
  case 'H':
    if(PileEmpty(p))
    {
      c = p->back ? 102 : 1;
      drawCard(p->data, rp, c, xx, yy, 0);
      xx += p->data->cardWidth;
    }
    else
    {
      LONG i;

      for(i = 0; i < p->cardSize - 1; i++)
      {
        if(i >= p->old_cardSize || p->old_card[i] != p->card[i])
        {
          drawCard(p->data, rp, p->card[i], xx, yy, 2);
          p->old_card[i] = p->card[i];
        }
        xx += p->data->cardWidth2;
      }
      if(p->cardSize != p->old_cardSize || p->old_card[i] != p->card[i])
      {
        drawCard(p->data, rp, p->card[i], xx, yy, 0);
        p->old_card[i] = p->card[i];
      }
      xx += p->data->cardWidth;
    }
    if(p->old_cardSize > p->cardSize && p->old_cardSize > 1) /* deletes too much */
    {
      drawBackground(p->data, rp, xx, yy,
      (p->old_cardSize - p->cardSize) * p->data->cardWidth2,
      p->data->cardHeight,
      p->data->pattern?((p->x + xx-xxstart) % p->data->pattern_width):0,
      p->data->pattern?((p->y + yy-yystart) % p->data->pattern_height):0);
    }
    p->old_cardSize = p->cardSize;
    break;
  }
}

static void PileDraw(struct Pile *p, struct RastPort *rp, LONG x, LONG y)
{
  p->old_cardSize = -1;
  PileUpdate(p, rp, x, y);
}

static int PileWidth(struct Pile *p, BOOL max, BOOL max2)
{
  if(p->type == 'H')
  {
    LONG s, t;

    if((s = max ? p->maxSize : p->cardSize) < 0)
      s = 0;
    t = max2 ? p->data->cardWidth / minHPart : p->data->cardWidth2;
    return p->data->cardWidth + (s - 1) * t;
  }
  return p->data->cardWidth;
}

static int PileHeight(struct Pile *p, BOOL max, BOOL max2)
{
  if(p->type == 'V')
  {
    LONG s,t;

    if((s = max ? p->maxSize : p->cardSize) < 0)
      s = 0;
    t = max2 ? p->data->cardHeight / minVPart : p->data->cardHeight2;
    return p->data->cardHeight + (s - 1) * t;
  }
  return p->data->cardHeight;
}

static int PileMergeX(struct Pile *p)
{
  if(p->type == 'H')
    return p->x + p->cardSize * p->data->cardWidth2;
  return p->x;
}

static int PileMergeY(struct Pile *p)
{
  if(p->type == 'V')
    return p->y + p->cardSize * p->data->cardHeight2;
  return p->y;
}

static int PileCardNr(struct Pile *p, int mx, int my)
{
  int cnr = -1;

  if(mx >= p->x && mx < p->x + PileWidth(p, FALSE,FALSE) &&
  my >= p->y && my < p->y + PileHeight(p, FALSE,FALSE))
  {
    switch(p->type)
    {
    case 'A': case 'B' : case 'C' : case 'D': case 'T': cnr = 1;
      break;
    case 'V':
      cnr = (my - p->y) / p->data->cardHeight2;
      if(cnr > p->cardSize - 1)
        cnr = p->cardSize - 1;
      cnr = p->cardSize - cnr;
      break;
    case 'H':
      cnr = (mx - p->x) / p->data->cardWidth2;
      if(cnr > p->cardSize - 1)
        cnr = p->cardSize - 1;
      cnr = p->cardSize - cnr;
      break;
    }
  }

  return cnr;
}

static BOOL PileFitsDrag(struct Pile *p, struct Pile *drag)
{
  if(drag->cardSize > 1 && drag->type != p->type)
    return FALSE;
  return TRUE;
}

/****************************************************************************************
  Cardgame_Data
****************************************************************************************/

static void Init(struct Cardgame_Data *data, Object *o)
{
  memset(data, 0, sizeof(struct Cardgame_Data));
  data->obj            = o;
  data->dragpile       = PileNew(1);
  data->dragpile->data = data;

  /* Timer */

  if((data->port = CreateMsgPort()))
  {
    if((data->req = (struct timerequest *) CreateIORequest(data->port, sizeof(struct timerequest))))
    {
      if(!OpenDevice(TIMERNAME, UNIT_VBLANK, (struct IORequest*)data->req, 0))
      {
        data->ihnode.ihn_Object  = data->obj;
        data->ihnode.ihn_Millis  = 1000;
        data->ihnode.ihn_Method  = MUIM_Cardgame_TimerEvent;
        data->ihnode.ihn_Flags   = MUIIHNF_TIMER;
        data->timer_blocked      = 1;
        TimerBase = (struct Device *) data->req->tr_node.io_Device;
        GetSysTime(&data->lasttick);
      }
    }
  }
}

static void Destruct(struct Cardgame_Data *data)
{
  if(data->pile)
    FreeVec(data->pile);
  if(data->pattern_name)
    FreeVec(data->pattern_name);
  if(data->cardset_name)
    FreeVec(data->cardset_name);

  /* Timer */

  if(data->req)
  {
    if(data->req->tr_node.io_Device)
      CloseDevice((struct IORequest *)data->req);
    DeleteIORequest(data->req);
  }
  if(data->port)
    DeleteMsgPort(data->port);
}

/* Layout */

static BOOL getNumber(char* s, int *pos, char terminator, long *n)
{
  /* read number inclusive terminator */
  char num[80]; /* buffer for number */
  int slen, i;
  
  slen = strlen(s);
  for(i = 0; *pos < slen && s[*pos] != terminator; i++)
  {
    num[i] = s[*pos];
    (*pos)++;
  }
  num[i] = 0;
  *n = strtol(num, NULL, 10);
  if(*n < 0 || *n > 1000 || *pos >= slen || s[*pos] != terminator)
    return FALSE;
  (*pos)++;
  return TRUE;
}

static BOOL createPiles(struct Cardgame_Data *data, char* s)
{
  int slen, i;

  if(data->pile)
    FreeVec(data->pile);
  data->pileSize = 0;
  if(!s)
    return FALSE;

  /* determine number of piles */

  slen = strlen(s);
  for(i = 0; i < slen; i++)
  {
    if(s[i] == ';')
      data->pileSize++;
  }

  /* create piles */

  if(data->pileSize && (data->pile = PileNew(data->pileSize)))
  {
    int pos = 0;
    long n;

    for(i = 0; i < data->pileSize; i++)
    {
      /* T / V / H */
      if(pos >= slen)
        return FALSE;
      if(s[pos] == 'T')
        data->pile[i].type = 'T';
      else if(s[pos] == 'A')
        data->pile[i].type = 'A';
      else if(s[pos] == 'B')
        data->pile[i].type = 'B';
      else if(s[pos] == 'C')
        data->pile[i].type = 'C';
      else if(s[pos] == 'D')
        data->pile[i].type = 'D';
      else if(s[pos] == 'V')
        data->pile[i].type = 'V';
      else if(s[pos] == 'H')
        data->pile[i].type = 'H';
      else
        return FALSE;
      pos++;
      
      if(pos >= slen || s[pos] != ',')
        return FALSE;
      pos++;

      /* X */
      if(!getNumber(s, &pos, ',', &n))
        return FALSE;
      data->pile[i].xgrid = n;

      /* Y */
      if(!getNumber(s, &pos, ',', &n))
        return FALSE;
      data->pile[i].ygrid = n;

      /* maxSize */
      if(!getNumber(s, &pos, ';', &n))
        return FALSE;
      data->pile[i].maxSize = n;
      data->pile[i].data = data;
    }
  }

  return TRUE;
}

static void widthHeight(struct Cardgame_Data *data, int *allw, int *allh)
{
  int i, j;

  *allw = 10;
  *allh = 10;
  for(i = 0; i < data->pileSize; i++)
  {
    struct Pile *p = &data->pile[i];

    j = PileWidth(p, TRUE,FALSE) + p->x;
    if(j > *allw)
      *allw = j;
    j = PileHeight(p, TRUE,FALSE) + p->y;
    if(j > *allh)
      *allh = j;
  }
  *allw += 4; /* +4 because of border */
  *allh += 4;
}

/* scale: scale factor in percent, (100 = minimum size) */
static void setPilesPos(struct Cardgame_Data *data, int scalex, int scaley)
{
  int i;

  for(i = 0; i < data->pileSize; i++)
  {
    struct Pile *p = &data->pile[i];

    p->x = (p->xgrid * data->cardWidth  * scalex) / (data->rasterpartx * 100) + 2,
    p->y = (p->ygrid * data->cardHeight * scaley) / (data->rasterparty * 100) + 2;
    /* +2 because of border */
  }
}

/* Drawing */
static void drawBackground(struct Cardgame_Data *data, struct RastPort *rp, int x, int y, int w,
int h, int offx, int offy)
{
  if(data->pattern)
  {
    int donew, doneh;

    if((donew = data->pattern_width - offx) > w)
      donew = w;
    if((doneh = data->pattern_height - offy) > h)
      doneh = h;

    BltBitMapRastPort(data->pattern_bmp, offx, offy, rp, x, y, donew, doneh, 0xC0);

    while(donew < w)
    {
      int nw;
      
      if((nw = w - donew) > data->pattern_width)
        nw = data->pattern_width;
      BltBitMapRastPort(data->pattern_bmp, 0, offy, rp, x + donew, y, nw, doneh, 0xC0);
      donew += nw;
    }

    if(doneh < h)
      drawBackground(data, rp, x, y + doneh, w, h - doneh, offx, 0);
  }
  else
  {
    SetAPen(rp, 0);
    RectFill(rp, x, y, x + w - 1, y + h - 1);
  }
}

static int getRank(int nr, int *suit)
{
  int r = 0;

#if (1)
  *suit = 0;
#else
  if (nr >= 2  && nr <= 14) {r = nr;      *suit = 0;}
  else
#endif
      if (nr >= 15 && nr <= 27) {r = nr - 13; *suit = 1;}
      else if (nr >= 28 && nr <= 40) {r = nr - 26; *suit = 2;}
      else if (nr >= 41 && nr <= 53) {r = nr - 39; *suit = 3;}

  return r;
}

static void drawCard(struct Cardgame_Data *data, struct RastPort *rp, int nr, int x, int y, int mode)
{
  int w = 0, h = 0;

  switch(mode)
  {
  case 0: /* total */
    w = data->cardWidth;
    h = data->cardHeight;
    break;
  case 1: /* top */
    w = data->cardWidth;
    h = data->cardHeight2;
    break;
  case 2: /* left */
    w = data->cardWidth2;
    h = data->cardHeight;
    break;
  }

  if(data->cardset)
  {
    int ix, iy;

    if(nr >= 200 && (!data->rekomode || data->norekoback))
      nr = 1;

    if(nr >= 200)
    {
      ix = (13+nr-200) * data->cardWidth;
      iy = 3*data->cardHeight;
    }
    else if(nr > 100) /* facedown */
    {
      ix = 13 * data->cardWidth;
      iy = 0;
    }
    else if(nr == 1) /* empty */
    {
      ix = 13 * data->cardWidth;
      iy = data->cardHeight;
    }
    else
    {
      int rank, suit;

      rank = getRank(nr, &suit);
      ix = (rank == 14) ? 0 : (rank - 1) * data->cardWidth;
      iy = suit * data->cardHeight;
    }
    BltBitMapRastPort(data->cardset_bmp, ix, iy, rp, x, y, w, h, 0xC0);
  }
  else
  {
    int x2 = x + w - 1, y2 = y + h - 1;

    if(nr >= 200)
      nr = 1;

    if(nr > 100) /* facedown */
      SetAPen(rp, 3);
    else if(nr == 1) /* empty */
      SetAPen(rp, 0);
    else
      SetAPen(rp, 2);
    RectFill(rp, x, y, x2, y2);

    SetAPen(rp, 1);
    switch (mode)
    {
    case 0: /* total */
      Move(rp, x , y);
      Draw(rp, x2, y);
      Draw(rp, x2, y2);
      Draw(rp, x , y2);
      Draw(rp, x , y);
      break;
    case 1: /* top */
      Move(rp, x , y2);
      Draw(rp, x , y);
      Draw(rp, x2, y);
      Draw(rp, x2, y2);
      break;
    case 2: /* left */
      Move(rp, x2 , y);
      Draw(rp, x , y);
      Draw(rp, x , y2);
      Draw(rp, x2 , y2);
      break;
    }
    if(nr >= 2 && nr < 100)
    {
      int rank, suit;
      char name[6];
      struct TextExtent te;

      rank = getRank(nr, &suit);
      name[1] = name[2] = 0;
      switch(rank)
      {
        case  2: name[0] = '2';  break;
        case  3: name[0] = '3';  break;
        case  4: name[0] = '4';  break;
        case  5: name[0] = '5';  break;
        case  6: name[0] = '6';  break;
        case  7: name[0] = '7';  break;
        case  8: name[0] = '8';  break;
        case  9: name[0] = '9';  break;
        case 10: name[0] = '1'; 
                 name[1] = '0';  break;
        case 11: name[0] = 'J';  break;
        case 12: name[0] = 'Q';  break;
        case 13: name[0] = 'K';  break;
        case 14: name[0] = 'A';  break;
      }
      switch (suit)
      {
        case 0: strcat(name, "+");  break;
        case 1: strcat(name, "*" ); break;
        case 2: strcat(name, "/" ); break;
        case 3: strcat(name, "-" ); break;
      }
      if(suit == 1 || suit == 2)
        SetAPen(rp, 3);

      TextExtent(rp, name, strlen(name), &te);

      Move(rp, x+2, y+te.te_Height);
      Text(rp, name, strlen(name));
    }
  }
}

static void drawObject(struct Cardgame_Data *data)
{
  int allw, allh, nr;
  int scalex = 180, scaley = 180;

  data->left = _mleft(data->obj);
  data->top = _mtop(data->obj);
  data->right = _mright(data->obj);
  data->bottom = _mbottom(data->obj);
  data->width = _mwidth(data->obj);
  data->height = _mheight(data->obj);
  data->rp = _rp(data->obj);

  /* calculate sizes */

  setPilesPos(data, 100, 100);

  /* maximum overlap width */

  data->cardWidth2  = data->cardWidth  / minHPart;
  /* overlap width is not important
  do
  {
    data->cardWidth2--;
    widthHeight(data, &allw, &allh);
  }
  while(allw > data->width && data->cardWidth2 > 1);
  */

  data->cardHeight2  = data->cardHeight / minVPart;
  do
  {
    data->cardHeight2--;
    widthHeight(data, &allw, &allh);
  }
  while(allh > data->height && data->cardHeight2 > 1);

  setPilesPos(data, scalex, scaley);
  widthHeight(data, &allw, &allh);
  do
  {
    if(allw > data->width  && scalex >= 105)
      scalex -= 5;
    if(allh > data->height && scaley >= 105)
      scaley -= 5;
    setPilesPos(data, scalex, scaley);
    widthHeight(data, &allw, &allh);
  }
  while((allw > data->width && scalex >= 105) || (allh > data->height && scaley >= 105));

  /* draw stack*/
  drawBackground(data, data->rp, data->left, data->top, data->width, data->height, 0, 0);

  for(nr = 0; nr < data->pileSize; nr++) 
    PileDraw(data->pile+nr, data->rp, data->pile[nr].x + data->left, data->pile[nr].y + data->top);
}

/* Status */

static void openStatus(struct Cardgame_Data *data)
{
  Object* app = (Object *) xget(data->obj, MUIA_ApplicationObject);

  if(!app)
    return;
  setatt(app, MUIA_Application_Sleep, TRUE);

  data->statusWin = WindowObject,
    //MUIA_Window_LeftEdge   , MUIV_Window_LeftEdge_Centered,
    //MUIA_Window_TopEdge    , MUIV_Window_TopEdge_Centered,
    MUIA_Window_Width      , MUIV_Window_Width_Visible(40),
    MUIA_Window_CloseGadget, FALSE,
    MUIA_Window_DepthGadget, FALSE,
    MUIA_Window_SizeGadget , FALSE,
    MUIA_Window_DragBar    , FALSE,
    MUIA_Window_Borderless , TRUE,
    MUIA_Window_Activate   , TRUE,

    WindowContents,
      HGroup, ButtonFrame,
        MUIA_Background, MUII_GroupBack,

        Child, BodychunkObject,
                MUIA_FixWidth             , IMG_TIME_WIDTH,
                MUIA_FixHeight            , IMG_TIME_HEIGHT,
                MUIA_Bitmap_Width         , IMG_TIME_WIDTH,
                MUIA_Bitmap_Height        , IMG_TIME_HEIGHT,
                MUIA_Bodychunk_Depth      , IMG_TIME_DEPTH,
                MUIA_Bodychunk_Body       , IMG_Time_body,
                MUIA_Bodychunk_Compression, IMG_TIME_COMPRESSION,
                MUIA_Bodychunk_Masking    , IMG_TIME_MASKING,
                MUIA_Bitmap_SourceColors  , IMG_Time_colors,
                MUIA_Bitmap_Transparent   , 0,
                MUIA_ShowSelState  , FALSE,
                End,

        Child, VGroup,
          Child,  data->statusText = TextObject,
                    End,
          Child,  data->statusFile = TextObject,
                    End,
          End,
        End,
    End;
  if(data->statusWin)
  {
    DoMethod(app, OM_ADDMEMBER, data->statusWin);
    setatt(data->statusWin, MUIA_Window_Open, TRUE);
  }
}

static void closeStatus(struct Cardgame_Data *data)
{
  Object* app = (Object *) xget(data->obj, MUIA_ApplicationObject);

  if(!app)
    return;

  if(data->statusWin)
  {
    setatt(data->statusWin, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, data->statusWin);
    MUI_DisposeObject(data->statusWin);
    data->statusWin = NULL;
  }
  setatt(app, MUIA_Application_Sleep, FALSE);
}

static void setStatus(struct Cardgame_Data *data, int action, char* text)
{
  setatt(data->statusText, MUIA_Text_Contents, GetStr(action));
  if(data->statusWin)
  {
    int i, j;
    if((i = strlen(text)) > 45)
    {
      char tt[50];

      text += i-(45-3);
      for(i = 0; i < 10 && text[i] != '/' && text[i] != ':'; ++i)
        ;
      if(i < 10)  /* start at directory boundaries, when less than 10 */
        text += i; /* chars snipped */
        
      tt[0] = tt[1] = tt[2] = '.';
      for(j = 0; text[j]; ++j)
        tt[j+3] = text[j];
      tt[j+3] = 0;
      setatt(data->statusFile, MUIA_Text_Contents, tt);
    }
    else
      setatt(data->statusFile, MUIA_Text_Contents, text);
  }
}

/* Mouse-Events */

static void mouseDown(struct Cardgame_Data *data, int mx, int my)
{
  dragAbort(data); /* maybe there is still an older Drag'n'Drop job? */

  data->mausx = mx;
  data->mausy = my;
  data->dragndrop = FALSE;
  MUI_RequestIDCMP(data->obj, IDCMP_MOUSEMOVE);
}

static void mouseUp(struct Cardgame_Data *data, int mx, int my, BOOL dc)
{
  LONG i, size;

  MUI_RejectIDCMP(data->obj, IDCMP_MOUSEMOVE);
  setatt(_win(data->obj), MUIA_Window_NoMenus, FALSE);

  if(data->dragndrop)
  { 
    if((i = mousePileAcceptable(data, mx, my)) == -1)
      DisplayBeep(_screen(data->obj));
    
    size = data->dragpile->cardSize;
    dragDrop(data, i);

    if(i != -1)
      DoMethod(data->obj, MUIM_Cardgame_DragDone, data->dragfrom, i, size);
  }
  else if((i = mousePile(data, mx, my, &size)) != -1)
  {
    DoMethod(data->obj, MUIM_Cardgame_ClickCard, i, size, (LONG)dc);
  }
}

static void mouseMove(struct Cardgame_Data *data, int mx, int my)
{
  if(!data->dragndrop && abs(mx - data->mausx) + abs(my - data->mausy) > 3)
  {
    LONG cnt, i;
    
    i = mousePile(data, data->mausx, data->mausy, &cnt);
    if(i != -1 && cnt)
    {
      if(DoMethod(data->obj, MUIM_Cardgame_DragStart, i, &cnt))
      {
        struct Pile *p = data->pile+i;
        if(!PileEmpty(p))
        {
          dragInit(data, i, cnt);

          data->offx = data->mausx - PileMergeX(p);
          data->offy = data->mausy - PileMergeY(p);

          setatt(_win(data->obj), MUIA_Window_NoMenus, TRUE);
        }
      }
    }
  }

  if(data->dragndrop)
    dragGoXY(data, mx - data->offx, my - data->offy, FALSE);
}

static LONG mousePile(struct Cardgame_Data *data, int mx, int my, LONG *cnr)
{
  int i;
  for(i = 0; i < data->pileSize; i++)
  {
    *cnr = PileCardNr(data->pile+i, mx, my);
    if(*cnr != -1)
      return i;
  }
  return -1;
}

static int mousePileAcceptable(struct Cardgame_Data *data, int mx, int my)
{
  BOOL from = FALSE;
  LONG size, dummy, to;

  to = mousePile(data, mx, my, &dummy);
  size = data->dragpile->cardSize;

  if(to == data->dragfrom)
    from = TRUE;
  else if(to != -1 && DoMethod(data->obj, MUIM_Cardgame_DragAccept,
  data->dragfrom, to, size, data->dragpile->card))
    return to;

  mx -= data->offx;
  my -= data->offy;
  to = mousePile(data, mx, my, &dummy);
  if(to == data->dragfrom)
    from = TRUE;
  else if(to != -1 && DoMethod(data->obj, MUIM_Cardgame_DragAccept,
  data->dragfrom, to, size, data->dragpile->card))
    return to;

  mx += data->cardWidth;
  to = mousePile(data, mx, my, &dummy);
  if(to == data->dragfrom)
    from = TRUE;
  else if(to != -1 && DoMethod(data->obj, MUIM_Cardgame_DragAccept,
  data->dragfrom, to, size, data->dragpile->card))
    return to;

  my += data->cardHeight; /* could be larger, but does not mind here */
  to = mousePile(data, mx, my, &dummy);
  if(to == data->dragfrom)
    from = TRUE;
  else if(to != -1 && DoMethod(data->obj, MUIM_Cardgame_DragAccept,
  data->dragfrom, to, size, data->dragpile->card))
    return to;

  mx -= data->cardWidth;
  to = mousePile(data, mx, my, &dummy);
  if(to == data->dragfrom)
    from = TRUE;
  else if(to != -1 && DoMethod(data->obj, MUIM_Cardgame_DragAccept,
  data->dragfrom, to, size, data->dragpile->card))
    return to;

  if(from)
    return data->dragfrom;

  return -1;
}

/* Drag'n'Drop */
static void dragInit(struct Cardgame_Data *data, int from, int size)
{
  struct Pile *f;

  data->dragndrop = TRUE;

  if(size < 0) /* simulated drag, cards do not leave source stack */
  {
    size = -size;
    data->dragsimulated = TRUE;
    data->oldopaque = data->opaque;
    data->opaque = FALSE;
  }
  else
    data->dragsimulated = FALSE;

  /* initialize drag structure, get cards */
  
  f = data->pile+from;
  data->dragfrom = from;

  data->dragpile->cardSize = 0;
  PileMoveTo(f, data->dragpile, size);

  data->dragpile->x    = PileMergeX(f);
  data->dragpile->y    = PileMergeY(f);
  data->dragpile->type = f->type;

  data->ghostW = PileWidth(data->dragpile, FALSE,FALSE);
  data->ghostH = PileHeight(data->dragpile, FALSE,FALSE);

  /* background into buffer */
  if(data->opaque)
  {
    data->ghostX = data->dragpile->x;
    data->ghostY = data->dragpile->y;
    if(PileWidth(f, TRUE,FALSE) > data->ghostBuffer->width ||
    PileHeight(f, TRUE,FALSE) > data->ghostBuffer->height)
    {
      /* swap buffers */
      struct Buffer* b;
      b = data->ghostBuffer;
      data->ghostBuffer = data->ghostBufferOld;
      data->ghostBufferOld = b;
      b = data->ghostBuffer2;
      data->ghostBuffer2 = data->ghostBufferOld2;
      data->ghostBufferOld2 = b;
    }

    PileUpdate(f, &data->ghostBuffer->rp, 0, 0);

    ClipBlit(&data->ghostBuffer->rp, data->ghostX - f->x, data->ghostY - f->y, 
             &data->ghostBuffer->rp, 0, 0, data->ghostW, data->ghostH, 0xC0);
  }
  else
  {
    data->ghostX = OUTSIDE;
    data->ghostY = OUTSIDE;
  }
}

static void dragGoXY(struct Cardgame_Data *data, int destx, int desty, BOOL fly)
{
  if(fly && data->animspeed)
  {
    int startx, starty, i, s, dx, dy;

    startx = data->dragpile->x;
    starty = data->dragpile->y;
    dx = destx - startx;
    dy = desty - starty;

    for(s = 1; s < 200; s++)
    {
      if(abs(dx/s) + abs(dy/s) <= data->animspeed)
        break;
    }

    for(i = 1; i < s; i++)
    {
      data->dragpile->x = startx + (dx*i/s);
      data->dragpile->y = starty + (dy*i/s);
      data->update_mode = MOVEGHOST;
      MUI_Redraw(data->obj, MADF_DRAWUPDATE);
    }
  }

  data->dragpile->x = destx;
  data->dragpile->y = desty;
  data->update_mode = MOVEGHOST;
  MUI_Redraw(data->obj, MADF_DRAWUPDATE);
}

static void dragDrop(struct Cardgame_Data *data, int to)
{
  struct Pile *topile;

  /* stuff related to abort */
  if(!data->dragndrop)
    return;
  if(to == -1)
    to = data->dragfrom;
  data->dragndrop = FALSE;

  /* bring ghost to destination */
  topile = data->pile+to;
  dragGoXY(data, PileMergeX(topile), PileMergeY(topile), TRUE);

  /* non-opaque: clear border */
  if(!data->opaque || !PileFitsDrag(topile, data->dragpile))
  {
    data->dragpile->x = OUTSIDE;
    data->dragpile->y = OUTSIDE;
    data->update_mode = MOVEGHOST;
    MUI_Redraw(data->obj, MADF_DRAWUPDATE);
  }

  /* do move */
  if(data->dragsimulated)
    PileMoveTo(data->dragpile, data->pile+data->dragfrom, data->dragpile->cardSize);
  else
    PileMoveTo(data->dragpile, topile, data->dragpile->cardSize);

  /* non-opaque: update from pile, as we still see cards there */
  if(!data->opaque && data->dragfrom != to && !data->dragsimulated)
  {
    data->update_mode = UPDATEPILE;
    data->update_pile = data->dragfrom;
    MUI_Redraw(data->obj, MADF_DRAWUPDATE);
  }

  //  To-Pile updaten. Das ist leider nötig, damit 
  //  old_xxx auf die richtigen Werte gesetzt,
  //  wegen nicht-opaque und wegen V auf H-Drags, etc.
  //

  data->update_mode = UPDATEPILE;
  data->update_pile = to;
  MUI_Redraw(data->obj, MADF_DRAWUPDATE);
  if(data->dragsimulated)
  {
    data->opaque = data->oldopaque;
    data->dragsimulated = FALSE;
  }
}

static void dragAbort(struct Cardgame_Data *data)
{
  if(data->insetupcleanup)
  {
    MUI_RejectIDCMP(data->obj, IDCMP_MOUSEMOVE);
    setatt(_win(data->obj), MUIA_Window_NoMenus, FALSE);
    dragDrop(data,-1);
  }
}

static void drawGhostBorder(struct Cardgame_Data *data, int x, int y)
{
  int w, h, i;

  SetDrMd(data->rp, COMPLEMENT);
  x += data->left;
  y += data->top;
  w = data->ghostW;
  h = data->ghostH;
  for(i = 0; i < 3; i++)
  {
    Move(data->rp, x + i    , y + i    );
    Draw(data->rp, x + w - i, y + i    );
    Draw(data->rp, x + w - i, y + h - i);
    Draw(data->rp, x + i    , y + h - i);
    Draw(data->rp, x + i    , y + i    );
  }
  SetDrMd(data->rp, JAM1);
}

static BOOL Overlap(int *x1, int *y1, int *x2, int *y2, int *w, int *h)
/*
**  Prüft, ob sich zwei Rechtecke (gleicher Größe) überlappen.
**  x1, y1 bzw. x2, y2 sind dann die relativen Eckpunkte des
**  Überlappbereiches mit den Ausmassen w und h
*/
{
  int dx = *x2 - *x1,
      dy = *y2 - *y1;

  *w -= abs(dx);
  *h -= abs(dy);
  if(*w <= 0 || *h <= 0)
    return FALSE;

  if(dx >= 0)
  {
    if(dy >= 0)
    {
      *x1 = dx;
      *y1 = dy;
      *x2 = 0;
      *y2 = 0;
    }
    else
    {
      *x1 = dx;
      *y1 = 0;
      *x2 = 0;
      *y2 = abs(dy);
    }
  }
  else
  {
    if(dy >= 0)
    {
      *x1 = 0;
      *y1 = dy;
      *x2 = abs(dx);
      *y2 = 0;
    }
    else
    {
      *x1 = 0;
      *y1 = 0;
      *x2 = abs(dx);
      *y2 = abs(dy);
    }
  }
  return TRUE;
}

static void ghostMove(struct Cardgame_Data *data, int x, int y)
{
  /* correct boundary problems */
  
  if(x != OUTSIDE) 
  {
    int w1 = data->width - 1,
        h1 = data->height - 1;

    if(x < 0)
      x = 0;
    else if(x + data->ghostW > w1)
      x = w1 - data->ghostW;
    if(y < 0)
      y = 0;
    else if (y + data->ghostH > h1)
      y = h1 - data->ghostH;
  }
  if(data->opaque)
  {
    int x1 = data->ghostX, y1 = data->ghostY,
        x2 = x, y2 = y,
        ww = data->ghostW, hh = data->ghostH;
    BOOL lap;
    struct Buffer *a;
  
    /* copy new background into buffer2 */
    ClipBlit(data->rp, data->left + x, data->top + y, &data->ghostBuffer2->rp, 0, 0,
             data->ghostW, data->ghostH, 0xC0);

    /* copy overlap area from buffer to buffer2 */
    lap = Overlap(&x1, &y1, &x2, &y2, &ww, &hh);
    if(lap)
    {
      ClipBlit(&data->ghostBuffer->rp, x1, y1, &data->ghostBuffer2->rp, x2, y2,
               ww, hh, 0xC0);
    }

    /* redraw card */
    ClipBlit(data->rp, data->left + data->ghostX, data->top + data->ghostY,
             data->rp, data->left + x, data->top + y, data->ghostW, data->ghostH, 0xC0);

    /* overwrite remaining parts of card with buffer */
    if(lap)
    {
      int dx = 0, dy = 0;

      if(x2) dx = ww;
      if(y2) dy = hh;
      ClipBlit(&data->ghostBuffer->rp, dx,  0, data->rp, data->left + data->ghostX + dx,
               data->top + data->ghostY, data->ghostW - ww, data->ghostH, 0xC0);
      ClipBlit(&data->ghostBuffer->rp,  0, dy, data->rp, data->left + data->ghostX,
               data->top + data->ghostY + dy, data->ghostW, data->ghostH - hh, 0xC0);
    }
    else
    {
      ClipBlit(&data->ghostBuffer->rp, 0, 0, data->rp, data->left + data->ghostX, data->top + data->ghostY, 
               data->ghostW, data->ghostH, 0xC0);
    }

    /* switch buffer and buffer2 */

    a = data->ghostBuffer;
    data->ghostBuffer = data->ghostBuffer2;
    data->ghostBuffer2 = a;

  }
  else
  {
    if(data->ghostX != OUTSIDE) drawGhostBorder(data, data->ghostX, data->ghostY);
    if(x            != OUTSIDE) drawGhostBorder(data, x, y);
  }

  data->ghostX = x;
  data->ghostY = y;
}


/****************************************************************************************
  Externe Funktionen
****************************************************************************************/

static IPTR _TimerEvent(struct IClass* cl, Object* obj/*, Msg msg*/)
{
  struct Cardgame_Data* data = (struct Cardgame_Data *)INST_DATA(cl, obj);

  if(!data->timer_blocked && !data->dragndrop)
  {
    struct timeval now;
    GetSysTime(&now);

    data->secs += ( now.tv_secs - data->lasttick.tv_secs );

    data->lasttick = now;

    DoMethod(obj, MUIM_Cardgame_TimerTick, data->secs);
  }

  return 0;
}

static IPTR _TimerReset(struct IClass* cl, Object* obj/*, Msg msg*/)
{
  struct Cardgame_Data* data = (struct Cardgame_Data *)INST_DATA(cl, obj);

  data->secs = 0;
  GetSysTime(&data->lasttick);

  DoMethod(obj, MUIM_Cardgame_TimerTick, data->secs);

  return 0;
}

static IPTR _MoveCards(struct IClass* cl, Object* obj, 
                         struct MUIP_Cardgame_MoveCards* msg)
{
  struct Cardgame_Data* data = (struct Cardgame_Data*)INST_DATA(cl, obj);
  dragAbort(data);
  dragInit(data, msg->source, msg->size);
  dragDrop(data, msg->dest);
  return FALSE;
}

static IPTR _SetEmptyImage(struct IClass* cl, Object* obj, struct MUIP_Cardgame_SetEmptyImage* msg)
{
  struct Cardgame_Data* data = (struct Cardgame_Data*)INST_DATA(cl, obj);
  if(msg->pile >= 0 && msg->pile < data->pileSize)
  {
    if(msg->img >= 0 && msg->img <= 1)
    {
      data->pile[msg->pile].back = msg->img;
      if(data->insetupcleanup)
      {
        data->update_mode = UPDATEPILE;
        data->update_pile = msg->pile;
        MUI_Redraw(obj, MADF_DRAWUPDATE);
      }
      return TRUE;
    }
  }
  return FALSE;
}

static IPTR _SetCards(struct IClass* cl, Object* obj, 
                        struct MUIP_Cardgame_SetCards* msg)
{
  int i;
  struct Pile *p;
  struct Cardgame_Data* data = (struct Cardgame_Data*)INST_DATA(cl, obj);

  dragAbort(data);

  p = data->pile + msg->pile;
  p->cardSize = msg->size;

  for(i = 0; i < msg->size; i++)
    p->card[i] = msg->cards[i];

  if(data->insetupcleanup)
  {
    data->update_mode = UPDATEPILE;
    data->update_pile = msg->pile;
    MUI_Redraw(obj, MADF_DRAWUPDATE);
  }

  return FALSE;
}

static IPTR _GetCards(struct IClass* cl, Object* obj, 
                        struct MUIP_Cardgame_GetCards* msg)
{
  int i;
  struct Pile *p;
  struct Cardgame_Data* data = (struct Cardgame_Data*)INST_DATA(cl, obj);

  p = data->pile + msg->pile;
  *(msg->size) = p->cardSize;
  for(i = 0; i < p->cardSize; i++)
    msg->cards[i] = p->card[i];

  return FALSE;
}

static IPTR _SetGraphic(struct IClass* cl, Object* obj, 
                         struct MUIP_Cardgame_SetGraphic* msg)
{
  struct Cardgame_Data* data = (struct Cardgame_Data*)INST_DATA(cl, obj);
  BOOL change = FALSE;

  if(msg->pattern)
  {
    if(!data->pattern_name || strcmp(msg->pattern, data->pattern_name))
    {
      if(data->pattern_name) 
        FreeVec(data->pattern_name);
      data->pattern_name = stringCopy(msg->pattern);
      change = TRUE;
    }
  }
  else
  {
    if(data->pattern_name)
    {
      FreeVec(data->pattern_name);
      data->pattern_name = NULL;
      change = TRUE;
    }
  }

  if(msg->cardset)
  {
    if(!data->cardset_name || strcmp(msg->cardset, data->cardset_name))
    {
      if(data->cardset_name)
        FreeVec(data->cardset_name);
      data->cardset_name = stringCopy(msg->cardset);
      change = TRUE;
    }
  }
  else
  {
    if(data->cardset_name)
    {
      FreeVec(data->cardset_name);
      data->cardset_name = NULL;
      change = TRUE;
    }
  }

  if(change && data->insetupcleanup)
  {
    Object* parent = (Object *) xget(obj, MUIA_Parent);
    if(DoMethod(parent, MUIM_Group_InitChange))
    {
      DoMethod(parent, OM_REMMEMBER, obj);
      DoMethod(parent, OM_ADDMEMBER, obj);
      DoMethod(parent, MUIM_Group_ExitChange);
    }
  }

  return TRUE;
}

static void GetGhostBuffers(struct Cardgame_Data *data)
{
  setPilesPos(data, 100, 100);

  bufferDispose(data->ghostBuffer);
  bufferDispose(data->ghostBuffer2);
  bufferDispose(data->ghostBufferOld);
  bufferDispose(data->ghostBufferOld2);
  data->ghostBuffer = data->ghostBuffer2 =
  data->ghostBufferOld = data->ghostBufferOld2 = NULL;

  if(data->cardset)
  {
    int w = 10, h = 10, i, d;
    struct Screen *scr;
    struct BitMap *frndbmp;

    scr = _screen(data->obj);

    /* Buffer */
    for(i = 0; i < data->pileSize; i++)
    {
      struct Pile *p = data->pile+i;
      int pw, ph;

      pw = PileWidth(p, TRUE, TRUE) + 1;
      ph = PileHeight(p, TRUE, TRUE) + 1;

      if(pw > w) w = pw;
      if(ph > h) h = ph;
    }
    if(h > scr->Height)
      h = scr->Height;

    d = GetBitMapAttr(scr->RastPort.BitMap, BMA_DEPTH);
    frndbmp = scr->RastPort.BitMap;
    data->opaque = TRUE;

    if(w * h > data->cardWidth * h + data->cardHeight * w ) /* w and h buffer */
    {
      data->ghostBuffer     = bufferCreate(w, data->cardHeight, d, frndbmp);
      data->ghostBuffer2    = bufferCreate(w, data->cardHeight, d, frndbmp);
      data->ghostBufferOld  = bufferCreate(data->cardWidth, h, d, frndbmp);
      data->ghostBufferOld2 = bufferCreate(data->cardWidth, h, d, frndbmp);

      if(!data->ghostBuffer    || !data->ghostBuffer2  ||
         !data->ghostBufferOld || !data->ghostBufferOld2)
      {
        data->opaque = FALSE;
        bufferDispose(data->ghostBuffer);
        bufferDispose(data->ghostBuffer2);
        bufferDispose(data->ghostBufferOld);
        bufferDispose(data->ghostBufferOld2);
        data->ghostBuffer = data->ghostBuffer2 =
        data->ghostBufferOld = data->ghostBufferOld2 = NULL;
      }
    }
    else /* wh buffer */
    {
      data->ghostBuffer     = bufferCreate(w, h, d, frndbmp);
      data->ghostBuffer2    = bufferCreate(w, h, d, frndbmp);
      data->ghostBufferOld  = NULL;
      data->ghostBufferOld2 = NULL;

      if(!data->ghostBuffer || !data->ghostBuffer2)
      {
        data->opaque = FALSE;
        bufferDispose(data->ghostBuffer);
        bufferDispose(data->ghostBuffer2);
        data->ghostBuffer = data->ghostBuffer2 = NULL;
      }
    }
  }
  else
    data->opaque = FALSE;
}

static Object *OpenCardset(struct Cardgame_Data *data, STRPTR name,
struct Screen *scr, BOOL unpack)
{
  Object *obj;

  setStatus(data, MSG_LOADING_CARDSET, name);
  if((obj = NewDTObject(name,
  DTA_GroupID           , GID_PICTURE,
  PDTA_FreeSourceBitMap , TRUE,
  OBP_Precision         , PRECISION_IMAGE,
  PDTA_Screen           , scr,
  DTA_SourceType        , DTST_FILE,
  PDTA_Remap            , TRUE,
  PDTA_DestMode         , PMODE_V43,
  PDTA_UseFriendBitMap  , TRUE,
  TAG_DONE)))
    return obj;
    
#if HAVE_XAD
  else if(unpack)
  {
    struct xadMasterBase *xadMasterBase;
    if((xadMasterBase = (struct xadMasterBase *)
    OpenLibrary("xadmaster.library", 9)))
    {
      struct xadArchiveInfo *ai;
      if((ai = xadAllocObjectA(XADOBJ_ARCHIVEINFO, 0)))
      {
        if(!xadGetInfo(ai, XAD_INFILENAME, name, TAG_DONE))
        {
          if(ai->xai_FileInfo)
          {
            STRPTR a;
            struct xadFileInfo *fi, *fi2;
            int i;

            /* get largest file of archive */
            fi = ai->xai_FileInfo;
            for(fi2 = fi->xfi_Next; fi2; fi2 = fi2->xfi_Next)
            {
              if(fi2->xfi_Size > fi->xfi_Size)
                fi = fi2;
            }

            strcpy(data->tempcardname, "T:SolitonTmpCardset");
            a = fi->xfi_FileName;
            for(i = 0; a[i];)
            {
             if(a[i] == '.')
             {
               a = a+i+1;
               i = 0;
             }
             else
               ++i;
            }
            if(i < 7 && a != fi->xfi_FileName)
            {
              data->tempcardname[19] = '.';
              for(i = 20; *a; ++i)
                data->tempcardname[i] = *(a++);
              data->tempcardname[i] = 0;
            }

            setStatus(data, MSG_EXTRACTING_CARDSET, name);
            if(!xadFileUnArc(ai, XAD_OUTFILENAME, data->tempcardname,
            XAD_ENTRYNUMBER, fi->xfi_EntryNumber, TAG_DONE))
            {
              if((obj = OpenCardset(data, data->tempcardname, scr, FALSE)))
                data->tempcardset = data->tempcardname;
              else
                DeleteFile(data->tempcardname);
            }
          }
          xadFreeInfo(ai);
        }
        xadFreeObjectA(ai,0);
      }
      CloseLibrary((struct Library *)xadMasterBase);
    }
  }
  return obj;
#else
  return NULL;
#endif
}

static IPTR _Setup(struct IClass* cl, Object* obj, Msg msg)
{
  struct Cardgame_Data *data;
  struct Screen *scr;

  if(!DoSuperMethodA(cl, obj, msg))
    return FALSE;

  data = (struct Cardgame_Data*)INST_DATA(cl,obj);
  openStatus(data);
  scr = _screen(obj);

  /* Pattern */

  data->pattern = NULL;
  if(data->pattern_name && strcmp(data->pattern_name, ""))
  {
    setStatus(data, MSG_LOADING_PATTERN, data->pattern_name);
    data->pattern = NewDTObject(data->pattern_name,
                      DTA_GroupID           , GID_PICTURE,
                      PDTA_FreeSourceBitMap , TRUE,
                      OBP_Precision         , PRECISION_IMAGE,
                      PDTA_Screen           , scr,
                      DTA_SourceType        , DTST_FILE,
                      PDTA_Remap            , TRUE,
                      PDTA_DestMode         , PMODE_V43,
                      PDTA_UseFriendBitMap  , TRUE,
                      TAG_DONE);
    if(data->pattern)
    {
      struct BitMapHeader* bhd;

      DoMethod(data->pattern, DTM_PROCLAYOUT, NULL, 1);
      GetDTAttrs(data->pattern, PDTA_BitMapHeader , &bhd, TAG_DONE);
      GetDTAttrs(data->pattern, PDTA_DestBitMap   , &data->pattern_bmp, TAG_DONE);
      if(!bhd || !data->pattern_bmp) 
      {
        DisposeDTObject(data->pattern);
        data->pattern = NULL;
      }
      else
      {
        data->pattern_width  = bhd->bmh_Width;
        data->pattern_height = bhd->bmh_Height;
      }
    }
  }


  /* Cardset */
  data->cardset = NULL;
  if(data->cardset_name && strcmp(data->cardset_name, ""))
  {
    data->cardset = OpenCardset(data, data->cardset_name, scr, TRUE);
    if(data->cardset)
    {
      struct BitMapHeader* bhd;

      DoMethod(data->cardset, DTM_PROCLAYOUT, NULL, 1);
      GetDTAttrs(data->cardset, PDTA_BitMapHeader , &bhd, TAG_DONE);
      GetDTAttrs(data->cardset, PDTA_DestBitMap   , &data->cardset_bmp, TAG_DONE);
      if (!bhd || !data->cardset_bmp) 
      {
        DisposeDTObject(data->cardset);
        data->cardset = NULL;
      }
      else
      {
        data->cardHeight = bhd->bmh_Height / 4;
        data->rekomode   = (bhd->bmh_Width == 17*88 && data->cardHeight == 130);
        data->cardWidth  = bhd->bmh_Width / (data->rekomode ? 17 : 14);

        data->cardWidth2  = data->cardWidth  / minHPart;
        data->cardHeight2 = data->cardHeight / minVPart;
      }
    } /* if cardset */
  } /* if cardset_name */

  if(!data->cardset)
  {
    data->cardWidth   = 40;
    data->cardHeight  = 60;

    data->cardWidth2  = data->cardWidth  / minHPart;
    data->cardHeight2 = data->cardHeight / minVPart;
  }
  GetGhostBuffers(data);

  closeStatus(data);

  data->insetupcleanup = TRUE;

  /* Timer */
  data->req->tr_node.io_Command = TR_ADDREQUEST;
  data->req->tr_time.tv_secs    = 1;
  data->req->tr_time.tv_micro   = 0;
  SendIO((struct IORequest *)data->req);
  DoMethod(_app(obj), MUIM_Application_AddInputHandler, &data->ihnode);

  MUI_RequestIDCMP(obj, IDCMP_MOUSEBUTTONS);
  return TRUE;
}

static IPTR _Cleanup(struct IClass* cl, Object* obj, Msg msg)
{
  struct Cardgame_Data *data;
  struct IORequest* req;

  MUI_RejectIDCMP(obj,IDCMP_MOUSEBUTTONS);

  data = (struct Cardgame_Data*)INST_DATA(cl,obj);
  data->insetupcleanup = FALSE;

  if(data->pattern)
    DisposeDTObject(data->pattern);
  if(data->cardset) 
  {
    DisposeDTObject(data->cardset);
    if(data->tempcardset)
    {
      DeleteFile(data->tempcardset);
      data->tempcardset = 0;
    }
    bufferDispose(data->ghostBuffer);
    bufferDispose(data->ghostBuffer2);
    bufferDispose(data->ghostBufferOld);
    bufferDispose(data->ghostBufferOld2);
    data->ghostBuffer = data->ghostBuffer2 =
    data->ghostBufferOld = data->ghostBufferOld2 = NULL;
  }

  /* Timer */

  DoMethod(_app(obj), MUIM_Application_RemInputHandler, &data->ihnode);
  req = (struct IORequest*)data->req;
  if(!CheckIO(req))
    AbortIO(req);
  WaitIO(req);

  return DoSuperMethodA(cl, obj, msg);
}

static IPTR _AskMinMax(struct IClass *cl, Object *obj, struct MUIP_AskMinMax *msg)
{
  struct Cardgame_Data *data;
  struct Screen *scr;
  int allw, allh, minw, minh;

  DoSuperMethodA(cl, obj, (Msg)msg);
  data = (struct Cardgame_Data*)INST_DATA(cl, obj);

  data->cardWidth2  = 1;
  data->cardHeight2 = 1;
  setPilesPos(data, 100, 100);
  widthHeight(data, &allw, &allh);
  minw = allw;
  minh = allh;

  scr = _screen(obj);
  if(scr->Width < allw + 60 || scr->Height < allh + 30) 
  {
    // hier ist mit (nicht sicher, da feste werte!!!) clipping zu rechnen
    minw = 250;
    minh = 150;
  }

  msg->MinMaxInfo->MinWidth  += minw;
  msg->MinMaxInfo->DefWidth  += (allw * 5) / 4;
  msg->MinMaxInfo->MaxWidth  += MUI_MAXMAX;
  msg->MinMaxInfo->MinHeight += minh;
  msg->MinMaxInfo->DefHeight += (allh * 5) / 4;
  msg->MinMaxInfo->MaxHeight += MUI_MAXMAX;

  return 0;
}

static IPTR _New(struct IClass *cl, Object *obj, struct opSet* msg)
{
  struct Cardgame_Data *data;
  SIPTR tmp;

  obj = (Object*)DoSuperNew(cl, obj,
                            MUIA_FillArea, FALSE,
                            MUIA_Font, MUIV_Font_Tiny,
                            TAG_MORE, msg->ops_AttrList);
  if (!obj) return 0;


  data = (struct Cardgame_Data*)INST_DATA(cl, obj);
  Init(data, obj);
  tmp = GetTagData(MUIA_Cardgame_Piles, 0, msg->ops_AttrList);
  if(tmp)
    createPiles(data, (char*)tmp);

  if(GetTagData(MUIA_Cardgame_TimerRunning, 0, msg->ops_AttrList))
    data->timer_blocked = 0;

  tmp = GetTagData(MUIA_Cardgame_RasterX, 0, msg->ops_AttrList);
  if(tmp)
    data->rasterpartx = tmp;
  else
    data->rasterpartx = 7;

  tmp = GetTagData(MUIA_Cardgame_RasterY, 0, msg->ops_AttrList);
  if(tmp)
    data->rasterparty = tmp;
  else
    data->rasterparty = 7;

  data->norekoback = GetTagData(MUIA_Cardgame_NoREKOBack, 0, msg->ops_AttrList);
  data->animspeed = (int)GetTagData(MUIA_Cardgame_MoveSpeed, 0, msg->ops_AttrList);

  return (IPTR)obj;
}

static IPTR _Dispose(struct IClass *cl, Object *obj, Msg msg)
{
  struct Cardgame_Data *data = (struct Cardgame_Data*)INST_DATA(cl, obj);

  Destruct(data);

  return DoSuperMethodA(cl,obj,msg);
}

static IPTR _Draw(struct IClass *cl, Object *obj, struct MUIP_Draw *msg)
{
  APTR cliphandle = 0;
  struct Cardgame_Data *data = (struct Cardgame_Data*)INST_DATA(cl, obj);

  // first draw the superclass
  DoSuperMethodA(cl, obj, (Msg)msg);

  if(msg->flags & MADF_DRAWUPDATE)
  {
    if(data->clipping)
      cliphandle = MUI_AddClipping(muiRenderInfo(obj), _mleft(obj), _mtop(obj), 
                                                       _mwidth(obj), _mheight(obj));

    switch(data->update_mode)
    {
    case MOVEGHOST:
      ghostMove(data, data->dragpile->x, data->dragpile->y);
      break;
    case UPDATEPILE:
      {
        struct Pile *p = data->pile + data->update_pile;
        PileUpdate(p, _rp(obj), p->x + data->left, p->y + data->top);
      }
      break;
    }

    if(data->clipping)
      MUI_RemoveClipping(muiRenderInfo(obj), cliphandle);
  }
  else if(msg->flags & MADF_DRAWOBJECT)
  {
    int allw, allh;

    setPilesPos(data, 100, 100);
    widthHeight(data, &allw, &allh);

    if(_width(obj) < allw || _height(obj) < allh) 
      data->clipping = TRUE;
    else
      data->clipping = FALSE;

    if(data->clipping)
      cliphandle = MUI_AddClipping(muiRenderInfo(obj), _mleft(obj), _mtop(obj), 
                                                       _mwidth(obj), _mheight(obj));

    drawObject(data);
    if(data->clipping)
      MUI_RemoveClipping(muiRenderInfo(obj), cliphandle);
  }

  return 0;
}

static IPTR _HandleInput(struct IClass* cl, Object*obj, struct MUIP_HandleInput* msg)
{
  struct Cardgame_Data* data = (struct Cardgame_Data*)INST_DATA(cl,obj);

  if(msg->imsg)
  {
    int mx = msg->imsg->MouseX - _mleft(obj),
        my = msg->imsg->MouseY - _mtop(obj);

    switch(msg->imsg->Class)
    {
    case IDCMP_MOUSEBUTTONS:
      switch(msg->imsg->Code)
      {
      case SELECTDOWN:
        if(mx >= 0 && mx <= _mwidth(obj) &&
        my >= 0 && my <= _mheight(obj))
          mouseDown(data, mx, my);
        break;
      case SELECTUP:
        {
          BOOL dc = FALSE;
          if(abs(data->lastx - mx) + abs(data->lasty - my) < 4)
          {
            dc = DoubleClick(data->lastsec     , data->lastmic,
                             msg->imsg->Seconds, msg->imsg->Micros);
          }
          mouseUp(data, mx, my, dc);
          data->lastsec = msg->imsg->Seconds;
          data->lastmic = msg->imsg->Micros;
          if(dc)
            data->lastsec = 0; /* do not allow follup click */
          data->lastx   = mx;
          data->lasty   = my;
        }
        break;
      case MENUDOWN:
        dragAbort(data);
        break;
      }
      break;
    case IDCMP_MOUSEMOVE:
      mouseMove(data, mx, my);
      break;
    }
  }

  return DoSuperMethodA(cl, obj, (Msg)msg);
}

static IPTR _Set(struct IClass* cl, Object* obj, struct opSet* msg)
{
  BOOL redraw = FALSE, redrawback = FALSE;
  struct Cardgame_Data* data = (struct Cardgame_Data*)INST_DATA(cl, obj);
  struct TagItem *tag;
  struct TagItem *tags = msg->ops_AttrList;

  while((tag = NextTagItem(&tags)))
  {
    switch(tag->ti_Tag)
    {
    case MUIA_Cardgame_TimerRunning:
      if(tag->ti_Data) /* running */
      {
        if(data->timer_blocked)
        {
          data->timer_blocked--;

          if(!data->timer_blocked)
            GetSysTime(&data->lasttick);
        }
      }
      else
      {
        data->timer_blocked++;
        if(data->timer_blocked == 1)
        {
          struct timeval now;
          GetSysTime(&now);
          data->secs += (now.tv_secs - data->lasttick.tv_secs);
        }
      }
      break;
    case MUIA_Cardgame_MoveSpeed:
      data->animspeed = (int)tag->ti_Data;
      if(data->animspeed < 0 || data->animspeed > 2000)
        data->animspeed = 0;
      break;
    case MUIA_Cardgame_Piles:
      createPiles(data, (char*)tag->ti_Data);
      redraw = TRUE;
      break;
    case MUIA_Cardgame_RasterX:
      data->rasterpartx = tag->ti_Data;
      redraw = TRUE;
      break;
    case MUIA_Cardgame_RasterY:
      data->rasterparty = tag->ti_Data;
      redraw = TRUE;
      break;
    case MUIA_Cardgame_NoREKOBack:
      data->norekoback = tag->ti_Data;
      redrawback = TRUE;
      break;
    }
  }

  if(redraw)
  {
    GetGhostBuffers(data);
    MUI_Redraw(obj, MADF_DRAWOBJECT);
  }
  else if(redrawback)
  {
    int i;
    for(i = 0; i < data->pileSize; ++i)
    {
      if(data->pile[i].type >= 'A' && data->pile[i].type <= 'D')
      {
        data->update_mode = UPDATEPILE;
        data->update_pile = i;
        MUI_Redraw(obj, MADF_DRAWUPDATE);
      }
    }
  }

  return DoSuperMethodA(cl, obj, (Msg)msg);
}

static IPTR _Get(struct IClass* cl, Object* obj, struct opGet* msg)
{
  struct Cardgame_Data* data = (struct Cardgame_Data*)INST_DATA(cl, obj);
  IPTR *store = msg->opg_Storage;

  switch(msg->opg_AttrID)
  {
  case MUIA_Cardgame_TimerRunning:
    if(data->timer_blocked == 0)
      *store = TRUE;
    else
      *store = FALSE;
    return TRUE;
  case MUIA_Cardgame_MoveSpeed:
    *store = data->animspeed;
    return TRUE;
  }

  return DoSuperMethodA(cl, obj, (Msg)msg);
}

/***************************************************************************************
  Init / Exit / Dispatcher
***************************************************************************************/
 
DISPATCHER(Cardgame_Dispatcher)
{
  switch(msg->MethodID)
  {
    case OM_NEW                     : return _New        (cl, obj, (struct opSet*)msg);
    case OM_DISPOSE                 : return _Dispose    (cl, obj, msg);
    case MUIM_AskMinMax             : return _AskMinMax  (cl, obj, (struct MUIP_AskMinMax*)msg);
    case MUIM_Draw                  : return _Draw       (cl, obj, (struct MUIP_Draw*)msg);
    case MUIM_Cleanup               : return _Cleanup    (cl, obj, msg);
    case MUIM_Setup                 : return _Setup      (cl, obj, msg);

    case OM_GET                     : return _Get        (cl, obj, (struct opGet*)msg);
    case OM_SET                     : return _Set        (cl, obj, (struct opSet*)msg);

    case MUIM_HandleInput           : return _HandleInput(cl, obj, (struct MUIP_HandleInput*)msg);

    case MUIM_Cardgame_DragStart    : return FALSE;
    case MUIM_Cardgame_DragAccept   : return FALSE;
    case MUIM_Cardgame_DragDone     : return FALSE;
    case MUIM_Cardgame_ClickCard    : return FALSE;

    case MUIM_Cardgame_SetCards     : return _SetCards     (cl, obj, (struct MUIP_Cardgame_SetCards*)msg);
    case MUIM_Cardgame_GetCards     : return _GetCards     (cl, obj, (struct MUIP_Cardgame_GetCards*)msg);
    case MUIM_Cardgame_SetGraphic   : return _SetGraphic   (cl, obj, (struct MUIP_Cardgame_SetGraphic*)msg);
    case MUIM_Cardgame_MoveCards    : return _MoveCards    (cl, obj, (struct MUIP_Cardgame_MoveCards*)msg);
    case MUIM_Cardgame_SetEmptyImage: return _SetEmptyImage(cl, obj, (struct MUIP_Cardgame_SetEmptyImage*)msg);

    case MUIM_Cardgame_TimerEvent   : return _TimerEvent(cl, obj/*, msg*/);
    case MUIM_Cardgame_TimerReset   : return _TimerReset(cl, obj/*, msg*/);
  }
  return DoSuperMethodA(cl,obj,msg);
}

struct MUI_CustomClass *CL_Cardgame = NULL;

BOOL Cardgame_Init(void)
{
  if(!(CL_Cardgame = MUI_CreateCustomClass(NULL, MUIC_Area, NULL,
  sizeof(struct Cardgame_Data), Cardgame_Dispatcher)))
  {
    ErrorReq(MSG_CREATE_CARDGAMECLASS);
    return FALSE;
  }
  return TRUE;
}

void Cardgame_Exit(void)
{
  if(CL_Cardgame)
    MUI_DeleteCustomClass(CL_Cardgame);
}

