#include <exec/types.h>

#include "debug.h"
#include "endianess.h"
#include "m68k.h"

extern struct M68k CPU;

ULONG read_addressing_mode(UBYTE mode_field, UBYTE reg_field, UBYTE size)
{
  ULONG val;
  ULONG addr;
  
  switch (mode_field)
  {
    case 0: 

#ifdef DEBUGGING
switch (size)
{
  case 0: /* BYTE */
SHOW_OPCODE(".b D%d",reg_field);
  break;
              
  case 1: /* WORD */
SHOW_OPCODE(".w D%d",reg_field);
  break;
              
  case 2: /* LONG */
SHOW_OPCODE(".l D%d",reg_field);
  break;
}
#endif

      return (ULONG) CPU.D[reg_field];
    break;
    
    case 1: 
SHOW_OPCODE(".l A%d",reg_field);
            return CPU.A[reg_field];
    break;
    
    case 2:  /* (Ax) */

             switch (size)
             {
               case 0: /* BYTE */
SHOW_OPCODE(".b (A%d)",reg_field);
                     return *(UBYTE *)CPU.A[reg_field];
               break;
               
               case 1: /* WORD */
SHOW_OPCODE(".w (A%d)",reg_field);
                     return SWAP_WORD(*(UWORD *)CPU.A[reg_field]);
               break;

               case 2: /* LONG */
SHOW_OPCODE(".l (A%d)",reg_field);
                     return SWAP(*(ULONG *)CPU.A[reg_field]);
               break;
             }
    break;
    
    case 3: /* (Ax)+ */

             switch (size)
             {
               case 0: /* BYTE */
SHOW_OPCODE(".b (A%d)+",reg_field);
                     val = *(UBYTE *)CPU.A[reg_field];
                     CPU.A[reg_field]+=1;
                     return val;
               break;
               
               case 1: /* WORD */
SHOW_OPCODE(".w (A%d)+",reg_field);
                     val = *(UWORD *)CPU.A[reg_field];
                     CPU.A[reg_field]+=2;
                     return SWAP_WORD(val);
               break;

               case 2: /* LONG */
SHOW_OPCODE(".l (A%d)+",reg_field);
                     val = *(ULONG *)CPU.A[reg_field];
                     CPU.A[reg_field]+=4;
                     return SWAP(val);
               break;
             }
    break;

    case 4: /* -(Ax) */
             switch (size)
             {
               case 0: /* BYTE */
SHOW_OPCODE(".b -(A%d)",reg_field);
                     CPU.A[reg_field]-=1;
                     return *(UBYTE *)CPU.A[reg_field];
               break;
               
               case 1: /* WORD */
SHOW_OPCODE(".w -(A%d)",reg_field);
                     CPU.A[reg_field]-=2;
                     return SWAP_WORD(*(UWORD *)CPU.A[reg_field]);
               break;
               
               case 2: /* LONG */
SHOW_OPCODE(".l -(A%d)",reg_field);
                     CPU.A[reg_field]-=4;
                     return SWAP(*(ULONG *)CPU.A[reg_field]);
               break;
             }
    break;
    
    
    case 5: /* (d16, An) */
    {
      LONG disp = (LONG)SWAP_WORD(*(WORD *)(CPU.PC+2));
      CPU.instsize +=2;
      switch (size)
      {
        case 0:
SHOW_OPCODE(".b (%d,A%d)",disp,reg_field);
          return *(UBYTE *)(CPU.A[reg_field]+disp);
        break;
        
        case 1:
SHOW_OPCODE(".w (%d,A%d)",disp,reg_field);
          return SWAP_WORD(*(UWORD *)(CPU.A[reg_field]+disp));
        break;
        
        case 2:
SHOW_OPCODE(".l (%d,A%d)",disp,reg_field);
          return SWAP(*(ULONG *)(CPU.A[reg_field]+disp));
        break;
      }
    }
    break;
    
    case 6: /* (d8, An, Xn) */
    {
      UBYTE reg_field2 = (*(UBYTE *)(CPU.PC+2));
      CPU.instsize +=2;
      
      if (reg_field2 & 0x8)
        addr = CPU.D[reg_field2>>4];
      else     
        addr = (LONG)(WORD)((WORD *)&CPU.D[reg_field2>>4])[L_WORD];     

      addr += (LONG)(WORD)(*(BYTE *)(CPU.PC+3)) + CPU.A[reg_field];
      
      switch (size)
      {
        case 0:
SHOW_OPCODE(".b (%d,A%d,X%d)",*(UBYTE *)(CPU.PC+2),reg_field,reg_field2>>4);
          return *(UBYTE *)addr;
        break;
        
        case 1:
SHOW_OPCODE(".w (%d,A%d,X%d)",*(UBYTE *)(CPU.PC+2),reg_field,reg_field2>>4);
          return SWAP_WORD(*(UWORD *)addr);
        break;
        
        case 2:
SHOW_OPCODE(".l (%d,A%d,X%d)",*(UBYTE *)(CPU.PC+2),reg_field,reg_field2>>4);
          return SWAP(*(ULONG *)addr);
        break;
      }
    }
    break;
    
    case 7:
      switch (reg_field)
      {
        case 0: /* (xxx).w */
                CPU.instsize += 2;
                val = (ULONG)(UWORD)SWAP_WORD(*(UWORD *)(CPU.PC+2));
                switch (size)
                {
                  case 0:
SHOW_OPCODE(".b 0x%x",val);
                    return (*(UBYTE *)val);      
                  break;
                  
                  case 1:
SHOW_OPCODE(".w 0x%x",val);
                    return SWAP_WORD(*(UWORD *)val);      
                  break;
                  
                  case 2:
SHOW_OPCODE(".l 0x%x",val);
                    return SWAP(*(ULONG *)val);      
                  break;
                }
        break;
        
        case 1: /* (xxx).l */
                val = (ULONG)SWAP(*(ULONG *)(CPU.PC+2));
SHOW_OPCODE(" 0x%x",val);
                CPU.instsize += 4;
                switch (size)
                {
                  case 0:
SHOW_OPCODE(".b 0x%x",val);
                    return (*(UBYTE *)val);      
                  break;
                  
                  case 1:
SHOW_OPCODE(".w 0x%x",val);
                    return SWAP_WORD(*(UWORD *)val);      
                  break;
                  
                  case 2:
SHOW_OPCODE(".l 0x%x",val);
                    return SWAP(*(ULONG *)val);      
                  break;
                }
        break;
        
        case 2: /* (d16, PC) */
          CPU.instsize += 2;
          val = SWAP_WORD(*(UWORD *)(CPU.PC+2));
          switch (size)
          {
            case 0x00:
SHOW_OPCODE(".b (%d,PC) [???]",val);
              return (ULONG)(UBYTE)(*(UBYTE *)(val+CPU.PC+CPU.instsize));
            break;
            
            case 0x01:
SHOW_OPCODE(".w (%d,PC) [???]",val);
              return (UWORD)SWAP_WORD(*(UWORD *)(val+CPU.PC+CPU.instsize));
            break;
            
            case 0x02:
SHOW_OPCODE(".l (%d,PC) [???]",val);
              return SWAP(*(ULONG *)(val+CPU.PC+CPU.instsize));
            break;
          }
        break;
        
        case 3: /* (d8, PC, Xn) */
        {
          reg_field = (*(UBYTE *)(CPU.PC+2));
          
          CPU.instsize += 2;

          if (reg_field & 0x08)
            addr = CPU.D[reg_field>>4];
          else
            addr = ((UWORD *)&CPU.D[reg_field>>4])[L_WORD];

          addr += (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+3)) + CPU.PC +2;
          
          switch (size)
          {
            case 0x00:
SHOW_OPCODE(".b (%d,PC,X%d)",*(BYTE *)(CPU.PC+3),reg_field);
              return (ULONG)(UBYTE)(*(UBYTE *)addr);
            break;
            
            case 0x01:
SHOW_OPCODE(".w (%d,PC,X%d)",*(BYTE *)(CPU.PC+3),reg_field);
              return (ULONG)(UWORD)SWAP_WORD(*(UWORD *)addr);
            break;
            
            case 0x02:
SHOW_OPCODE(".l (%d,PC,X%d)",*(BYTE *)(CPU.PC+3),reg_field);
              return SWAP(*(ULONG *)addr);
            break;
          }
        }
        break;
        
        case 4: /* <#data> */ 
            switch (size)
            {
              case 0:
                val = SWAP_WORD(*(UWORD *)(CPU.PC+2));
                CPU.instsize += 2;
SHOW_OPCODE(".b #0x%02x",val);
                return val;
              break;
              
              case 1:
                val = SWAP_WORD(*(UWORD *)(CPU.PC+2));
                CPU.instsize += 2;
SHOW_OPCODE(".w #0x%04x",val);
                return (UWORD)val;
              break;
              
              case 2:
                val = SWAP(*(ULONG *)(CPU.PC+2));
                CPU.instsize += 4;
SHOW_OPCODE(".l #0x%08x",val);
                return val;
              break;
            }
        break;
      }
    break;
  }
  
  
  SHOW_OPCODE("\nunrecognized! -->mode_field: %d, reg_field: %d, size : %d\n",mode_field,reg_field,size);

  return 0xdeadbeef;
}


