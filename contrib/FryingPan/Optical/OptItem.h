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

#ifndef _OPTITEM_H_
#define _OPTITEM_H_

#include <Generic/LibrarySpool.h>
#include "Optical.h"
#include "IOptItem.h"
#include <Generic/String.h>
#include <Generic/VectorT.h>
#include <Generic/Synchronizer.h>
#include <Generic/Debug.h>

/*
 * this file is for Optical's internal use only.
 */

using namespace GenNS;

class OptItem : public IOptItem
{
   Synchronizer      di_sync;
   uint32            di_opencnt;

   VectorT<IOptItem*>di_children;
   IOptItem         *di_parent;

   EItemType         di_itemtype;
   uint16            di_number;
   uint16            di_flags;         // DIF_xxx
   EDataType         di_datatype; 

   int32             di_start;         // first block
   uint16            di_pregap;        // automatically generated pregap
   uint16            di_postgap;       // automatically generated postgap
   int32            di_datasize;       // real data size (end = start + pregap + data + postgap - 1)

   uint32            di_actual;        // how many sectors have actually been written so far? (equals to "size" in most cases)
   uint16            di_sectorsize;    // size of sector

   bool              di_prot;          // digital protection
   bool              di_preemph;       //
   bool              di_incremental;   // packet / incrermental data track
   bool              di_blank;         // blank
   bool              di_cdtext;        // cdtext information available?
   bool              di_complete;      // disc complete
   
   uint16            di_packetlength;  // packet length (only when incremental set to 1)

   String            di_title;         // cdtext information (only when cd_text set to 1)
   String            di_artist;
   String            di_message;
   String            di_lyrics;
   String            di_composer;
   String            di_director;

   uint32            di_discid;        // disc, from which the track originates


protected:
   virtual                ~OptItem();
   virtual IOptItem       *addChild(IOptItem* child);

protected:
   static bool             sFreeChild(IOptItem* const& child);

public:
   /*
    * own methods go here
    */

                           OptItem(IOptItem *parent);
   void                    printReport(DbgHandler *dbg) const;

public:                                                       
   /*
    * inherited methods go here
    */

   virtual void            claim() const;
   virtual void            dispose() const;

   virtual EItemType       getItemType() const;
   virtual void            setItemType(EItemType lNewType);
   virtual uint16          getItemNumber() const;
   virtual void            setItemNumber(uint16 number);
   virtual EDataType       getDataType() const;
   virtual void            setDataType(EDataType lNewType);
   virtual uint16          getPreGapSize() const;
   virtual void            setPreGapSize(uint16 lNewSize);
   virtual uint16          getPostGapSize() const;
   virtual void            setPostGapSize(uint16 lNewSize);
   virtual int32           getStartAddress() const;
   virtual void            setStartAddress(int32 lNewAddress);
   virtual int32           getEndAddress() const;
   virtual void            setEndAddress(int32 lNewAddress);
   virtual int32           getBlockCount() const;
   virtual int32           getDataBlockCount() const;
   virtual void            setDataBlockCount(int32 count);
   virtual int32           getDataStartAddress() const;
   virtual int32           getDataEndAddress() const;

   virtual bool            isProtected() const;
   virtual void            setProtected(bool bProtected);
   virtual bool            isPreemphasized() const;
   virtual void            setPreemphasized(bool bPreemph);
   virtual bool            isIncremental() const;
   virtual void            setIncremental(bool bState);
   virtual bool            hasCDText() const;
   virtual void            setCDText(bool bCDText);

   virtual const char*     getCDTTitle() const;
   virtual void            setCDTTitle(const char* sNewTitle);
   virtual const char*     getCDTArtist() const;
   virtual void            setCDTArtist(const char* sNewArtist);
   virtual const char*     getCDTMessage() const;
   virtual void            setCDTMessage(const char* sNewMessage);
   virtual const char*     getCDTLyrics() const;
   virtual void            setCDTLyrics(const char* sNewLyrics);
   virtual const char*     getCDTComposer() const;
   virtual void            setCDTComposer(const char* sNewComposer);
   virtual const char*     getCDTDirector() const;
   virtual void            setCDTDirector(const char* sNewDirector);

   virtual const IOptItem* getParent() const;
   virtual const IOptItem* getChild(int32 num) const;
   virtual int32           getChildCount() const;

   virtual bool            isBlank() const;
   virtual void            setBlank(bool blank);

   virtual uint16          getSectorSize() const;
   virtual void            setSectorSize(uint16 size);

   virtual uint32          getDiscID() const;
   virtual void            setDiscID(uint32 id);
   
   virtual uint16          getPacketSize() const;
   virtual void            setPacketSize(uint16 lNewSize);
    
   virtual uint16          getFlags() const;
   virtual void            setFlags(uint16 lNewFlags);

   virtual bool            isComplete() const;
   virtual void            setComplete(bool complete);
};

#endif //_OPTITEM_H_
