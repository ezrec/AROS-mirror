/*
 * FryingPan - Amiga CD/DVD Recording Software (User Interface and supporting Libraries only)
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <libclass/dos.h>
#include <dos/dos.h>
#include <dos/dosextens.h>

#define BIT(m, x) (((((char*)m)[x>>3])&(1<<(7-(x&7))))?1:0)

long getnbits(unsigned char* mem)
{
   static int x=0;      // offset in bits;
   static int cp=0;
   int        cq=0;
   unsigned int c=0;
   int        k=7;

   while (k--) {
      c<<=1;
      c |= BIT(mem, x);
      x++;
   }

   cp <<= 3;
   cp  ^= c;
   cp += 13;
   cp &=0x3ff;


   k = 10;
   cq = 0;

   while (k--) {
      cq<<=1;
      cq |= BIT(mem, x);
      x++;
   }

   if (cp != cq) return  -1;

   return c;
}

int odd_bits(char i)
{
   char k,l=0;
   for (k=0; k<7; k++) {
      if (i & (1<<k)) l++;
   }
   return l&1;
}

int decompose(int i)
{
   int p = 0;
   p |= (odd_bits(i&0x55));
   p |= (odd_bits(i&0x66)<<1);
   p |= (odd_bits(i&0x78)<<2);

   return p;
}


bool analyze(bool &bState)
{
   int k=0,l;
   char lst=0;
   long lVal=0;
   BPTR fh;
   char *repository; 
   Process *ths = (Process*)Exec->FindTask(0);
   void* wnd;

   bState = false;

   wnd = ths->pr_WindowPtr;
   ths->pr_WindowPtr = (void*)~0U;

   fh = DOS->Open("L:Optical.key", MODE_OLDFILE);
   if (0 == fh)
      fh = DOS->Open("Devs:Optical.key", MODE_OLDFILE);
   if (0 == fh)
      fh = DOS->Open("Devs:keyfiles/Optical.key", MODE_OLDFILE);
   if (0 == fh)
      fh = DOS->Open("S:Optical.key", MODE_OLDFILE);
   if (0 == fh)
      fh = DOS->Open("Optical.key", MODE_OLDFILE);

   ths->pr_WindowPtr = wnd;

   if (0 == fh)
   {
      return false;
   }
   
   while (-1 != (l = DOS->FGetC(fh)))
   {
      lVal <<= 8;
      lVal |= (l&0xff);
      if (lVal == 0x3e9)
         break;
   }
   
   bState = false;

   if (lVal != 0x3e9)
   {
      DOS->Close(fh);     // no hunk 0x3e9
      return false;
   }

   lVal <<= 8;
   lVal |= DOS->FGetC(fh);
   lVal <<= 8;
   lVal |= DOS->FGetC(fh);
   lVal <<= 8;
   lVal |= DOS->FGetC(fh);
   lVal <<= 8;
   lVal |= DOS->FGetC(fh);
   lVal <<= 2;

   bState = false;

   repository = new char[lVal];
   if (!repository)
   {
      DOS->Close(fh);
      return false;
   }
   
   DOS->Flush(fh);
   
   if ((DOS->Read(fh, (void*)repository, lVal)) != lVal)
   {
      DOS->Close(fh);
      delete [] repository;
      return false;
   }

   bState = false;

   lVal <<= 8;
   lVal |= DOS->FGetC(fh);
   lVal <<= 8;
   lVal |= DOS->FGetC(fh);
   lVal <<= 8;
   lVal |= DOS->FGetC(fh);
   lVal <<= 8;
   lVal |= DOS->FGetC(fh);

   DOS->Close(fh);

   if (lVal != 0x3f2)
   {
      return false;
   }

   bState = false;

   for (;;) {
      lst = getnbits((unsigned char*)repository);
      if (!lst) break;
      {
         int l = (lst^0x3d)&0xf;
         int p;
         while (l--) {
            p = getnbits((unsigned char*)repository);
            if (p == -1) 
               return false;
         }
      }
      l = decompose(lst);

      if (l == -1) 
      {
         return false;
      }
      

      bState = false;
   
      k++;
   }
   bState = true;
   delete [] repository;
   return true;
}

