#include <exec/types.h>

#define DEBUG 0
#include <aros/debug.h>

#include "debug.h"
#include "endianess.h"
#include "m68k.h"

extern char * oplength[];
extern struct M68k CPU;

/*
** Calculate the uptodate ccr from the previous operation
*/ 
inline void calc_ccr()
{
  UWORD register flags;

//kprintf("in calc_ccr: lastop: 0x%x\n",CPU.lastop);
  switch (CPU.lastop & (OP_TYPE_MASK|OP_DONTCALCFLAGZ))
  {
    case OP_UPTODATE: return;
    break;
    
    case OP_TYPE1: /* copy X, calc N,Z, set V,C to 0 */
      flags = CPU.SR & CPUFLAG_X;
      switch (CPU.lastop & OP_SIZE_MASK)
      {
        case OP_SIZE_BYTE:
          if (0 == ((BYTE *)&CPU.lastresult)[L_BYTE])
            flags |= CPUFLAG_Z;
          if ((BYTE)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
        
        case OP_SIZE_WORD:
          if (0 == ((WORD *)&CPU.lastresult)[L_WORD])
            flags |= CPUFLAG_Z;
          if ((WORD)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
        
        case OP_SIZE_LONG:
          if (0 == CPU.lastresult)
            flags |= CPUFLAG_Z;
          if ((LONG)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
      }
    break;
    
    case OP_TYPE2: /* calculate all flags (add) */
      flags = CPU.SR & (CPUFLAG_X|CPUFLAG_C);
      switch (CPU.lastop & OP_SIZE_MASK)
      {
        case OP_SIZE_BYTE:
          if ((CPU.oldvalue & 0x80) == (CPU.addvalue   & 0x80) &&
              (CPU.oldvalue & 0x80) != (CPU.lastresult & 0x80))
            flags |= CPUFLAG_V;
          if (0 == ((BYTE *)&CPU.lastresult)[L_BYTE])
            flags |= CPUFLAG_Z;
          if ((BYTE)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
        
        case OP_SIZE_WORD:
          if ((CPU.oldvalue & 0x8000) == (CPU.addvalue   & 0x8000) &&
              (CPU.oldvalue & 0x8000) != (CPU.lastresult & 0x8000))
            flags |= CPUFLAG_V;
          if (0 == ((WORD *)&CPU.lastresult)[L_WORD])
            flags |= CPUFLAG_Z;
          if ((WORD)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
        
        case OP_SIZE_LONG:
          if ((CPU.oldvalue & 0x80000000) == (CPU.addvalue   & 0x80000000) &&
              (CPU.oldvalue & 0x80000000) != (CPU.lastresult & 0x80000000))
            flags |= CPUFLAG_V;
          if (0 == CPU.lastresult)
            flags |= CPUFLAG_Z;
          if ((LONG)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
      }
    break;
    
    case OP_TYPE3: /* calculate all flags */
      flags = 0;
      switch (CPU.lastop & OP_SIZE_MASK)
      {
        case OP_SIZE_BYTE:
          if (0 == (BYTE)CPU.lastresult)
            flags |= CPUFLAG_Z;
          else
          {
            flags |= CPUFLAG_X|CPUFLAG_C|CPUFLAG_N;
            if ((UBYTE)CPU.lastresult <= 0x80)
              flags |= CPUFLAG_V;
          }
          CPU.SR = flags;
          return;
        break;
      
        case OP_SIZE_WORD:
          if (0 == (WORD)CPU.lastresult)
            flags |= CPUFLAG_Z;
          else
          {
            flags |= CPUFLAG_X|CPUFLAG_C|CPUFLAG_N;
            if ((UWORD)CPU.lastresult <= 0x8000)
              flags |= CPUFLAG_V;
          }
          CPU.SR = flags;
          return;
        break;
      
        case OP_SIZE_LONG:
          if (0 == (LONG)CPU.lastresult)
            flags |= CPUFLAG_Z;
          else
          {
            flags |= CPUFLAG_X|CPUFLAG_C|CPUFLAG_N;
            if ((ULONG)CPU.lastresult <= 0x80000000)
              flags |= CPUFLAG_V;
          }
          CPU.SR = flags;
          return;
        break;
      }
    break;

    case OP_TYPE4: /* calculate all flags (sub) */
      flags = 0;
      switch (CPU.lastop & OP_SIZE_MASK)
      {
        case OP_SIZE_BYTE:
          if ((BYTE)CPU.subvalue >= (BYTE)CPU.oldvalue)
          {
            if ((BYTE)CPU.lastresult > 0)
              flags |= CPUFLAG_V;
          }
          else
          {
            /* CPU.subvalue < CPU.oldvalue */
            if ((BYTE)CPU.lastresult < 0)
              flags |= CPUFLAG_V;
          }
          if (0 == ((BYTE *)&CPU.lastresult)[L_BYTE])
            flags |= CPUFLAG_Z;
          if ((BYTE)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
        
        case OP_SIZE_WORD:
          if ((WORD)CPU.subvalue >= (WORD)CPU.oldvalue)
          {
            if ((WORD)CPU.lastresult > 0)
              flags |= CPUFLAG_V;
          }
          else
          {
            /* CPU.subvalue < CPU.oldvalue */
            if ((WORD)CPU.lastresult < 0)
              flags |= CPUFLAG_V;
          }
          if (0 == ((WORD *)&CPU.lastresult)[L_WORD])
            flags |= CPUFLAG_Z;
          if ((WORD)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
        
        case OP_SIZE_LONG:
          if ((LONG)CPU.subvalue >= (LONG)CPU.oldvalue)
          {
            if ((LONG)CPU.lastresult > 0)
              flags |= CPUFLAG_V;
          }
          else
          {
            /* CPU.subvalue < CPU.oldvalue */
            if ((LONG)CPU.lastresult < 0)
              flags |= CPUFLAG_V;
          }
          if (0 == CPU.lastresult)
            flags |= CPUFLAG_Z;
          if ((LONG)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
      }
    break;

    case OP_TYPE5: /* copy X, calculate V,C,Z,N (cmp) */
      flags = CPU.SR & CPUFLAG_X;
      switch (CPU.lastop & OP_SIZE_MASK)
      {
        case OP_SIZE_BYTE:
          if ((BYTE)CPU.subvalue >= (BYTE)CPU.oldvalue)
          {
            if ((BYTE)CPU.lastresult > 0)
              flags |= CPUFLAG_V;
          }
          else
          {
            /* CPU.subvalue < CPU.oldvalue */
            if ((BYTE)CPU.lastresult < 0)
              flags |= CPUFLAG_V;
          }
          if ((UBYTE)CPU.lastresult > (UBYTE)CPU.oldvalue)
            flags |= CPUFLAG_C;
          if (0 == ((BYTE *)&CPU.lastresult)[L_BYTE])
            flags |= CPUFLAG_Z;
          if ((BYTE)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
        
        case OP_SIZE_WORD:
          if ((WORD)CPU.subvalue >= (WORD)CPU.oldvalue)
          {
            if ((WORD)CPU.lastresult > 0)
              flags |= CPUFLAG_V;
          }
          else
          {
            /* CPU.subvalue < CPU.oldvalue */
            if ((WORD)CPU.lastresult < 0)
              flags |= CPUFLAG_V;
          }
          if ((UWORD)CPU.lastresult > (UWORD)CPU.oldvalue)
            flags |= CPUFLAG_C;
          if (0 == ((WORD *)&CPU.lastresult)[L_WORD])
            flags |= CPUFLAG_Z;
          if ((WORD)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
        
        case OP_SIZE_LONG:
          if ((LONG)CPU.subvalue >= (LONG)CPU.oldvalue)
          {
ALIVE
            if ((LONG)CPU.lastresult > 0)
              flags |= CPUFLAG_V;
          }
          else
          {
ALIVE
            /* CPU.subvalue < CPU.oldvalue */
            if ((LONG)CPU.lastresult < 0)
              flags |= CPUFLAG_V;
          }
          if ((ULONG)CPU.lastresult > (ULONG)CPU.oldvalue)
            flags |= CPUFLAG_C;
          if (0 == CPU.lastresult)
            flags |= CPUFLAG_Z;
          if ((LONG)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
      }
    break;


    case OP_UPTODATE|OP_DONTCALCFLAGZ: return;
    break;

    case OP_TYPE1|OP_DONTCALCFLAGZ: /* copy X, calc N,Z, set V,C to 0 */
      flags = CPU.SR & (CPUFLAG_X|CPUFLAG_Z);
      switch (CPU.lastop & OP_SIZE_MASK)
      {
        case OP_SIZE_BYTE:
          if ((BYTE)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
        
        case OP_SIZE_WORD:
          if ((WORD)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
        
        case OP_SIZE_LONG:
          if ((LONG)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
      }
    break;
    
    case OP_TYPE2|OP_DONTCALCFLAGZ: /* copy Z,X,C, calcualte C,V (add) */
      flags = CPU.SR & (CPUFLAG_Z|CPUFLAG_X|CPUFLAG_C);
      switch (CPU.lastop & OP_SIZE_MASK)
      {
        case OP_SIZE_BYTE:
          if ((CPU.oldvalue & 0x80) == (CPU.addvalue   & 0x80) &&
              (CPU.oldvalue & 0x80) == (CPU.lastresult & 0x80))
            flags |= CPUFLAG_V;
          if ((BYTE)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
        
        case OP_SIZE_WORD:
          if ((CPU.oldvalue & 0x8000) == (CPU.addvalue   & 0x8000) &&
              (CPU.oldvalue & 0x8000) == (CPU.lastresult & 0x8000))
            flags |= CPUFLAG_V;
          if ((WORD)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
        
        case OP_SIZE_LONG:
          if ((CPU.oldvalue & 0x80000000) == (CPU.addvalue   & 0x80000000) &&
              (CPU.oldvalue & 0x80000000) == (CPU.lastresult & 0x80000000))
            flags |= CPUFLAG_V;
          if ((LONG)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
      }
    break;
    
    case OP_TYPE3|OP_DONTCALCFLAGZ: /* calculate all flags */
      flags = CPU.SR & CPUFLAG_Z;
      switch (CPU.lastop & OP_SIZE_MASK)
      {
        case OP_SIZE_BYTE:
          if (0 != (BYTE)CPU.lastresult)
          {
            flags |= CPUFLAG_X|CPUFLAG_N;
            if ((UBYTE)CPU.lastresult <= 0x80)
              flags |= CPUFLAG_V;
          }
          CPU.SR = flags;
          return;
        break;
      
        case OP_SIZE_WORD:
          if (0 != (WORD)CPU.lastresult)
          {
            flags |= CPUFLAG_X|CPUFLAG_N;
            if ((UWORD)CPU.lastresult <= 0x8000)
              flags |= CPUFLAG_V;
          }
          CPU.SR = flags;
          return;
        break;
      
        case OP_SIZE_LONG:
          if (0 != (LONG)CPU.lastresult)
          {
            flags |= CPUFLAG_X|CPUFLAG_N;
            if ((ULONG)CPU.lastresult <= 0x80000000)
              flags |= CPUFLAG_V;
          }
          CPU.SR = flags;
          return;
        break;
      }
    break;

    case OP_TYPE4|OP_DONTCALCFLAGZ: /* calculate all flags (sub) */
      flags = CPU.SR & CPUFLAG_Z;
      switch (CPU.lastop & OP_SIZE_MASK)
      {
        case OP_SIZE_BYTE:
          if ((BYTE) (CPU.lastresult ^ CPU.oldvalue) < 0)
            flags |= CPUFLAG_V;
          if ((BYTE)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
        
        case OP_SIZE_WORD:
          if ((WORD) (CPU.lastresult ^ CPU.oldvalue) < 0)
            flags |= CPUFLAG_V;
          if ((WORD)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
        
        case OP_SIZE_LONG:
          if ((LONG) (CPU.lastresult ^ CPU.oldvalue) < 0)
            flags |= CPUFLAG_V;
          if ((LONG)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
      }
    break;
    
    case OP_TYPE5|OP_DONTCALCFLAGZ: /* copy X, calculate V,C,Z,N (cmp) */
      flags = CPU.SR & (CPUFLAG_Z|CPUFLAG_X);
      switch (CPU.lastop & OP_SIZE_MASK)
      {
        case OP_SIZE_BYTE:
          if ((BYTE)CPU.subvalue >= (BYTE)CPU.oldvalue)
          {
            if ((BYTE)CPU.lastresult > 0)
              flags |= CPUFLAG_V;
          }
          else
          {
            if ((BYTE)CPU.lastresult < 0)
              flags |= CPUFLAG_V;
          }
          if ((UBYTE)CPU.lastresult > (UBYTE)CPU.oldvalue)
            flags |= CPUFLAG_C;
          if ((BYTE)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
        
        case OP_SIZE_WORD:
          if ((WORD)CPU.subvalue >= (WORD)CPU.oldvalue)
          {
            if ((WORD)CPU.lastresult > 0)
              flags |= CPUFLAG_V;
          }
          else
          {
            if ((WORD)CPU.lastresult < 0)
              flags |= CPUFLAG_V;
          }
          if ((UWORD)CPU.lastresult > (UWORD)CPU.oldvalue)
            flags |= CPUFLAG_C;
          if ((WORD)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
        
        case OP_SIZE_LONG:
          if ((LONG)CPU.subvalue >= (LONG)CPU.oldvalue)
          {
            if ((LONG)CPU.lastresult > 0)
              flags |= CPUFLAG_V;
          }
          else
          {
            if ((LONG)CPU.lastresult < 0)
              flags |= CPUFLAG_V;
          }
          if ((ULONG)CPU.lastresult > (ULONG)CPU.oldvalue)
            flags |= CPUFLAG_C;
          if ((LONG)CPU.lastresult < 0)
            flags |= CPUFLAG_N;
          CPU.SR = flags;
          return;
        break;
      }
    break;
    
    default:
      kprintf("Unknown case!!\n");
  }
}


BOOL test_cc(UWORD inst,UBYTE size)
{
  switch ((inst >> 8) & 0x0f)
  {
    case 0x00: /* BRA */
SHOW_OPCODE("BRA%s ",oplength[size]);
      return TRUE;
    break;

    case 0x01: /* DBF */
SHOW_OPCODE("BF/BSR%s ",oplength[size]);
      return FALSE;
    break;
    
    case 0x02: /* BHI (not(C) & not(Z) */
SHOW_OPCODE("BHI%s ",oplength[size]);
      /* calculate the condition codes C and Z */
      switch (CPU.lastop & (OP_TYPE_MASK|OP_DONTCALCFLAGZ))
      {
        case OP_UPTODATE:
        case OP_UPTODATE|OP_DONTCALCFLAGZ:
          if (0 == (CPU.SR & (CPUFLAG_C|CPUFLAG_Z)))
            return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
          return FALSE;
        break;
        
        case OP_TYPE1: /* calc Z, C = 0 */
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if (0 != (BYTE)CPU.lastresult)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_WORD:
              if (0 != (WORD)CPU.lastresult)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_LONG:
              if (0 != CPU.lastresult)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;
        
        case OP_TYPE2: /* calc Z and C */
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if (0 != (BYTE)CPU.lastresult &&
                (UBYTE)CPU.lastresult >= (UBYTE)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            case OP_SIZE_WORD:
              if (0 != (WORD)CPU.lastresult &&
                (UWORD)CPU.lastresult >= (UWORD)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            case OP_SIZE_LONG:
              if (0 != (LONG)CPU.lastresult &&
                (ULONG)CPU.lastresult >= (ULONG)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }        
        case OP_TYPE3: /* calc Z and C */
          /* this will never branch */
SHOW_OPCODE("[NOT BRANCHING]\n");
          return FALSE;
        break;
        
        case OP_TYPE4: /* calc Z and C */
        case OP_TYPE5:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if (0 != (BYTE)CPU.lastresult &&
                  (UBYTE)CPU.lastresult <= (UBYTE)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            case OP_SIZE_WORD:
              if (0 != (WORD)CPU.lastresult &&
                  (UWORD)CPU.lastresult <= (UWORD)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            case OP_SIZE_LONG:
              if (0 != (LONG)CPU.lastresult &&
                  (ULONG)CPU.lastresult <= (ULONG)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }        
        break;
        
        case OP_TYPE1|OP_DONTCALCFLAGZ: /* C=0, Z is in SR */
          if (0 == (CPU.SR & CPUFLAG_Z))
            return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
          return FALSE;
        break;
        
        case OP_TYPE2|OP_DONTCALCFLAGZ: /* calc C, Z is in CCR */
          if (0 == (CPU.SR & CPUFLAG_Z))
          {
            switch (CPU.lastop & OP_SIZE_MASK)
            {
              case OP_SIZE_BYTE:
                if ((UBYTE)CPU.lastresult >= (UBYTE)CPU.oldvalue)
                  return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
                return FALSE;
              break;
              case OP_SIZE_WORD:
                if ((UWORD)CPU.lastresult >= (UWORD)CPU.oldvalue)
                  return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
                return FALSE;
              break;
              case OP_SIZE_LONG:
                if ((ULONG)CPU.lastresult >= (ULONG)CPU.oldvalue)
                  return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
                return FALSE;
              break;
            }
          }
        break;

        case OP_TYPE3|OP_DONTCALCFLAGZ: /* calc C, Z is in SR */
          if (0 == (CPU.SR & CPUFLAG_Z))
          {
            switch (CPU.lastop & OP_SIZE_MASK)
            {
              case OP_SIZE_BYTE:
                if (0 != (BYTE)CPU.lastresult)
                  return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
                return FALSE;
              break;
              case OP_SIZE_WORD:
                if (0 != (WORD)CPU.lastresult)
                  return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
                return FALSE;
              break;
              case OP_SIZE_LONG:
                if (0 != (LONG)CPU.lastresult)
                  return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
                return FALSE;
              break;
            }
          }

        case OP_TYPE4|OP_DONTCALCFLAGZ: /* calc C, Z is in SR */
        case OP_TYPE5|OP_DONTCALCFLAGZ:
          if (0 == (CPU.SR & CPUFLAG_Z))
          {
            switch (CPU.lastop & OP_SIZE_MASK)
            {
              case OP_SIZE_BYTE:
                if ((UBYTE)CPU.lastresult <= (UBYTE)CPU.oldvalue)
                  return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
                return FALSE;
              break;
              case OP_SIZE_WORD:
                if ((UWORD)CPU.lastresult <= (UWORD)CPU.oldvalue)
                  return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
                return FALSE;
              break;
              case OP_SIZE_LONG:
                if ((ULONG)CPU.lastresult <= (ULONG)CPU.oldvalue)
                  return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
                return FALSE;
              break;
            }
          }
      }      
    break;
    
    case 0x03: /* BLS  C=1 and Z=1 */
SHOW_OPCODE("BLS%s ",oplength[size]);
      /* calculate the condition codes C and Z */
      switch (CPU.lastop & (OP_TYPE_MASK|OP_DONTCALCFLAGZ))
      {
        case OP_UPTODATE:
        case OP_UPTODATE|OP_DONTCALCFLAGZ:
          if ((CPUFLAG_C|CPUFLAG_Z) == (CPU.SR & (CPUFLAG_C|CPUFLAG_Z)))
            return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
          return FALSE;
        break;
        
        case OP_TYPE1: /* calc Z, C = 0 */
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
        break;
        
        case OP_TYPE2: /* calc Z and C */
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if (0 == (BYTE)CPU.lastresult &&
                (UBYTE)CPU.lastresult < (UBYTE)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            case OP_SIZE_WORD:
              if (0 == (WORD)CPU.lastresult &&
                (UWORD)CPU.lastresult < (UWORD)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            case OP_SIZE_LONG:
              if (0 == (LONG)CPU.lastresult &&
                (ULONG)CPU.lastresult < (ULONG)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }        
        case OP_TYPE3: /* calc Z and C */
          /* this will never branch */
SHOW_OPCODE("[NOT BRANCHING]\n");
          return FALSE;
        break;
        
        case OP_TYPE4: /* calc Z and C */
        case OP_TYPE5:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if (0 == (BYTE)CPU.lastresult &&
                  (UBYTE)CPU.lastresult > (UBYTE)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            case OP_SIZE_WORD:
              if (0 == (WORD)CPU.lastresult &&
                  (UWORD)CPU.lastresult > (UWORD)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            case OP_SIZE_LONG:
              if (0 == (LONG)CPU.lastresult &&
                  (ULONG)CPU.lastresult > (ULONG)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }        
        break;
        
        case OP_TYPE1|OP_DONTCALCFLAGZ: /* C=0, Z is in SR */
SHOW_OPCODE("[NOT BRANCHING]\n");
          return FALSE;
        break;
        
        case OP_TYPE2|OP_DONTCALCFLAGZ: /* calc C, Z is in CCR */
          if (0 != (CPU.SR & CPUFLAG_Z))
          {
            switch (CPU.lastop & OP_SIZE_MASK)
            {
              case OP_SIZE_BYTE:
                if ((UBYTE)CPU.lastresult < (UBYTE)CPU.oldvalue)
                  return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
                return FALSE;
              break;
              case OP_SIZE_WORD:
                if ((UWORD)CPU.lastresult < (UWORD)CPU.oldvalue)
                  return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
                return FALSE;
              break;
              case OP_SIZE_LONG:
                if ((ULONG)CPU.lastresult < (ULONG)CPU.oldvalue)
                  return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
                return FALSE;
              break;
            }
          }
        break;

        case OP_TYPE3|OP_DONTCALCFLAGZ: /* calc C, Z is in SR */
          if (0 != (CPU.SR & CPUFLAG_Z))
          {
            switch (CPU.lastop & OP_SIZE_MASK)
            {
              case OP_SIZE_BYTE:
                if (0 == (BYTE)CPU.lastresult)
                  return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
                return FALSE;
              break;
              case OP_SIZE_WORD:
                if (0 == (WORD)CPU.lastresult)
                  return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
                return FALSE;
              break;
              case OP_SIZE_LONG:
                if (0 == (LONG)CPU.lastresult)
                  return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
                return FALSE;
              break;
            }
          }

        case OP_TYPE4|OP_DONTCALCFLAGZ: /* calc C, Z is in SR */
        case OP_TYPE5|OP_DONTCALCFLAGZ:
          if (0 != (CPU.SR & CPUFLAG_Z))
          {
            switch (CPU.lastop & OP_SIZE_MASK)
            {
              case OP_SIZE_BYTE:
                if ((UBYTE)CPU.lastresult > (UBYTE)CPU.oldvalue)
                  return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
                return FALSE;
              break;
              case OP_SIZE_WORD:
                if ((UWORD)CPU.lastresult > (UWORD)CPU.oldvalue)
                  return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
                return FALSE;
              break;
              case OP_SIZE_LONG:
                if ((ULONG)CPU.lastresult > (ULONG)CPU.oldvalue)
                  return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
                return FALSE;
              break;
            }
          }
      }      
    break;
    
    case 0x04: /* BCC  C=0 */
SHOW_OPCODE("BCC%s ",oplength[size]);
      switch (CPU.lastop & OP_TYPE_MASK|OP_DONTCALCFLAGZ)
      {
        case OP_UPTODATE:
        case OP_UPTODATE|OP_DONTCALCFLAGZ:
          if (0 == (CPU.SR & CPUFLAG_C))
            return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
          return FALSE;
        break;
        
        case OP_TYPE1|OP_DONTCALCFLAGZ:
        case OP_TYPE1: /* C = 0 */
          return TRUE;
        break;
        
        case OP_TYPE2|OP_DONTCALCFLAGZ:
        case OP_TYPE2:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if ((UBYTE)CPU.lastresult >= (UBYTE)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_WORD:
              if ((UWORD)CPU.lastresult >= (UWORD)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            
            case OP_SIZE_LONG:
              if ((ULONG)CPU.lastresult >= (ULONG)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;
        
        case OP_TYPE3|OP_DONTCALCFLAGZ:
        case OP_TYPE3: /* calc all flags */
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if (0 != (UBYTE)CPU.lastresult)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_WORD:
              if (0 != (UWORD)CPU.lastresult)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            
            case OP_SIZE_LONG:
              if (0 != (ULONG)CPU.lastresult)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;

        case OP_TYPE4|OP_DONTCALCFLAGZ:
        case OP_TYPE4:
        case OP_TYPE5:
        case OP_TYPE5|OP_DONTCALCFLAGZ:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if ((UBYTE)CPU.lastresult <= (UBYTE)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_WORD:
              if ((UWORD)CPU.lastresult <= (UWORD)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            
            case OP_SIZE_LONG:
              if ((ULONG)CPU.lastresult <= (ULONG)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;

      }
    break;
    
    case 0x05: /* BCS   C=1 */
SHOW_OPCODE("BCS%s ",oplength[size]);
      switch (CPU.lastop & OP_TYPE_MASK|OP_DONTCALCFLAGZ)
      {
        case OP_UPTODATE:
        case OP_UPTODATE|OP_DONTCALCFLAGZ:
          if (0 != (CPU.SR & CPUFLAG_C))
            return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
          return FALSE;
        break;
        
        case OP_TYPE1|OP_DONTCALCFLAGZ:
        case OP_TYPE1: /* C = 0 */
SHOW_OPCODE("[NOT BRANCHING]\n");
        break;
        
        case OP_TYPE2|OP_DONTCALCFLAGZ:
        case OP_TYPE2:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if ((UBYTE)CPU.lastresult < (UBYTE)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_WORD:
              if ((UWORD)CPU.lastresult < (UWORD)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            
            case OP_SIZE_LONG:
              if ((ULONG)CPU.lastresult < (ULONG)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;
        
        case OP_TYPE3|OP_DONTCALCFLAGZ:
        case OP_TYPE3: /* calc all flags */
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if (0 == (UBYTE)CPU.lastresult)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_WORD:
              if (0 == (UWORD)CPU.lastresult)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            
            case OP_SIZE_LONG:
              if (0 == (ULONG)CPU.lastresult)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;

        case OP_TYPE4|OP_DONTCALCFLAGZ:
        case OP_TYPE4:
        case OP_TYPE5:
        case OP_TYPE5|OP_DONTCALCFLAGZ:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if ((UBYTE)CPU.lastresult > (UBYTE)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_WORD:
              if ((UWORD)CPU.lastresult > (UWORD)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            
            case OP_SIZE_LONG:
              if ((ULONG)CPU.lastresult > (ULONG)CPU.oldvalue)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;

      }
    break;
    
    case 0x06: /* BNE  Z=0 */
SHOW_OPCODE("BNE%s ",oplength[size]);
      switch (CPU.lastop & OP_TYPE_MASK|OP_DONTCALCFLAGZ)
      {
        case OP_UPTODATE:
        case OP_UPTODATE|OP_DONTCALCFLAGZ:
        case OP_TYPE1|OP_DONTCALCFLAGZ:
        case OP_TYPE2|OP_DONTCALCFLAGZ:
        case OP_TYPE3|OP_DONTCALCFLAGZ:
        case OP_TYPE4|OP_DONTCALCFLAGZ:
        case OP_TYPE5|OP_DONTCALCFLAGZ:
          if (0 == (CPU.SR & CPUFLAG_Z))
            return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
          return FALSE;
        break;

        case OP_TYPE1: /* calc Z */
        case OP_TYPE2:
        case OP_TYPE3:
        case OP_TYPE4:
        case OP_TYPE5:
          switch(CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if (0 != (UBYTE)CPU.lastresult)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            case OP_SIZE_WORD:
              if (0 != (UWORD)CPU.lastresult)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            case OP_SIZE_LONG:
              if (0 != (ULONG)CPU.lastresult)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }          
        break;
      }
    break;
    
    case 0x07: /* BEQ  Z=1*/
SHOW_OPCODE("BEQ%s ",oplength[size]);
      switch (CPU.lastop & OP_TYPE_MASK|OP_DONTCALCFLAGZ)
      {
        case OP_UPTODATE:
        case OP_UPTODATE|OP_DONTCALCFLAGZ:
        case OP_TYPE1|OP_DONTCALCFLAGZ:
        case OP_TYPE2|OP_DONTCALCFLAGZ:
        case OP_TYPE3|OP_DONTCALCFLAGZ:
        case OP_TYPE4|OP_DONTCALCFLAGZ:
        case OP_TYPE5|OP_DONTCALCFLAGZ:
          if (0 != (CPU.SR & CPUFLAG_Z))
            return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
          return FALSE;
        break;

        case OP_TYPE1: /* calc Z */
        case OP_TYPE2:
        case OP_TYPE3:
        case OP_TYPE4:
        case OP_TYPE5:
          switch(CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if (0 == (UBYTE)CPU.lastresult)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            case OP_SIZE_WORD:
              if (0 == (UWORD)CPU.lastresult)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            case OP_SIZE_LONG:
              if (0 == (ULONG)CPU.lastresult)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }          
        break;
      }
    break;
    
    case 0x08: /* BVC  V=0 */
SHOW_OPCODE("BVC%s ",oplength[size]);
      switch (CPU.lastop & OP_TYPE_MASK|OP_DONTCALCFLAGZ)
      {
        case OP_UPTODATE:
        case OP_UPTODATE|OP_DONTCALCFLAGZ:
          if (0 == (CPU.SR & CPUFLAG_V))
            return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
          break;
        break;

        case OP_TYPE1:
        case OP_TYPE1|OP_DONTCALCFLAGZ:
          return TRUE;
        break;        
      
        case OP_TYPE2:
        case OP_TYPE2|OP_DONTCALCFLAGZ:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if (!((CPU.oldvalue & 0x80) == (CPU.addvalue   & 0x80) &&
                    (CPU.oldvalue & 0x80) != (CPU.lastresult & 0x80)    ))
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_WORD:
              if (!((CPU.oldvalue & 0x8000) == (CPU.addvalue   & 0x8000) &&
                    (CPU.oldvalue & 0x8000) != (CPU.lastresult & 0x8000)    ))
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            
            case OP_SIZE_LONG:
              if (!((CPU.oldvalue & 0x80000000) == (CPU.addvalue   & 0x80000000) &&
                    (CPU.oldvalue & 0x80000000) != (CPU.lastresult & 0x80000000)    ))
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;
        
        case OP_TYPE3:
        case OP_TYPE3|OP_DONTCALCFLAGZ:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if ((UBYTE)CPU.lastresult > (UBYTE)0x80)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_WORD:
              if ((UWORD)CPU.lastresult > (UWORD)0x8000)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            
            case OP_SIZE_LONG:
              if ((ULONG)CPU.lastresult > (ULONG)0x80000000)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;

        case OP_TYPE4:
        case OP_TYPE4|OP_DONTCALCFLAGZ:
        case OP_TYPE5:
        case OP_TYPE5|OP_DONTCALCFLAGZ:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if ((BYTE)CPU.subvalue >= (BYTE)CPU.oldvalue)
              {
               if ((BYTE)CPU.lastresult <= 0)
                return TRUE;
              }
              else
              {
                if ((BYTE)CPU.lastresult >= 0)
                  return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_WORD:
              if ((WORD)CPU.subvalue >= (WORD)CPU.oldvalue)
              {
               if ((WORD)CPU.lastresult <= 0)
                return TRUE;
              }
              else
              {
                if ((WORD)CPU.lastresult >= 0)
                  return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            
            case OP_SIZE_LONG:
              if ((LONG)CPU.subvalue >= (LONG)CPU.oldvalue)
              {
               if ((LONG)CPU.lastresult <= 0)
                return TRUE;
              }
              else
              {
                if ((LONG)CPU.lastresult >= 0)
                  return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;
      }
    break;
    
    case 0x09: /* BVS  V=1 */
SHOW_OPCODE("BVS%s ",oplength[size]);
      switch (CPU.lastop & OP_TYPE_MASK|OP_DONTCALCFLAGZ)
      {
        case OP_UPTODATE:
        case OP_UPTODATE|OP_DONTCALCFLAGZ:
          if (0 != (CPU.SR & CPUFLAG_V))
            return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
          break;
        break;

        case OP_TYPE1:
        case OP_TYPE1|OP_DONTCALCFLAGZ:
SHOW_OPCODE("[NOT BRANCHING]\n");
          break;
        break;        
      
        case OP_TYPE2:
        case OP_TYPE2|OP_DONTCALCFLAGZ:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if ((CPU.oldvalue & 0x80) == (CPU.addvalue   & 0x80) &&
                  (CPU.oldvalue & 0x80) != (CPU.lastresult & 0x80))
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_WORD:
              if ((CPU.oldvalue & 0x8000) == (CPU.addvalue   & 0x8000) &&
                  (CPU.oldvalue & 0x8000) != (CPU.lastresult & 0x8000))
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            
            case OP_SIZE_LONG:
              if ((CPU.oldvalue & 0x80000000) == (CPU.addvalue   & 0x80000000) &&
                  (CPU.oldvalue & 0x80000000) != (CPU.lastresult & 0x80000000))
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;
        
        case OP_TYPE3:
        case OP_TYPE3|OP_DONTCALCFLAGZ:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if ((UBYTE)CPU.lastresult <= (UBYTE)0x80)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_WORD:
              if ((UWORD)CPU.lastresult <= (UWORD)0x8000)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            
            case OP_SIZE_LONG:
              if ((ULONG)CPU.lastresult <= (ULONG)0x80000000)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;

        case OP_TYPE4:
        case OP_TYPE4|OP_DONTCALCFLAGZ:
        case OP_TYPE5|OP_DONTCALCFLAGZ:
        case OP_TYPE5:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if ((BYTE)CPU.subvalue >= (BYTE)CPU.oldvalue)
              {
               if ((BYTE)CPU.lastresult > 0)
                return TRUE;
              }
              else
              {
                if ((BYTE)CPU.lastresult < 0)
                  return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_WORD:
              if ((WORD)CPU.subvalue >= (WORD)CPU.oldvalue)
              {
               if ((WORD)CPU.lastresult > 0)
                return TRUE;
              }
              else
              {
                if ((WORD)CPU.lastresult < 0)
                  return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            
            case OP_SIZE_LONG:
              if ((LONG)CPU.subvalue >= (LONG)CPU.oldvalue)
              {
               if ((LONG)CPU.lastresult > 0)
                return TRUE;
              }
              else
              {
                if ((LONG)CPU.lastresult < 0)
                  return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;
      }
    break;
    
    case 0x0a: /* BPL  N=0 */
SHOW_OPCODE("BPL%s ",oplength[size]);
      switch (CPU.lastop & (OP_TYPE_MASK|OP_DONTCALCFLAGZ))
      {
        case OP_UPTODATE:
        case OP_UPTODATE|OP_DONTCALCFLAGZ:
          if (0 == (CPU.SR & CPUFLAG_N))
            return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
          return FALSE;
        break;
        
        case OP_TYPE1:
        case OP_TYPE1|OP_DONTCALCFLAGZ:
        case OP_TYPE2:
        case OP_TYPE2|OP_DONTCALCFLAGZ:
        case OP_TYPE4:
        case OP_TYPE4|OP_DONTCALCFLAGZ:
        case OP_TYPE5:
        case OP_TYPE5|OP_DONTCALCFLAGZ:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if ((BYTE)CPU.lastresult >= 0)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            case OP_SIZE_WORD:
              if ((WORD)CPU.lastresult >= 0)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            case OP_SIZE_LONG:
              if ((LONG)CPU.lastresult >= 0)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;
        
        case OP_TYPE3:
        case OP_TYPE3|OP_DONTCALCFLAGZ:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if (0 == (BYTE)CPU.lastresult)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            
            case OP_SIZE_WORD: 
              if (0 == (WORD)CPU.lastresult)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            
            case OP_SIZE_LONG:
              if (0 == (LONG)CPU.lastresult)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;
      
      }
    break;
    
    case 0x0b: /* BMI  N=1 */
SHOW_OPCODE("BMI%s ",oplength[size]);
      switch (CPU.lastop & (OP_TYPE_MASK|OP_DONTCALCFLAGZ))
      {
        case OP_UPTODATE:
        case OP_UPTODATE|OP_DONTCALCFLAGZ:
          if (0 != (CPU.SR & CPUFLAG_N))
            return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
          return FALSE;
        break;
        
        case OP_TYPE1:
        case OP_TYPE1|OP_DONTCALCFLAGZ:
        case OP_TYPE2:
        case OP_TYPE2|OP_DONTCALCFLAGZ:
        case OP_TYPE4:
        case OP_TYPE4|OP_DONTCALCFLAGZ:
        case OP_TYPE5:
        case OP_TYPE5|OP_DONTCALCFLAGZ:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if ((BYTE)CPU.lastresult < 0)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            case OP_SIZE_WORD:
              if ((WORD)CPU.lastresult < 0)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            case OP_SIZE_LONG:
              if ((LONG)CPU.lastresult < 0)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;
        
        case OP_TYPE3:
        case OP_TYPE3|OP_DONTCALCFLAGZ:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if (0 != (BYTE)CPU.lastresult)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            
            case OP_SIZE_WORD: 
              if (0 != (WORD)CPU.lastresult)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            
            case OP_SIZE_LONG:
              if (0 != (LONG)CPU.lastresult)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;
      }
    break;
    
    case 0x0c: /* BGE (N=1 & V=1) || (N=0 & V=0) */
SHOW_OPCODE("BGE%s ",oplength[size]);
      switch (CPU.lastop & (OP_TYPE_MASK|OP_DONTCALCFLAGZ))
      {
        case OP_UPTODATE:
        case OP_UPTODATE|OP_DONTCALCFLAGZ:
          if (0 == (CPU.SR & (CPUFLAG_N|CPUFLAG_V)) ||
              (CPUFLAG_N|CPUFLAG_V) == (CPU.SR & (CPUFLAG_N|CPUFLAG_V)) )
            return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
          return FALSE;
        break;
      
        case OP_TYPE1: /* V=0, calc N */
        case OP_TYPE1|OP_DONTCALCFLAGZ:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if ((BYTE)CPU.lastresult >= 0)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            
            case OP_SIZE_WORD:
              if ((WORD)CPU.lastresult >= 0)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_LONG:
              if ((LONG)CPU.lastresult >= 0)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;
      
        case OP_TYPE2:
        case OP_TYPE2|OP_DONTCALCFLAGZ:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if ((BYTE)CPU.lastresult < 0)
              {
                /* N = 1, calc V */
                if ((BYTE)CPU.oldvalue >= 0 && (BYTE)CPU.addvalue >= 0)
                  return TRUE;
              }
              else
              {
                /* N = 0, calc V */
                /* lastresult >= 0 */
                if ((BYTE)CPU.oldvalue >= 0 || (BYTE)CPU.addvalue >= 0)
                  return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_WORD:
              if ((WORD)CPU.lastresult < 0)
              {
                /* N = 1, calc V */
                if ((WORD)CPU.oldvalue >= 0 && (WORD)CPU.addvalue >= 0)
                  return TRUE;
              }
              else
              {
                /* N = 0, calc V */
                /* lastresult >= 0 */
                if ((WORD)CPU.oldvalue >= 0 || (WORD)CPU.addvalue >= 0)
                  return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_LONG:
              if ((LONG)CPU.lastresult < 0)
              {
                /* N = 1, calc V */
                if ((LONG)CPU.oldvalue >= 0 && (LONG)CPU.addvalue >= 0)
                  return TRUE;
              }
              else
              {
                /* N = 0, calc V */
                /* lastresult >= 0 */
                if ((LONG)CPU.oldvalue >= 0 || (LONG)CPU.addvalue >= 0)
                  return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;

        case OP_TYPE3:
        case OP_TYPE3|OP_DONTCALCFLAGZ:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if ((UBYTE)CPU.lastresult <= 0x80)
              {
                /* V=1 */
                if (0 != (BYTE)CPU.lastresult)
                  return TRUE;
              }
              else
              {
                /* V=0 */
                if (0 == (BYTE)CPU.lastresult)
                  return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_WORD:
              if ((UWORD)CPU.lastresult <= (UWORD)0x8000)
              {
                /* V=1 */
                if (0 != (WORD)CPU.lastresult)
                  return TRUE;
              }
              else
              {
                /* V=0 */
                if (0 == (WORD)CPU.lastresult)
                  return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_LONG:
              if ((ULONG)CPU.lastresult <= (ULONG)0x80000000)
              {
                /* V=1 */
                if (0 != (LONG)CPU.lastresult)
                  return TRUE;
              }
              else
              {
                /* V=0 */
                if (0 == (LONG)CPU.lastresult)
                  return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;        

        case OP_TYPE4:
        case OP_TYPE4|OP_DONTCALCFLAGZ:
        case OP_TYPE5|OP_DONTCALCFLAGZ:
        case OP_TYPE5:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if ((BYTE)CPU.subvalue <= (BYTE)CPU.oldvalue)
              {
                /* V=N */
                return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_WORD:
              if ((WORD)CPU.subvalue <= (WORD)CPU.oldvalue)
              {
                /* V=N */
                return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            
            case OP_SIZE_LONG:
kprintf("s: %d, o: %d, l: %d\n",CPU.subvalue,CPU.oldvalue,CPU.lastresult);
              if ((LONG)CPU.subvalue <= (LONG)CPU.oldvalue)
              {
                /* V=N */
                return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

          }
        break;
      }
    break;
    
    case 0x0d: /* BLT (N=1 & V=0) || (N=0 & V=1) */
SHOW_OPCODE("BLT%s ",oplength[size]);
      switch (CPU.lastop & (OP_TYPE_MASK|OP_DONTCALCFLAGZ))
      {
        case OP_UPTODATE:
        case OP_UPTODATE|OP_DONTCALCFLAGZ:
          if (CPUFLAG_N == (CPU.SR & (CPUFLAG_N|CPUFLAG_V)) ||
              CPUFLAG_V == (CPU.SR & (CPUFLAG_N|CPUFLAG_V)) )
            return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
          return FALSE;
        break;
      
        case OP_TYPE1: /* V=0, calc N */
        case OP_TYPE1|OP_DONTCALCFLAGZ:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if ((BYTE)CPU.lastresult < 0)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            
            case OP_SIZE_WORD:
              if ((WORD)CPU.lastresult < 0)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_LONG:
              if ((LONG)CPU.lastresult < 0)
                return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;
      
        case OP_TYPE2:
        case OP_TYPE2|OP_DONTCALCFLAGZ:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if ((BYTE)CPU.lastresult < 0)
              {
                /* N=1 */
                if ((BYTE)CPU.oldvalue >= 0 || (BYTE)CPU.addvalue >= 0)
                  return TRUE;
              }
              else
              {
                /* N=0 */
                /* lastresult >= 0 */
                if ((BYTE)CPU.oldvalue < 0 && (BYTE)CPU.addvalue < 0)
                  return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_WORD:
              if ((WORD)CPU.lastresult < 0)
              {
                /* N=1 */
                if ((WORD)CPU.oldvalue >= 0 || (WORD)CPU.addvalue >= 0)
                  return TRUE;
              }
              else
              {
                /* N=0 */
                /* lastresult >= 0 */
                if ((WORD)CPU.oldvalue < 0 && (WORD)CPU.addvalue < 0)
                  return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_LONG:
              if ((LONG)CPU.lastresult < 0)
              {
                /* N=1 */
                if ((LONG)CPU.oldvalue >= 0 || (LONG)CPU.addvalue >= 0)
                  return TRUE;
              }
              else
              {
                /* N=0 */
                /* lastresult >= 0 */
                if ((LONG)CPU.oldvalue < 0 && (LONG)CPU.addvalue < 0)
                  return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;

        case OP_TYPE3:
        case OP_TYPE3|OP_DONTCALCFLAGZ:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if ((UBYTE)CPU.lastresult <= 0x80)
              {
                /* V=1 */
                if (0 == (BYTE)CPU.lastresult)
                  return TRUE;
              }
              else
              {
                /* V=0 */
                if (0 != (BYTE)CPU.lastresult)
                  return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_WORD:
              if ((UWORD)CPU.lastresult <= (UWORD)0x8000)
              {
                /* V=1 */
                if (0 == (WORD)CPU.lastresult)
                  return TRUE;
              }
              else
              {
                /* V=0 */
                if (0 != (WORD)CPU.lastresult)
                  return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_LONG:
              if ((ULONG)CPU.lastresult <= (ULONG)0x80000000)
              {
                /* V=1 */
                if (0 == (LONG)CPU.lastresult)
                  return TRUE;
              }
              else
              {
                /* V=0 */
                if (0 != (LONG)CPU.lastresult)
                  return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
          }
        break;        

        case OP_TYPE4:
        case OP_TYPE4|OP_DONTCALCFLAGZ:
        case OP_TYPE5|OP_DONTCALCFLAGZ:
        case OP_TYPE5:
          switch (CPU.lastop & OP_SIZE_MASK)
          {
            case OP_SIZE_BYTE:
              if ((BYTE)CPU.subvalue >= (BYTE)CPU.oldvalue)
              {
                if ((BYTE)CPU.lastresult != 0)
                  return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

            case OP_SIZE_WORD:
              if ((WORD)CPU.subvalue >= (WORD)CPU.oldvalue)
              {
                if ((WORD)CPU.lastresult != 0)
                  return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;
            
            case OP_SIZE_LONG:
              if ((LONG)CPU.subvalue >= (LONG)CPU.oldvalue)
              {
                if ((LONG)CPU.lastresult != 0)
                  return TRUE;
              }
SHOW_OPCODE("[NOT BRANCHING]\n");
              return FALSE;
            break;

          }
        break;
      }
    break;
    
    case 0x0e: /* BGT (N=1 & V=1 & Z=0) || (N=0 & V=0 & Z=0) */
SHOW_OPCODE("BGT%s ",oplength[size]);
      /* just calculate all the flags */
      calc_ccr();
      if ((CPUFLAG_N|CPUFLAG_V) == (CPU.SR & (CPUFLAG_N|CPUFLAG_V|CPUFLAG_Z))||
          0                     == (CPU.SR & (CPUFLAG_N|CPUFLAG_V|CPUFLAG_Z)))
        return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
      return FALSE; 
    break;
    
    case 0x0f: /* BLE (Z=1 || (N=1 & V=0) || (N=0 & V=1)) */
SHOW_OPCODE("BLE%s ",oplength[size]);
      /* just calculate all the flags */
      calc_ccr();
      if ((0         != (CPU.SR &  CPUFLAG_Z          ) )     ||
          (CPUFLAG_N == (CPU.SR & (CPUFLAG_N|CPUFLAG_V) ) )   ||
          (CPUFLAG_V == (CPU.SR & (CPUFLAG_N|CPUFLAG_V) ) ) )
        return TRUE;
SHOW_OPCODE("[NOT BRANCHING]\n");
      return FALSE; 
    break;
  }
}

