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

#include "CDText.h"
#include <LibC/LibC.h>
#include <Generic/Debug.h>

CDText::CDText(const IOptItem* i) :
   crc(SumCRC::CRC_16_CCITT)
{
   currentBlock   = Blk_Unknown;
   cdText         = false;
   currentPackNo  = 0;
   currentOffset  = 0;
   currentTrack   = 0;
   currentByte    = 0;

   memset(&size, 0, sizeof(size));

   if (i->getItemType() != Item_Disc)
      return;

   if (i->getChildCount() == 0)
      return;

   cdText |= i->getChild(0)->hasCDText();

   addBlockData(Blk_Title, (uint8*)i->getChild(0)->getCDTTitle(), strlen(i->getChild(0)->getCDTTitle())+1);
   for (int tno=0; tno<i->getChild(0)->getChildCount(); tno++)
   {
      cdText |= i->getChild(0)->getChild(tno)->hasCDText();

      addBlockData(Blk_Title, 
            (uint8*)i->getChild(0)->getChild(tno)->getCDTTitle(), 
            strlen(i->getChild(0)->getChild(tno)->getCDTTitle())+1);
   }

   addBlockData(Blk_Performer, (uint8*)i->getChild(0)->getCDTArtist(), strlen(i->getChild(0)->getCDTArtist())+1);
   for (int tno=0; tno<i->getChild(0)->getChildCount(); tno++)
   {
      addBlockData(Blk_Performer, 
            (uint8*)i->getChild(0)->getChild(tno)->getCDTArtist(), 
            strlen(i->getChild(0)->getChild(tno)->getCDTArtist())+1);
   }

   addBlockData(Blk_SongWriter, (uint8*)i->getChild(0)->getCDTLyrics(), strlen(i->getChild(0)->getCDTLyrics())+1);
   for (int tno=0; tno<i->getChild(0)->getChildCount(); tno++)
   {
      addBlockData(Blk_SongWriter, 
            (uint8*)i->getChild(0)->getChild(tno)->getCDTLyrics(), 
            strlen(i->getChild(0)->getChild(tno)->getCDTLyrics())+1);
   }

   addBlockData(Blk_Composer, (uint8*)i->getChild(0)->getCDTComposer(), strlen(i->getChild(0)->getCDTComposer())+1);
   for (int tno=0; tno<i->getChild(0)->getChildCount(); tno++)
   {
      addBlockData(Blk_Composer, 
            (uint8*)i->getChild(0)->getChild(tno)->getCDTComposer(), 
            strlen(i->getChild(0)->getChild(tno)->getCDTComposer())+1);
   }

   addBlockData(Blk_Arranger, (uint8*)i->getChild(0)->getCDTDirector(), strlen(i->getChild(0)->getCDTDirector())+1);
   for (int tno=0; tno<i->getChild(0)->getChildCount(); tno++)
   {
      addBlockData(Blk_Arranger, 
            (uint8*)i->getChild(0)->getChild(tno)->getCDTDirector(), 
            strlen(i->getChild(0)->getChild(tno)->getCDTDirector())+1);
   }

   addBlockData(Blk_Message, (uint8*)i->getChild(0)->getCDTMessage(), strlen(i->getChild(0)->getCDTMessage())+1);
   for (int tno=0; tno<i->getChild(0)->getChildCount(); tno++)
   {
      addBlockData(Blk_Message, 
            (uint8*)i->getChild(0)->getChild(tno)->getCDTMessage(), 
            strlen(i->getChild(0)->getChild(tno)->getCDTMessage())+1);
   }

   closePack();

   /*
    * we want to build size info here. now some rules:
    * - the total size of cdtext block is what-we-got-so-far + 2 (= id of the last block).
    * - size info always goes last because it's easier to compute :P
    * - we fill in some table (initially filled with 0s) and push it to cdtext.
    * the resulting array can be written almost directly to cd. we need to make
    * the buffer n*96 bytes big and copy our data there (it's easy), then just tell drive
    * to generate the pad data for us. that's it %]
    */

   size.charCode = Chr_ISO8859_1;
   size.firstTrack = i->getChild(0)->getChild(0)->getItemNumber();
   size.lastTrack  = i->getChild(0)->getChild(-1)->getItemNumber();
   size.copyright  = 0;
   size.lastSeqNum[0] = currentPackNo+2;
   size.languages[0]  = Lng_English;
   addBlockData(Blk_Size, (uint8*)&size, sizeof(size));
}