ULONG read_addressing_mode_nochange(UBYTE mode_field, UBYTE reg_field, UBYTE size)
{
  ULONG addr;
  ULONG val;
  
  switch (mode_field)
  {
    case 0: return (ULONG) CPU.D[reg_field];
    break;
    
    case 1: return CPU.A[reg_field];
    break;
    
    case 2:  /* (Ax) */
             switch (size)
             {
               case 0: /* BYTE */
                     return *(UBYTE *)CPU.A[reg_field];
               break;
               
               case 1: /* WORD */
                     return SWAP_WORD(*(UWORD *)CPU.A[reg_field]);
               break;
               
               case 2: /* LONG */
                     return SWAP(*(ULONG *)CPU.A[reg_field]);
               break;
             }
    break;
    
    case 3: /* (Ax)+ */

             switch (size)
             {
               case 0: /* BYTE */
                     return *(UBYTE *)CPU.A[reg_field];
               break;
               
               case 1: /* WORD */
                     return SWAP_WORD(*(UWORD *)CPU.A[reg_field]);
               break;
               
               case 2: /* LONG */
                     return SWAP(*(ULONG *)CPU.A[reg_field]);
               break;
             }
    break;

    case 4: /* -(Ax) */
             switch (size)
             {
               case 0: /* BYTE */
                     return *(UBYTE *)(CPU.A[reg_field]-1);
               break;
               
               case 2: /* LONG */
                     return SWAP(*(ULONG *)(CPU.A[reg_field])-2);
               break;
               
               case 1: /* WORD */
                     return SWAP_WORD(*(UWORD *)(CPU.A[reg_field])-4);
               break;
             }
    break;
    
    case 5: /* (d16, An) */
    {
      LONG disp = (LONG)SWAP_WORD(*(WORD *)(CPU.PC+2));
      switch (size)
      {
        case 0:
          return *(UBYTE *)(CPU.A[reg_field]+disp);
        break;
        
        case 1:
          return SWAP_WORD(*(UWORD *)(CPU.A[reg_field]+disp));
        break;
        
        case 2:
          return SWAP(*(ULONG *)(CPU.A[reg_field]+disp));
        break;
      }
    }
    break;
    
    case 6: /* (d8, An, Xn) */
    {
      UBYTE reg_field2 = (*(UBYTE *)(CPU.PC+2));

      if (reg_field2 & 0x8)
        addr = CPU.D[reg_field2>>4];
      else
        addr = (LONG)(WORD)((WORD *)&CPU.D[reg_field2>>4]);

      addr += (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+3)) + CPU.A[reg_field];

      switch (size)
      {
        case 0:
            return *(UBYTE *)addr;
        break;
        
        case 1:
            return SWAP_WORD(*(UWORD *)addr);
        break;
        
        case 2:
            return SWAP(*(ULONG *)addr);
        break;
      }
    }
    break;
    
    case 7:
      switch (reg_field)
      {
        case 0: /* (xxx).w */
                val = (ULONG)(UWORD)SWAP_WORD(*(UWORD *)(CPU.PC+2));
                switch (size)
                {
                  case 0:
                    return (*(UBYTE *)val);      
                  break;
                  
                  case 1:
                    return SWAP_WORD(*(UWORD *)val);      
                  break;
                  
                  case 2:
                    return SWAP(*(ULONG *)val);      
                  break;
                }
        break;
        
        case 1: /* (xxx).l */
          val = (ULONG)SWAP(*(ULONG *)(CPU.PC+2));
          switch (size)
          {
            case 0:
              return (*(UBYTE *)val);      
            break;
                  
            case 1:
              return SWAP_WORD(*(UWORD *)val);      
            break;
                  
            case 2:
              return SWAP(*(ULONG *)val);      
            break;
          }
        break;

        case 2: /* (d16, PC) */
          val = SWAP_WORD(*(UWORD *)(CPU.PC+2));
          switch (size)
          {
            case 0x00:
              return (ULONG)(UBYTE)(*(UBYTE *)(val+CPU.PC+2));
            break;
            
            case 0x01:
              return (UWORD)SWAP_WORD(*(UWORD *)(val+CPU.PC+2));
            break;
            
            case 0x02:
              return SWAP(*(ULONG *)(val+CPU.PC+2));
            break;
          }
        break;
        
        case 3: /* (d8, PC, Xn) */
        {
          reg_field = (*(UBYTE *)(CPU.PC+2));

          if (reg_field & 0x8)
            addr = CPU.A[reg_field >> 4];
          else
            addr = (LONG)(WORD)((WORD *)&CPU.A[reg_field >> 4])[L_WORD];

          addr += (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+3))+ CPU.PC + 2;

          switch (size)
          {
              case 0x00:
                return (ULONG)(UBYTE)(*(UBYTE *)addr);
              break;
            
              case 0x01:
                return (UWORD)SWAP_WORD(*(UWORD *)addr);
              break;
            
              case 0x02:
                return SWAP(*(ULONG *)addr);
              break;
          }
        }
        break;

        case 4: /* <#data> */ 
            switch (size)
            {
              case 0:
                val = SWAP_WORD(*(UWORD *)(CPU.PC+2));
                return val;
              break;
              
              case 1:
                val = SWAP_WORD(*(UWORD *)(CPU.PC+2));
                return (UWORD)val;
              break;
              
              case 2:
                val = SWAP(*(ULONG *)(CPU.PC+2));
                return val;
              break;
            }
        break;
      }
    break;
  }
    
  
  
  SHOW_OPCODE("\nunrecognized! -->mode_field: %d, reg_field: %d\n",mode_field,reg_field);

  return 0xdeadbeef;
}

ULONG read_addressing_mode_offset(UBYTE mode_field, UBYTE reg_field, UBYTE size)
{
  ULONG val;
  LONG addr;
  
  switch (mode_field)
  {
    case 0: 

#ifdef DEBUGGING
switch (size)
{
  case 0: /* BYTE */
SHOW_OPCODE(".b D%d",reg_field);
  break;
              
  case 1: /* WORD */
SHOW_OPCODE(".w D%d",reg_field);
  break;
              
  case 2: /* LONG */
SHOW_OPCODE(".l D%d",reg_field);
  break;
}
#endif

      return (ULONG) CPU.D[reg_field];
    break;
    
    case 1: 
SHOW_OPCODE(".l A%d",reg_field);
            return CPU.A[reg_field];
    break;
    
    case 2:  /* (Ax) */

             switch (size)
             {
               case 0: /* BYTE */
SHOW_OPCODE(".b (A%d)",reg_field);
                     return *(UBYTE *)CPU.A[reg_field];
               break;
               
               case 1: /* WORD */
SHOW_OPCODE(".w (A%d)",reg_field);
                     return SWAP_WORD(*(UWORD *)CPU.A[reg_field]);
               break;

               case 2: /* LONG */
SHOW_OPCODE(".l (A%d)",reg_field);
                     return SWAP(*(ULONG *)CPU.A[reg_field]);
               break;
             }
    break;
    
    case 3: /* (Ax)+ */

             switch (size)
             {
               case 0: /* BYTE */
SHOW_OPCODE(".b (A%d)+",reg_field);
                     val = *(UBYTE *)CPU.A[reg_field];
                     CPU.A[reg_field]+=1;
                     return val;
               break;
               
               case 1: /* WORD */
SHOW_OPCODE(".w (A%d)+",reg_field);
                     val = *(UWORD *)CPU.A[reg_field];
                     CPU.A[reg_field]+=2;
                     return SWAP_WORD(val);
               break;

               case 2: /* LONG */
SHOW_OPCODE(".l (A%d)+",reg_field);
                     val = *(ULONG *)CPU.A[reg_field];
                     CPU.A[reg_field]+=4;
                     return SWAP(val);
               break;
             }
    break;

    case 4: /* -(Ax) */
             switch (size)
             {
               case 0: /* BYTE */
SHOW_OPCODE(".b -(A%d)",reg_field);
                     CPU.A[reg_field]-=1;
                     return *(UBYTE *)CPU.A[reg_field];
               break;
               
               case 1: /* WORD */
SHOW_OPCODE(".w -(A%d)",reg_field);
                     CPU.A[reg_field]-=2;
                     return SWAP_WORD(*(UWORD *)CPU.A[reg_field]);
               break;
               
               case 2: /* LONG */
SHOW_OPCODE(".l -(A%d)",reg_field);
                     CPU.A[reg_field]-=4;
                     return SWAP(*(ULONG *)CPU.A[reg_field]);
               break;
             }
    break;
    
    
    case 5: /* (d16, An) */
    {
      LONG disp;
      CPU.instsize +=2;
      switch (size)
      {
        case 0:
          disp = (LONG)SWAP_WORD(*(WORD *)(CPU.PC+4));
SHOW_OPCODE(" (%d,A%d)",disp,reg_field);
          return *(UBYTE *)(CPU.A[reg_field]+disp);
        break;
        
        case 1:
          disp = (LONG)SWAP_WORD(*(WORD *)(CPU.PC+4));
SHOW_OPCODE(" (%d,A%d)",disp,reg_field);
          return SWAP_WORD(*(UWORD *)(CPU.A[reg_field]+disp));
        break;
        
        case 2:
          disp = (LONG)SWAP_WORD(*(WORD *)(CPU.PC+6));
SHOW_OPCODE(" (%d,A%d)",disp,reg_field);
          return SWAP(*(ULONG *)CPU.A[reg_field]+disp);
        break;
      }
    }
    break;
    
    case 6: /* (d8, An, Xn) */
    { 
      UBYTE reg_field2;
      CPU.instsize += 2;
      
      addr = CPU.A[reg_field];
      
      switch (size)
      {
        case 0: /* BYTE */
          reg_field2 = (*(UBYTE *)(CPU.PC+4));

          if (reg_field2 & 0x8)
            addr += CPU.D[reg_field2>>4];
          else
            addr += (LONG)(WORD)((WORD *)&CPU.D[reg_field2>>4])[L_WORD];

          addr += (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+5));
SHOW_OPCODE(" (%d,A%d,X%d)",*(BYTE *)(CPU.PC+5),reg_field,reg_field2>>4);
          return *(UBYTE *)addr;
        break;

        case 1: /* WORD */
          reg_field2 = (*(UBYTE *)(CPU.PC+4));

          if (reg_field2 & 0x8)
            addr += CPU.D[reg_field2>>4];
          else
            addr += (LONG)(WORD)((WORD *)&CPU.D[reg_field2>>4])[L_WORD];

          addr += (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+5));
SHOW_OPCODE(" (%d,A%d,X%d)",*(BYTE*)(CPU.PC+5),reg_field,reg_field2>>4);
          return SWAP_WORD(*(UWORD *)addr);
        break;

        case 2: /* LONG */
          reg_field2 = (*(UBYTE *)(CPU.PC+4));

          if (reg_field2 & 0x8)
            addr += CPU.D[reg_field2>>4];
          else
            addr += (LONG)(WORD)((WORD *)&CPU.D[reg_field2>>4])[L_WORD];

          addr += (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+7));
SHOW_OPCODE(" (%d,A%d,X%d)",*(BYTE *)(CPU.PC+7),reg_field,reg_field2>>4);
          return SWAP(*(ULONG *)addr);
        break;
      }
    }
    break;
    
    case 7:

      switch (reg_field)
      {
        case 0: /* (xxx).W */
          CPU.instsize+=2;
          switch (size)
          {
            case 0:
                val = (ULONG)SWAP_WORD(*(UWORD *)(CPU.PC+4));
SHOW_OPCODE(" 0x%x",val);
                return *(UBYTE *)val;
            break;

            case 1:
                val = (ULONG)SWAP_WORD(*(UWORD *)(CPU.PC+4));
SHOW_OPCODE(" 0x%x",val);
                return SWAP_WORD(*(UWORD *)val);
            break;
            
            case 2:
                val = (ULONG)SWAP_WORD(*(UWORD *)(CPU.PC+6));
SHOW_OPCODE(" 0x%x",val);
                return SWAP(*(ULONG *)val);
            break;
          }
        break;
        
        case 1: /* (xxx).L */
          CPU.instsize+=4;
          switch (size)
          {
            case 0:
                val = (ULONG)SWAP(*(ULONG *)(CPU.PC+4));
SHOW_OPCODE(" 0x%x",val);
                return *(UBYTE *)val;
            break;

            case 1:
                val = (ULONG)SWAP(*(ULONG *)(CPU.PC+4));
SHOW_OPCODE(" 0x%x",val);
                return SWAP_WORD(*(UWORD *)val);
            break;
            
            case 2:
                val = (ULONG)SWAP(*(ULONG *)(CPU.PC+6));
SHOW_OPCODE(" 0x%x",val);
                return SWAP(*(ULONG *)val);
            break;
          }
        break;
        
        case 2: /* (d16, PC) */
          CPU.instsize += 2;
          switch (size)
          {
            case 0x00:
              val = SWAP_WORD(*(UWORD *)(CPU.PC+4));
SHOW_OPCODE(".b (%d,PC) [Addresscalculation!!]",val);
              return (*(UBYTE *)(val+CPU.PC+6));
            break;
            
            case 0x01:
              val = SWAP_WORD(*(UWORD *)(CPU.PC+4));
SHOW_OPCODE(".w (%d,PC) [Addresscalculation!!]",val);
              return SWAP_WORD(*(UWORD *)(val+CPU.PC+6));
            break;
            
            case 0x02:
              val = SWAP_WORD(*(UWORD *)(CPU.PC+6));
SHOW_OPCODE(".l (%d,PC) [Addresscalculation!!]",val);
              return SWAP(*(ULONG *)(val+CPU.PC+6));
            break;
          }
        break;
        
        case 3: /* (d8, PC, Xn) */
        {
          CPU.instsize += 2;
          switch (size)
          {
            case 0x00:
              reg_field = *(UBYTE *)(CPU.PC+4);
              if (reg_field & 0x8)
                addr = CPU.D[reg_field>>4];
              else
                addr = (LONG)(WORD)((WORD *)&CPU.D[reg_field>>4])[L_WORD];
              addr = (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+5)) + 2 + CPU.PC;
SHOW_OPCODE(".b (%d,PC,X%d) [Addresscalculation!!]",*(UBYTE *)(CPU.PC+5),reg_field>>4);
              return (*(UBYTE *)addr);
            break;
            
            case 0x01:
              reg_field = *(UBYTE *)(CPU.PC+4);
              if (reg_field & 0x8)
                addr = CPU.D[reg_field>>4];
              else
                addr = (LONG)(WORD)((WORD *)&CPU.D[reg_field>>4])[L_WORD];
              addr = (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+5)) + 2 + CPU.PC;
SHOW_OPCODE(".w (%d,PC,X%d) [Addresscalculation!!]",*(UBYTE *)(CPU.PC+5),reg_field>>4);
                return SWAP_WORD(*(UWORD *)addr);
            break;
            
            case 0x02:
              reg_field = *(UBYTE *)(CPU.PC+6);
              if (reg_field & 0x8)
                addr = CPU.D[reg_field>>4];
              else
                addr = (LONG)(WORD)((WORD *)&CPU.D[reg_field>>4])[L_WORD];
              addr = (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+7)) + 2 + CPU.PC;