CDText::~CDText()
{
}

void CDText::addBlockData(Block b, uint8 *data, int16 size)
{
   if (b != currentBlock)
   {
      closePack();
      currentTrack   = 0;
      currentByte    = 0;
      currentOffset  = 0;
      currentBlock   = b;
   } 

   ASSERT(currentOffset <= 12);

   if (currentOffset == 12)
   {
      currentOffset = 0;
   }

   if (currentOffset == 0)
   {
      currentPack.block        = b;
      currentPack.blockTrack   = currentTrack;
      currentPack.totalPack    = currentPackNo++;
      currentPack.charPos      = currentByte;
   }

   while ((currentOffset < 12) && (size--))
   {
      currentPack.data[currentOffset++] = *data++;
      if ((b >= Blk_Title) && (b <= Blk_Message))
         currentByte++;
   }

   if (currentOffset == 12)
   {
      closePack();
   }

   if (size > 0)
   {
      if (b >= Blk_DiscID)
         currentTrack++;
      addBlockData(b, data, size);
   }
   else
   {
      currentTrack++;
      currentByte = 0;
   }
}

void CDText::closePack()
{
   if (currentOffset == 0)
      return;

   while (currentOffset < 12)
      currentPack.data[currentOffset++] = 0;

   crc.Initialize();
   crc.Update((uint8*)&currentPack, 16);
   crc.Finalize();
   currentPack.crc16[0] = ~((crc.GetSum() & 0xff00) >> 8);
   currentPack.crc16[1] = ~(crc.GetSum() & 0xff);
   packs << currentPack;

   if (currentBlock == Blk_Size)
   {
      size.packsCount[Blk_Size - 0x80] = 3;
   }
   else
   {
      size.packsCount[currentBlock - 0x80]++;
   }

   currentOffset = 0;
}

void CDText::printReport(DbgHandler* __dbg)
{
   for (int i=0; i<packs.Count(); i++)
   {
      _D(Lvl_Info, "%08ld: %02lx %02lx %02lx %02lx: %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx | %02lx %02lx",
            i,
            packs[i].block,
            packs[i].blockTrack,
            packs[i].totalPack,
            packs[i].charPos,
            packs[i].data[0], packs[i].data[1], packs[i].data[2], packs[i].data[3], packs[i].data[4], packs[i].data[5],
            packs[i].data[6], packs[i].data[7], packs[i].data[8], packs[i].data[9], packs[i].data[10], packs[i].data[11],
            packs[i].crc16[0], packs[i].crc16[1]);
   }
}

bool CDText::hasCDText()
{
   return cdText;
}
   
bool CDText::getDataBuffer(uint8*& tgt, int32& size)
{
   int32    off = 0;
   uint8   *dat = 0;

   if (hasCDText() == false)
      return false;

   size = 24 * packs.Count() + 95;
   size = (size / 96)*96;

   tgt  = new uint8[size];
   for (int i=0; i<packs.Count(); i++)
   {
      dat = (uint8*)&packs[i];
      for (int j=0; j<6; j++)
      {
         tgt[off++] = (dat[3*j] >> 2) & 0x3f;
         tgt[off++] = ((dat[3*j]<< 4) | (dat[3*j+1] >> 4)) & 0x3f;
         tgt[off++] = ((dat[3*j+1]<< 2) | (dat[3*j+2] >> 6)) & 0x3f;
         tgt[off++] = (dat[3*j+2]) & 0x3f;
      }
   }

   while (off < size)
      tgt[off++] = 0;

   return true;
}