SHOW_OPCODE(".w (%d,PC,X%d) [Addresscalculation!!]",*(UBYTE *)(CPU.PC+7),reg_field>>4);
              return SWAP(*(ULONG *)addr);
            break;
          }
        }
        break;
        
        case 4: /* <#data> */
            switch (size)
            {
              case 0:
                val = SWAP_WORD(*(UWORD *)(CPU.PC+4));
                CPU.instsize += 2;
SHOW_OPCODE(".b 0x%02x",val);
                return val;
              break;
              
              case 1:
                val = SWAP_WORD(*(UWORD *)(CPU.PC+4));
                CPU.instsize += 2;
SHOW_OPCODE(".w 0x%04x",val);
                return (UWORD)val;
              break;
              
              case 2:
                val = SWAP(*(ULONG *)(CPU.PC+6));
                CPU.instsize += 4;
SHOW_OPCODE(".l 0x%08x",val);
                return val;
              break;
            }
        break;
      }
    break;
  }
  
  
  SHOW_OPCODE("\nunrecognized! -->mode_field: %d, reg_field: %d, size : %d\n",mode_field,reg_field,size);

  return 0xdeadbeef;
}


ULONG read_addressing_mode_nochange_offset(UBYTE mode_field, UBYTE reg_field, UBYTE size)
{
  ULONG addr;
  ULONG val;
  
  switch (mode_field)
  {
    case 0: return (ULONG) CPU.D[reg_field];
    break;
    
    case 1: return CPU.A[reg_field];
    break;
    
    case 2:  /* (Ax) */
             switch (size)
             {
               case 0: /* BYTE */
                     return *(UBYTE *)CPU.A[reg_field];
               break;
               
               case 1: /* WORD */
                     return SWAP_WORD(*(UWORD *)CPU.A[reg_field]);
               break;
               
               case 2: /* LONG */
                     return SWAP(*(ULONG *)CPU.A[reg_field]);
               break;
             }
    break;
    
    case 3: /* (Ax)+ */

             switch (size)
             {
               case 0: /* BYTE */
                     return *(UBYTE *)CPU.A[reg_field];
               break;
               
               case 1: /* WORD */
                     return SWAP_WORD(*(UWORD *)CPU.A[reg_field]);
               break;
               
               case 2: /* LONG */
                     return SWAP(*(ULONG *)CPU.A[reg_field]);
               break;
             }
    break;

    case 4: /* -(Ax) */
             switch (size)
             {
               case 0: /* BYTE */
                     return *(UBYTE *)(CPU.A[reg_field]-1);
               break;
               
               case 2: /* LONG */
                     return SWAP(*(ULONG *)(CPU.A[reg_field])-2);
               break;
               
               case 1: /* WORD */
                     return SWAP_WORD(*(UWORD *)(CPU.A[reg_field])-4);
               break;
             }
    break;
    
    case 5: /* (d16, An) */
    {
      LONG disp;
      CPU.instsize +=2;
      switch (size)
      {
        case 0:
          disp = (LONG)SWAP_WORD(*(WORD *)(CPU.PC+4));
SHOW_OPCODE(" (%d,A%d)",disp,reg_field);
          return *(UBYTE *)(CPU.A[reg_field]+disp);
        break;
        
        case 1:
          disp = (LONG)SWAP_WORD(*(WORD *)(CPU.PC+4));
SHOW_OPCODE(" (%d,A%d)",disp,reg_field);
          return SWAP_WORD(*(UWORD *)(CPU.A[reg_field]+disp));
        break;
        
        case 2:
          disp = (LONG)SWAP_WORD(*(WORD *)(CPU.PC+6));
SHOW_OPCODE(" (%d,A%d)",disp,reg_field);
          return SWAP(*(ULONG *)CPU.A[reg_field]+disp);
        break;
      }
    }
    break;
    
    case 6: /* (d8, An, Xn) */
    { 
      UBYTE reg_field2;
      
      addr = CPU.A[reg_field];
      
      switch (size)
      {
        case 0: /* BYTE */
          addr += (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+5));
          reg_field2 = (*(UBYTE *)(CPU.PC+4));
          
          if (reg_field2 & 0x08)
            addr += CPU.D[reg_field2 >> 4];
          else
            addr += (LONG)(WORD)((WORD *)&CPU.D[reg_field2>>4])[L_WORD];

SHOW_OPCODE(" (%d,A%d,X%d)",*(BYTE *)(CPU.PC+5),reg_field,reg_field2>>4);
          return *(UBYTE *)addr;
        break;

        case 1: /* WORD */
          addr += (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+5));
          reg_field2 = (*(UBYTE *)(CPU.PC+4));
          
          if (reg_field2 & 0x08)
            addr += CPU.D[reg_field2 >> 4];
          else
            addr += (LONG)(WORD)((WORD *)&CPU.D[reg_field2>>4])[L_WORD];

SHOW_OPCODE(" (%d,A%d,X%d)",*(BYTE *)(CPU.PC+5),reg_field,reg_field2>>4);
          return SWAP_WORD(*(UWORD *)addr);
        break;

        case 2: /* LONG */
          addr += (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+7));
          reg_field2 = (*(UBYTE *)(CPU.PC+6));
          
          if (reg_field2 & 0x08)
            addr += CPU.D[reg_field2 >> 4];
          else
            addr += (LONG)(WORD)((WORD *)&CPU.D[reg_field2>>4])[L_WORD];

SHOW_OPCODE(" (%d,A%d,X%d)",*(BYTE *)(CPU.PC+5),reg_field,reg_field2>>4);
          return SWAP(*(ULONG *)addr);
        break;
      }
    }
    break;
    
    case 7:

      switch (reg_field)
      {
        case 0: /* (xxx).W */
          CPU.instsize+=2;
          switch (size)
          {
            case 0:
                val = (ULONG)SWAP_WORD(*(UWORD *)(CPU.PC+4));
SHOW_OPCODE(" 0x%x",val);
                return *(UBYTE *)val;
            break;

            case 1:
                val = (ULONG)SWAP_WORD(*(UWORD *)(CPU.PC+4));
SHOW_OPCODE(" 0x%x",val);
                return SWAP_WORD(*(UWORD *)val);
            break;
            
            case 2:
                val = (ULONG)SWAP_WORD(*(UWORD *)(CPU.PC+6));
SHOW_OPCODE(" 0x%x",val);
                return SWAP(*(ULONG *)val);
            break;
          }
        break;
        
        case 1: /* (xxx).L */
          CPU.instsize+=4;
          switch (size)
          {
            case 0:
                val = (ULONG)SWAP(*(ULONG *)(CPU.PC+4));
SHOW_OPCODE(" 0x%x",val);
                return *(UBYTE *)val;
            break;

            case 1:
                val = (ULONG)SWAP(*(ULONG *)(CPU.PC+4));
SHOW_OPCODE(" 0x%x",val);
                return SWAP_WORD(*(UWORD *)val);
            break;
            
            case 2:
                val = (ULONG)SWAP(*(ULONG *)(CPU.PC+6));
SHOW_OPCODE(" 0x%x",val);
                return SWAP(*(ULONG *)val);
            break;
          }
        break;
        
        case 2: /* (d16, PC) */
          CPU.instsize += 2;
          switch (size)
          {
            case 0x00:
              val = SWAP_WORD(*(UWORD *)(CPU.PC+4));
SHOW_OPCODE(".b (%d,PC) [Addresscalculation!!]",val);
              return (*(UBYTE *)(val+CPU.PC+6));
            break;
            
            case 0x01:
              val = SWAP_WORD(*(UWORD *)(CPU.PC+4));
SHOW_OPCODE(".w (%d,PC) [Addresscalculation!!]",val);
              return SWAP_WORD(*(UWORD *)(val+CPU.PC+6));
            break;
            
            case 0x02:
              val = SWAP_WORD(*(UWORD *)(CPU.PC+6));
SHOW_OPCODE(".l (%d,PC) [Addresscalculation!!]",val);
              return SWAP(*(ULONG *)(val+CPU.PC+6));
            break;
          }
        break;
        
        case 3: /* (d8, PC, Xn) */
        {
          switch (size)
          {
            case 0x00:
              reg_field = *(BYTE *)(CPU.PC+4);
              if (reg_field & 0x8)
                addr = CPU.A[reg_field >> 4];
              else
                addr = ((UWORD *)&CPU.A[reg_field >> 4])[L_WORD];
                
              addr += (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+5)) + CPU.PC + 2;
              
              return *(UBYTE *)addr;
            break;
            
            case 0x01:
              reg_field = *(BYTE *)(CPU.PC+4);
              if (reg_field & 0x8)
                addr = CPU.A[reg_field >> 4];
              else
                addr = ((UWORD *)&CPU.A[reg_field >> 4])[L_WORD];
                
              addr += (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+5)) + CPU.PC + 2;
              
              return SWAP_WORD(*(UWORD *)addr);
            break;
            
            case 0x02:
              reg_field = *(BYTE *)(CPU.PC+6);
              if (reg_field & 0x8)
                addr = CPU.A[reg_field >> 4];
              else
                addr = ((UWORD *)&CPU.A[reg_field >> 4])[L_WORD];
                
              addr += (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+7)) + CPU.PC + 2;
              
              return SWAP(*(ULONG *)addr);
            break;
          }
        }
        break;
        
        case 4: 
            switch (size)
            {
              case 0:
                val = SWAP_WORD(*(UWORD *)(CPU.PC+4));
                CPU.instsize += 2;
SHOW_OPCODE(".b 0x%02x",val);
                return val;
              break;
              
              case 1:
                val = SWAP_WORD(*(UWORD *)(CPU.PC+4));
                CPU.instsize += 2;
SHOW_OPCODE(".w 0x%04x",val);
                return (UWORD)val;
              break;
              
              case 2:
                val = SWAP(*(ULONG *)(CPU.PC+6));
                CPU.instsize += 4;
SHOW_OPCODE(".l 0x%08x",val);
                return val;
              break;
            }
        break;
      }
    break;
  }
  
  
  SHOW_OPCODE("\nunrecognized! -->mode_field: %d, reg_field: %d\n",mode_field,reg_field);

  return 0xdeadbeef;
}


/* LEA needs the following function */
ULONG getaddress_addressing_mode(UBYTE mode_field, UBYTE reg_field, UBYTE size)
{
  LONG offset;
  LONG val;
  LONG addr;
  
  switch (mode_field)
  {
    case 0: /* not possible */
    break;
    
    case 1: /* not possible */
    break;
    
    case 2:  /* (Ax) */
SHOW_OPCODE(".l (A%d)",reg_field);
      return CPU.A[reg_field];
    break;
    
    case 3: /* not possible */
    break;

    case 4: /* not possible */
    break;
    
    
    case 5: /* (d16, An) */
    {
      offset = (LONG)(WORD)SWAP_WORD(*(WORD *)(CPU.PC+2));
SHOW_OPCODE(" (%d,A%d)",offset,reg_field);
      CPU.instsize +=2;
      return CPU.A[reg_field]+offset;
    }
    break;
    
    case 6: /* (d8, An, Xn) */
    {
      UBYTE reg_field2 = (*(UBYTE *)(CPU.PC+2));
      
      if (reg_field2 & 0x8)
        addr = CPU.D[reg_field2>>4];
      else 
        addr = (LONG)(WORD)((UWORD *)&CPU.D[reg_field2>>4])[L_WORD];
        
      addr += (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+3))+CPU.A[reg_field];

SHOW_OPCODE(" (%d.w,A%d,X%d) ; %x [CPU.instsize += 2??]",*(BYTE *)(CPU.PC+3),reg_field,reg_field2>>4,addr);
      CPU.instsize +=2;
      return (LONG)addr;
    }
    break;
    
    case 7:
      switch (reg_field)
      {
        case 0: /* (xxx).W */
                CPU.instsize += 2;
                val = (ULONG)(UWORD)SWAP_WORD((ULONG)*(UWORD *)(CPU.PC+2));
SHOW_OPCODE(" $%x",val);
                return val;      
        break;
        
        case 1: /* (xxx).L */
                CPU.instsize += 4;
                val = (ULONG)SWAP(*(ULONG *)(CPU.PC+2));
SHOW_OPCODE(" $%x",val);
                return (ULONG)val;
        break;
        
        case 2: /* (d16, PC) */
                CPU.instsize += 2;
                offset = (LONG)(WORD)SWAP_WORD(*(UWORD *)(CPU.PC+2));
SHOW_OPCODE(" (%d.w,PC)",offset);
                return CPU.PC+2+offset;
        break;
        
        case 3: /* (d8, PC, Xn) */
        {
          reg_field = (*(UBYTE *)(CPU.PC+2));
          
          CPU.instsize += 2;
          
          if (reg_field & 0x8)
            addr = CPU.D[reg_field>>4];
          else
            addr = ((UWORD *)&CPU.D[reg_field>>4])[L_WORD];
          
          addr += (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+3)) + CPU.PC + 2;

#ifdef DEBUGGING
            switch (size)
            {
              case 0x00:
SHOW_OPCODE(".b (%d,PC,X%d)",val,reg_field>>4);
              break;
            
              case 0x01:
SHOW_OPCODE(".w (%d,PC,X%d)",val,reg_field>>4);
              break;
            
              case 0x02:
SHOW_OPCODE(".l (%d,PC,X%d)",val,reg_field>>4);
              break;
            }
#endif
          return (ULONG)addr;
        }
        break;
        
      }
    break;
  }
  
  
  SHOW_OPCODE("\nunrecognized! -->mode_field: %d, reg_field: %d, size : %d\n",mode_field,reg_field,size);

  return 0xdeadbeef;
}



VOID write_addressing_mode(UBYTE mode_field, UBYTE reg_field, ULONG val, UBYTE size)
{
  ULONG offset;
  ULONG addr;
  switch (mode_field)
  {
    case 0:  
             switch (size)
             {
               case 0: /* BYTE */
SHOW_OPCODE("D%d",reg_field);
                 (((BYTE *)&CPU.D[reg_field]))[L_BYTE] = (UBYTE)val;
               break;
               
               case 1: /* WORD */
SHOW_OPCODE("D%d",reg_field);
                 (((WORD *)&CPU.D[reg_field]))[L_WORD] = (UWORD)val;
               break;
               
               case 2: /* LONG */
SHOW_OPCODE("D%d",reg_field);
                 (LONG)CPU.D[reg_field] = (ULONG)val;
               break;
             }
             return;
    break;
    
    case 1:  CPU.A[reg_field] = val;
SHOW_OPCODE("A%d",reg_field);
             return;
    break;
    
    case 2:  /* (Ax) */
SHOW_OPCODE("(A%d)",reg_field);
             switch (size)
             {
               case 0: /* BYTE */
                     *(UBYTE *)CPU.A[reg_field] = val;
                     return;
               break;
               
               case 1: /* WORD */
                     *(UWORD *)CPU.A[reg_field] = SWAP_WORD(val);
                     return;
               break;
               
               case 2: /* LONG */
                     *(ULONG *)CPU.A[reg_field] = SWAP(val);
                     return;
               break;
             }
    break;
    
    case 3: /* (Ax)+ */
SHOW_OPCODE("(A%d)+",reg_field);
             switch (size)
             {
               case 0: /* BYTE */
                     *(UBYTE *)CPU.A[reg_field] = val;
                     CPU.A[reg_field]+=1;
                     return;
               break;
               
               case 1: /* WORD */
                     *(UWORD *)CPU.A[reg_field] = SWAP_WORD(val);
                     CPU.A[reg_field]+=2;
                     return;
               break;
               
               case 3: /* LONG */
                     *(ULONG *)CPU.A[reg_field] = SWAP(val);
                     CPU.A[reg_field]+=4;
                     return;
               break;
             }
    break;

    case 4: /* -(Ax) */
SHOW_OPCODE("-(A%d)",reg_field);
             switch (size)
             {
               case 0: /* BYTE */
                     CPU.A[reg_field]-=1;
                     *(UBYTE *)CPU.A[reg_field] = val;
                     return;
               break;
               
               case 1: /* WORD */
                     CPU.A[reg_field]-=2;
                     *(UWORD *)CPU.A[reg_field] = SWAP_WORD(val);
                     return;
               break;
               
               case 2: /* LONG */
                     CPU.A[reg_field]-=4;
                     *(ULONG *)CPU.A[reg_field] = SWAP(val);
                     return;
               break;
             }
    break;
    
    case 5: /* (d16, An) */
      CPU.instsize += 2;
      offset = (LONG)(WORD)SWAP_WORD(*(UWORD *)(CPU.PC+2));
      switch (size)
      {
        case 0:
SHOW_OPCODE("%x(A%d) ; %x\n",offset,reg_field,CPU.A[reg_field]+(LONG)offset);
          *(UBYTE *)(CPU.A[reg_field]+(LONG)offset) = (UBYTE)val;
          return;
        break;
        
        case 1:
SHOW_OPCODE("%x(A%d) ; %x\n",offset,reg_field,CPU.A[reg_field]+(LONG)offset);
          *(UWORD *)(CPU.A[reg_field]+(LONG)offset) = (UWORD)SWAP_WORD(val);
          return;
        break;
        
        case 2:
SHOW_OPCODE("%x(A%d) ; %x\n",offset,reg_field,CPU.A[reg_field]+(LONG)offset);
          *(ULONG *)(CPU.A[reg_field]+(LONG)offset) = (ULONG)SWAP(val);
          return;
        break;
      }
    break;
    
    case 6: /* (d8, An, Dn) */
    {
      UBYTE reg_field2 = (*(UBYTE *)(CPU.PC+2));  
      CPU.instsize += 2;
      
      if (reg_field2 & 0x8)
        addr = CPU.D[reg_field2>>4];
      else
        addr = (LONG)(WORD)((UWORD *)&CPU.D[reg_field2>>4])[L_WORD];
      
      addr += (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+3)) + CPU.A[reg_field];
      
      switch (size)
      {
        case 0:
SHOW_OPCODE("%x(A%d,X%d) ; %x\n",*(BYTE *)(CPU.PC+3),reg_field,reg_field2>>4,addr);
          *(UBYTE *)addr = (UBYTE)val;
          return;
        break;
        
        case 1:
SHOW_OPCODE("%x(A%d,X%d) ; %x\n",*(BYTE *)(CPU.PC+3),reg_field,reg_field2>>4,addr);
          *(UWORD *)addr = (UWORD)SWAP_WORD(val);
          return;
        break;
        
        case 2:
SHOW_OPCODE("%x(A%d,X%d) ; %x\n",*(BYTE *)(CPU.PC+3),reg_field,reg_field2>>4,addr);
          *(ULONG *)addr = (ULONG)SWAP(val);
          return;
        break;
      }
    }
    break;
    
    case 7:
      switch (reg_field)
      {
        case 0: /* (xxx).W */
          addr = (ULONG)SWAP_WORD(*(UWORD *)(CPU.PC+2));
SHOW_OPCODE("$%x",addr);
          CPU.instsize += 2;
          switch (size)
          {
            case 0:
              *(UBYTE *)addr = val;
              return;
            break;
            
            case 1:
              *(UWORD *)addr = SWAP_WORD(val);
              return;
            break;
            
            case 2:
              *(ULONG *)addr = SWAP(val);
              return;
            break;
          }
        break;
        
        case 1: /* (xxx).L */
          addr = (ULONG)SWAP(*(ULONG *)(CPU.PC+2));
SHOW_OPCODE("$%x",addr);
          CPU.instsize += 2;
          switch (size)
          {
            case 0:
              *(UBYTE *)addr = val;
              return;
            break;
            
            case 1:
              *(UWORD *)addr = SWAP_WORD(val);
              return;
            break;
            
            case 2:
              *(ULONG *)addr = SWAP(val);
              return;
            break;
          }
        break;
        
        case 2: /* (d16, PC) */
          addr = (LONG)(WORD)SWAP_WORD(*(UWORD *)(CPU.PC+2));
          addr += CPU.PC;
SHOW_OPCODE("(%d.w, PC) ; $%x [???]",SWAP_WORD(*(UWORD *)(CPU.PC+2)),addr);
          CPU.instsize += 2;
          switch (size)
          {
            case 0:
              *(UBYTE *)addr = val;
              return;
            break;
            
            case 1:
              *(UWORD *)addr = SWAP_WORD(val);
              return;
            break;
            
            case 2:
              *(ULONG *)addr = SWAP(val);
              return;
            break;
          }
        break;
        
        case 3: /* (d8, PC, Xn) */
        {
          reg_field = (*(UBYTE *)(CPU.PC+2));
          
          if (reg_field & 0x08)
            addr = CPU.D[reg_field>>4];
          else
            addr = (LONG)((WORD *)&CPU.D[reg_field>>4])[L_WORD];

          addr += (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+3)) + CPU.PC + 2;
          
          CPU.instsize += 2;

          switch (size)
          {
            case 0:
              *(UBYTE *)addr = val;
              return;
            break;
            
            case 1:
              *(UWORD *)addr = SWAP_WORD(val);
              return;
            break;
            
            case 2:
              *(ULONG *)addr = SWAP(val);
              return;
            break;
          }

        }
        break;

      }
    break;
  }
  
  kprintf("Unknow addressing mode for writing data!\n");
}

VOID write_addressing_mode_offset(UBYTE mode_field, UBYTE reg_field, ULONG val, UBYTE size)
{
  ULONG offset;
  ULONG addr;
  switch (mode_field)
  {
    case 0:  
             switch (size)
             {
               case 0: /* BYTE */
SHOW_OPCODE("D%d",reg_field);
                 (((BYTE *)&CPU.D[reg_field]))[L_BYTE] = (UBYTE)val;
               break;
               
               case 1: /* WORD */
SHOW_OPCODE("D%d",reg_field);
                 (((WORD *)&CPU.D[reg_field]))[L_WORD] = (UWORD)val;
               break;
               
               case 2: /* LONG */
SHOW_OPCODE("D%d",reg_field);
                 (LONG)CPU.D[reg_field] = (ULONG)val;
               break;
             }
             return;
    break;
    
    case 1:  CPU.A[reg_field] = val;
SHOW_OPCODE("A%d",reg_field);
             return;
    break;
    
    case 2:  /* (Ax) */
SHOW_OPCODE("(A%d)",reg_field);
             switch (size)
             {
               case 0: /* BYTE */
                     *(UBYTE *)CPU.A[reg_field] = val;
                     return;
               break;
               
               case 1: /* WORD */
                     *(UWORD *)CPU.A[reg_field] = SWAP_WORD(val);
                     return;
               break;
               
               case 2: /* LONG */
                     *(ULONG *)CPU.A[reg_field] = SWAP(val);
                     return;
               break;
             }
    break;
    
    case 3: /* (Ax)+ */
SHOW_OPCODE("(A%d)+",reg_field);
             switch (size)
             {
               case 0: /* BYTE */
                     *(UBYTE *)CPU.A[reg_field] = val;
                     CPU.A[reg_field]+=1;
                     return;
               break;
               
               case 1: /* WORD */
                     *(UWORD *)CPU.A[reg_field] = SWAP_WORD(val);
                     CPU.A[reg_field]+=2;
                     return;
               break;
               
               case 3: /* LONG */
                     *(ULONG *)CPU.A[reg_field] = SWAP(val);
                     CPU.A[reg_field]+=4;
                     return;
               break;
             }
    break;

    case 4: /* -(Ax) */
SHOW_OPCODE("-(A%d)",reg_field);
             switch (size)
             {
               case 0: /* BYTE */
                     CPU.A[reg_field]-=1;
                     *(UBYTE *)CPU.A[reg_field] = val;
                     return;
               break;
               
               case 1: /* WORD */
                     CPU.A[reg_field]-=2;
                     *(UWORD *)CPU.A[reg_field] = SWAP_WORD(val);
                     return;
               break;
               
               case 2: /* LONG */
                     CPU.A[reg_field]-=4;
                     *(ULONG *)CPU.A[reg_field] = SWAP(val);
                     return;
               break;
             }
    break;
    
    case 5: /* (d16, An) */
      CPU.instsize += 2;
      switch (size)
      {
        case 0:
          offset = (LONG)(WORD)SWAP_WORD(*(UWORD *)(CPU.PC+4));
SHOW_OPCODE("%x(A%d) ; %x\n",offset,reg_field,CPU.A[reg_field]+(LONG)offset);
          *(UBYTE *)(CPU.A[reg_field]+(LONG)offset) = (UBYTE)val;
          return;
        break;
        
        case 1:
          offset = (LONG)(WORD)SWAP_WORD(*(UWORD *)(CPU.PC+4));
SHOW_OPCODE("%x(A%d) ; %x\n",offset,reg_field,CPU.A[reg_field]+(LONG)offset);
          *(UWORD *)(CPU.A[reg_field]+(LONG)offset) = (UWORD)SWAP_WORD(val);
          return;
        break;
        
        case 2:
          offset = (LONG)(WORD)SWAP_WORD(*(UWORD *)(CPU.PC+6));
SHOW_OPCODE("%x(A%d) ; %x\n",offset,reg_field,CPU.A[reg_field]+(LONG)offset);
          *(ULONG *)(CPU.A[reg_field]+(LONG)offset) = (ULONG)SWAP(val);
          return;
        break;
      }
    break;
    
    case 6: /* (d8, An, Xn) */
    {
      UBYTE reg_field2;
      CPU.instsize += 2;
      
      addr = CPU.A[reg_field];
      
      switch (size)
      {
        case 0:
          reg_field2 = (*(UBYTE *)(CPU.PC+4));
          
          if (reg_field2 & 0x8)
            addr += CPU.D[reg_field2>>4];
          else
            addr += (LONG)(WORD)((UWORD *)&CPU.D[reg_field2>>4])[L_WORD];
            
          addr += (LONG)(WORD)(BYTE)(*(UBYTE *)(CPU.PC+5));
SHOW_OPCODE("%x(A%d,X%d) ; %x\n",*(UBYTE *)(CPU.PC+5),reg_field,reg_field2>>4,addr);
          *(UBYTE *)addr = (UBYTE)val;
          return;
        break;
        
        case 1:
          reg_field2 = (*(UBYTE *)(CPU.PC+4));
          
          if (reg_field2 & 0x8)
            addr += CPU.D[reg_field2>>4];
          else
            addr += (LONG)(WORD)((UWORD *)&CPU.D[reg_field2>>4])[L_WORD];

          addr += (LONG)(WORD)(BYTE)(*(UBYTE *)(CPU.PC+5));
SHOW_OPCODE("%x(A%d,X%d) ; %x\n",*(UBYTE *)(CPU.PC+5),reg_field,reg_field2>>4,addr);
          *(UWORD *)addr = (UWORD)SWAP_WORD(val);
          return;
        break;
        
        case 2:
          reg_field2 = (*(UBYTE *)(CPU.PC+6));
          
          if (reg_field2 & 0x8)
            addr += CPU.D[reg_field2>>4];
          else
            addr += (LONG)(WORD)((UWORD *)&CPU.D[reg_field2>>4])[L_WORD];
            
          addr += (LONG)(WORD)(BYTE)(*(UBYTE *)(CPU.PC+7));
SHOW_OPCODE("%x(A%d,X%d) ; %x\n",*(UBYTE *)(CPU.PC+7),reg_field,reg_field2>>4,addr);
          *(ULONG *)addr = (ULONG)SWAP(val);
          return;
        break;
      }
    }
    break;
    
    case 7:
      switch (reg_field)
      {
        case 0: /* (xxx).W */
          CPU.instsize += 2;
          switch (size)
          {
            case 0:
              addr = (ULONG)SWAP_WORD(*(UWORD *)(CPU.PC+4));
SHOW_OPCODE("$%x.w",addr);
              *(UBYTE *)addr = val;
              return;
            break;
            
            case 1:
              addr = (ULONG)SWAP_WORD(*(UWORD *)(CPU.PC+4));
SHOW_OPCODE("$%x.w",addr);
              *(UWORD *)addr = SWAP_WORD(val);
              return;
            break;
            
            case 2:
              addr = (ULONG)SWAP_WORD(*(UWORD *)(CPU.PC+6));
SHOW_OPCODE("$%x.w",addr);
              *(ULONG *)addr = SWAP(val);
              return;
            break;
          }
        break;
        
        case 1: /* (xxx).L */
          CPU.instsize += 2;
          switch (size)
          {
            case 0:
              addr = (ULONG)SWAP(*(ULONG *)(CPU.PC+4));
SHOW_OPCODE("$%x.l",addr);
              *(UBYTE *)addr = val;
              return;
            break;
            
            case 1:
              addr = (ULONG)SWAP(*(ULONG *)(CPU.PC+4));
SHOW_OPCODE("$%x.l",addr);
              *(UWORD *)addr = SWAP_WORD(val);
              return;
            break;
            
            case 2:
              addr = (ULONG)SWAP(*(ULONG *)(CPU.PC+6));
SHOW_OPCODE("$%x.l",addr);
              *(ULONG *)addr = SWAP(val);
              return;
            break;
          }
        break;
        
        case 2: /* (d16, PC) */
          CPU.instsize += 2;
          switch (size)
          {
            case 0:
              addr = (LONG)(WORD)SWAP_WORD(*(UWORD *)(CPU.PC+4));
              addr += CPU.PC;
SHOW_OPCODE("(%d.w, PC) ; $%x [???]",SWAP_WORD(*(UWORD *)(CPU.PC+2)),addr);
              *(UBYTE *)addr = val;
              return;
            break;
            
            case 1:
              addr = (LONG)(WORD)SWAP_WORD(*(UWORD *)(CPU.PC+4));
              addr += CPU.PC;
SHOW_OPCODE("(%d.w, PC) ; $%x [???]",SWAP_WORD(*(UWORD *)(CPU.PC+2)),addr);
              *(UWORD *)addr = SWAP_WORD(val);
              return;
            break;
            
            case 2:
              addr = (LONG)(WORD)SWAP_WORD(*(UWORD *)(CPU.PC+6));
              addr += CPU.PC;
SHOW_OPCODE("(%d.w, PC) ; $%x [???]",SWAP_WORD(*(UWORD *)(CPU.PC+2)),addr);
              *(ULONG *)addr = SWAP(val);
              return;
            break;
          }
        break;
        
        case 3: /* (d8, PC, Xn) */
          CPU.instsize += 2;
          switch (size)
          {
            case 0:
              reg_field = (*(UBYTE *)(CPU.PC+4));
              addr = (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+5)) + CPU.PC + 2;
              if (reg_field & 8)
              {
                addr += CPU.D[reg_field>>4];
SHOW_OPCODE("(%d.w,PC,X%d.l) ; $%x [???]",(*(UBYTE *)(CPU.PC+5)),reg_field>>4,addr);
              }
              else
              {
                addr += (LONG)(WORD)((WORD *)&CPU.D[reg_field>>4])[L_WORD];
SHOW_OPCODE("(%d.w,PC,X%d.w) ; $%x [???]",(*(UBYTE *)(CPU.PC+5)),reg_field>>4,addr);
              }
              *(UBYTE *)addr = val;
              return;
            break;
            
            case 1:
              reg_field = (*(BYTE *)(CPU.PC+4));
              addr = (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+5)) + CPU.PC + 2;
              if (reg_field & 8)
              {
                addr += CPU.D[reg_field>>4];
SHOW_OPCODE("(%d.w,PC,X%d.l) ; $%x [???]",(*(UBYTE *)(CPU.PC+5)),reg_field>>4,addr);
              }
              else
              {
                addr += (LONG)(WORD)((WORD *)&CPU.D[reg_field>>4])[L_WORD];
SHOW_OPCODE("(%d.w,PC,X%d.w) ; $%x [???]",(*(UBYTE *)(CPU.PC+5)),reg_field>>4,addr);
              }
              *(UWORD *)addr = SWAP_WORD(val);
              return;
            break;
            
            case 2:
              reg_field = (*(BYTE *)(CPU.PC+6));
              addr = (LONG)(WORD)(BYTE)(*(BYTE *)(CPU.PC+7)) + CPU.PC + 2;
              if (reg_field & 8)
              {
                addr += CPU.D[reg_field>>4];
SHOW_OPCODE("(%d.w,PC,X%d.l) ; $%x [???]",(*(UBYTE *)(CPU.PC+7)),reg_field>>4,addr);
              }
              else
              {
                addr += (LONG)(WORD)((WORD *)&CPU.D[reg_field>>4])[L_WORD];
SHOW_OPCODE("(%d.w,PC,X%d.w) ; $%x [???]",(*(UBYTE *)(CPU.PC+7)),reg_field>>4,addr);
              }
              *(ULONG *)addr = SWAP(val);
              return;
            break;
          }
        break;
      }
    break;
  }
  kprintf("Unknow addressing mode for writing data!\n");
}
