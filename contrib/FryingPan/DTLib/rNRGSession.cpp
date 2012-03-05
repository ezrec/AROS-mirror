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

#include "rNRGSession.h"
#include <Generic/LibrarySpool.h>
#include <libclass/dos.h>
#include <Optical/IOptItem.h>
#include "Main.h"

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d)  ((((ULONG)(a)) << 24) | \
		           (((ULONG)(b)) << 16) | \
		           (((ULONG)(c)) <<  8) | \
		           (((ULONG)(d)) <<  0))
#endif

class file
{
public:
   enum filetype
   {
      Type_Unknown,
      Type_Intel,
      Type_Motorola,
      Type_Wave,
      Type_AIFF,
      Type_MP3
   };

protected:
   String            filename;
   BPTR              fileptr;

private:
   file() 
   {
      fileptr = 0;
      filename = "";
   };

   file(const file&) {};

public:

   static file *newFile(const String &filename)
   {
      BPTR           fileptr;
      file          *f;   

      fileptr = DOS->Open(filename.Data(), MODE_OLDFILE);
      if (fileptr == 0)
         return 0;

      f = new file;
      f->filename = filename;
      f->fileptr = fileptr;

      return f;
   }

   ~file()
   {
      if (fileptr != 0)
         DOS->Close(fileptr);
      fileptr = 0;
   }

   const String& getFileName() const
   {
      return filename;
   }

   void seek(uint32 offset) const
   {
      DOS->Seek(fileptr, offset, OFFSET_BEGINNING);
   }

   uint32 read(void* mem, uint32 len) const
   {
      return DOS->Read(fileptr, mem, len);
   }

   BPTR getBPTR() const
   {
      return fileptr;
   }
};

class index
{
   uint16            num;     // index number
   uint32            loc;     // boundary: start
   uint32            cnt;     // and count

   index()
   {
      num = 0;
      loc = 0;
      cnt = 0;
   }
public:

   static index *newIndex(uint16 number, uint32 location)
   {
      index *i = new index();

      i->num = (number >> 4) * 10 + (number & 15);
      i->loc = location;
      i->cnt = 0xffffffff;

      return i;
   }

   void setEndAddress(uint32 scnt)
   {
      cnt = scnt - loc;
   }

   uint32 getSecCnt() const
   {
      return cnt;
   }

   uint32 getLocation() const
   {
      return loc;
   }

   void setLocation(uint32 l)
   {
      loc = l;
   }

   uint32 getNumber() const
   {
      return num;
   }

   void fillItem(IOptItem* item)
   {
      item->setStartAddress(loc);
      item->setItemNumber(num);
      item->setDataBlockCount(cnt);
   }

   bool isValid() const
   {
      return (cnt != 0);
   }
};

class track
{
   EDataType         type;          // type of data
   uint32            flags;         // additional flags (like "RAW")
   uint16            secsize;       // resulting from the two above fields
   uint16            pregap;        // in sectors
   String            isrc;          // ISRC number
   int16             number;        // track number
   VectorT<index*>   indices;       // indices

   String            arranger;      // CDText
   String            composer;
   String            performer;
   String            message;
   String            title;
   String            songwriter;

   uint64            filepos;
   uint64            length;
   int64             currpos;

   file             *f;

   track() 
   {
      type = Data_Unknown;
      flags = 0;
      secsize = 2352;
      pregap = 0;
      number = 0;
   };  
 
public:
   static track* newTrack(file *f, int32 num)
   {
      track *nt = new track;
      nt->f = f;
      nt->number = (num >> 4) * 10 + (num & 15);
      return nt;
   }

   const file *getFile() const
   {
      return f;
   }

   void setType(EDataType dt)
   {
      type = dt;
   }

   void setSectorSize(uint16 size)
   {
      secsize = size;
   }

   void setDataStart(uint64 pos)
   {
      filepos = pos;
   }

   void setDataEnd(uint64 pos)
   {
      length = pos - filepos;
   }

   void setDataMode2(uint8 mode)
   {
   }

   EDataType getType() const
   {
      return type;
   }

   void debug(DbgHandler *__dbg)
   {
      if (__dbg == 0)
         return;

      _D(Lvl_Info, "Number:          %ld", number);
      _D(Lvl_Info, "File type:       %ld", type);
      _D(Lvl_Info, "Flags:           %ld", flags);
      _D(Lvl_Info, "Sector Size:     %ld", secsize);
      _D(Lvl_Info, "PreGap:          %ld", pregap);
      _D(Lvl_Info, "ISRC:            %s", (int32)isrc.Data());
      _D(Lvl_Info, "Arranger:        %s", (int32)arranger.Data());
      _D(Lvl_Info, "Composer:        %s", (int32)composer.Data());
      _D(Lvl_Info, "Performer:       %s", (int32)performer.Data());
      _D(Lvl_Info, "Message:         %s", (int32)message.Data());
      _D(Lvl_Info, "Title:           %s", (int32)title.Data());
      _D(Lvl_Info, "Song Writer:     %s", (int32)songwriter.Data());

      _D(Lvl_Info, "===");
   }

   void setArranger(const String &s)
   {
      arranger = s;
   }

   void setComposer(const String &s)
   {
      composer = s;
   }

   void setPerformer(const String &s)
   {
      performer = s;
   }

   void setMessage(const String &s)
   {
      message = s;
   }

   void setTitle(const String &s)
   {
      title = s;
   }

   void setSongWriter(const String &s)
   {
      songwriter = s;
   }

   void setPreGap(int32 gap)
   {
      pregap = gap;
   }

   int32 getPreGap() const
   {
      return pregap;
   }

   int32 getNumber() const
   {
      return number;
   }

   void setISRC(const String &s)
   {
      isrc = s;
   }

   void addIndex(index *i)
   {
      indices << i;
   }

   index *getIndex(int32 i)
   {
      return indices[i];
   }

   int32 getIndexCount()
   {
      return indices.Count();
   }

   int32 getSectorSize() const
   {
      return secsize;
   }

   void fillItem(IOptItem *item)
   {
      uint32 len = 0;

      item->setCDTDirector(arranger);
      item->setCDTComposer(composer);
      item->setCDTArtist(performer);
      item->setCDTMessage(message);
      item->setCDTTitle(title);
      item->setCDTLyrics(songwriter);
      item->setFlags(flags);
      item->setSectorSize(secsize);
      item->setPreGapSize(pregap);
      item->setItemNumber(number);
      item->setDataType(type);
      item->setSectorSize(secsize);
      
      item->setStartAddress(indices[0]->getLocation() - pregap);
      for (int32 i=0; i<indices.Count(); i++)
      {
         if (indices[i]->isValid() == false)
            continue;

         IOptItem *indx = item->addChild();
         
         indices[i]->fillItem(indx);
         len += indices[i]->getSecCnt();
      }
      item->setDataBlockCount(len);
   }

   void setUp()
   {
      currpos = -1;
   }

   uint32 read(void* &mem, uint32 len)
   {
      len *= secsize;

      if (currpos == -1)
      {
         f->seek(filepos);
         currpos = 0;
      }
      else if ((uint64)currpos >= length)
         return 0;

      len = len < (length - currpos) ? len : (length - currpos);
      f->read(mem, len);
      mem = &((char*)mem)[len];
      currpos += len;

      return len / secsize;
   }
};



struct nrg_etnf
{
   uint32   start;
   uint32   length;
   uint32   type;       // 0 = mode1, 1 = ?, 2 = mode2form1, 3 = mixed mode2
   uint32   secpos;     // without pregaps
   uint32   dummy;      // whatever.
};

struct nrg_etn2
{
   uint64   start;
   uint64   length;
   uint32   type;
   uint32   secpos;     // as above
   uint64   dummy;
};

struct nrg_cuex
{
   uint8    adr_ctl;    // 
   uint8    track;      // BCD track number
   uint8    index;      // index (bcd?)
   uint8    res;        // ???
   uint32   sector;     // ...
};

struct nrg_daoi
{
   uint32   dummy;
   uint8    mcn[14];
   uint8    disc_mode;
   uint8    one1;
   uint8    first_track;
   uint8    last_track;
   struct dao_track
   {
      uint8    pad[12];
      uint16   secsize;
      uint8    mode;
      uint8    zero1;
      uint8    zero2;
      uint8    type;
      uint32   pregap;
      uint32   offset;
      uint32   end;
   }        track[0];
};

struct nrg_daox
{
   uint32   dummy;
   uint8    mcn[14];
   uint8    disc_mode;
   uint8    one1;
   uint8    first_track;
   uint8    last_track;
   struct dao_track
   {
      uint8    pad[12];
      uint16   secsize;
      uint8    mode;
      uint8    zero1;
      uint8    zero2;
      uint8    type;
      uint64   pregap;        // pregap data start
      uint64   offset;        // normal data start
      uint64   end;           // normal data end
   }        track[0];
};


                     rNRGSession::rNRGSession(const char* sFileName, EDtError &rc)
{
   _createDebug(true, "rNRGSession");
   name = sFileName;
   f    = 0;
   analyse(sFileName, rc);
}

                     rNRGSession::~rNRGSession()
{
   tracks.ForEach(&freeTrack);

   if (f != 0)
      delete f;
   f = 0;

   _destroyDebug();
}
   
bool                 rNRGSession::freeTrack(track* const& t)
{
   delete t;
   return true;
}

IFileReader*         rNRGSession::openRead(const char* sFile, EDtError &rc)
{
   IFileReader *out = 0;

   checkFile(sFile, rc);
   if (rc == DT_OK)
   {
      out = new rNRGSession(sFile, rc);
      if (rc != DT_OK)
      {
         out->dispose();
         out = 0;
      }
   }
   return out;
}

const char*          rNRGSession::getName()
{
   return static_getName();
}

bool                 rNRGSession::readData(const IOptItem* item, void* mem, int len)
{
   _d(Lvl_Info, "%s: reading -> %08lx, %ld, curr trk: %ld; bptr=%lx", (int)__PRETTY_FUNCTION__, (int)mem, len, currtrk, (int)tracks[currtrk]->getFile()->getBPTR());

   if (item->getItemType() != Item_Track)
   {
      int32 ret;

      if (currtrk == tracks.Count())
         return false;

      ret = tracks[currtrk]->read(mem, len);
      if (ret == len)
         return true;

      currtrk++;
      ret += readData(item, mem, len-ret);

      return true;
   }
   else
   {
      ASSERTS(false, "Functionality not yet implemented, sorry.");
      return false;
   }
}

bool                 rNRGSession::setUp()
{
   _d(Lvl_Info, "%s: seeking all items to the start position...", (int)__PRETTY_FUNCTION__);

   for (int i=0; i<tracks.Count(); i++)
      tracks[i]->setUp();


   f->seek(0);

   currtrk = 0;

   return true;
}

bool                 rNRGSession::analyse(const char* name, EDtError &rc)
{
   BPTR fh;
   bool res=true;
   uint32 data;
   uint32 size;
   nrg_cuex *cue1 = 0;
   nrg_daoi *dao1 = 0;
   nrg_daox *dao2 = 0;

   rc = DT_UnableToOpenFile;

   fh = DOS->Open(name, MODE_OLDFILE);
   if (fh == 0)
      return false;
  
   f = file::newFile(name);

   rc = DT_OK;

   DOS->Seek(fh, -4, OFFSET_END);
   DOS->Read(fh, &data, 4);

   data = L2BE(data);
   DOS->Seek(fh, data, OFFSET_BEGINNING);

   do
   {
      DOS->Read(fh, &data, 4);
      DOS->Read(fh, &size, 4);
      data = L2BE(data);
      size = L2BE(size);
      if (data == MAKE_ID('C','D','T','X'))
      {
         _d(Lvl_Info, "%s: CDTX Tag", (int)__PRETTY_FUNCTION__);
         DOS->Seek(fh, size, OFFSET_CURRENT);
      }
      else if (data == MAKE_ID('C','U','E','X'))
      {
         _d(Lvl_Info, "%s: CUEX Tag", (int)__PRETTY_FUNCTION__);
         cue1 = (nrg_cuex*)new char[size];
         DOS->Read(fh, cue1, size);
      }
      else if (data == MAKE_ID('C','U','E','S'))
      {
         _d(Lvl_Info, "%s: CUES Tag", (int)__PRETTY_FUNCTION__);
         DOS->Seek(fh, size, OFFSET_CURRENT);
      }
      else if (data == MAKE_ID('D','A','O','X'))
      {
         _d(Lvl_Info, "%s: DAOX Tag", (int)__PRETTY_FUNCTION__);
         dao2 = (nrg_daox*)new char[size];
         DOS->Read(fh, dao2, size);
      }
      else if (data == MAKE_ID('D','A','O','I'))
      {
         _d(Lvl_Info, "%s: DAOI Tag", (int)__PRETTY_FUNCTION__);
         dao1 = (nrg_daoi*)new char[size];
         DOS->Read(fh, dao1, size);
         
         _d(Lvl_Info, "%s: DAOI: ftrk=%ld, ltrk=%ld", (int)__PRETTY_FUNCTION__, dao1->first_track, dao1->last_track);
         _d(Lvl_Info, "%s: DAOI: MCN=%02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx", (int)__PRETTY_FUNCTION__, dao1->mcn[0], dao1->mcn[1], dao1->mcn[2], dao1->mcn[3], dao1->mcn[4], dao1->mcn[5], dao1->mcn[6], dao1->mcn[7], dao1->mcn[8], dao1->mcn[9], dao1->mcn[10], dao1->mcn[11], dao1->mcn[12], dao1->mcn[13]);

         for (int i=0; i<=(dao1->last_track-dao1->first_track); i++)
         {
            _d(Lvl_Info, "%s: DAOI: track=%ld secsize=%ld mode=%ld type=%ld pregap=%lx data=%lx end=%lx", 
                  (int)__PRETTY_FUNCTION__, 
                  i+dao1->first_track,
                  dao1->track[i].secsize,
                  dao1->track[i].mode,
                  dao1->track[i].type,
                  dao1->track[i].pregap,
                  dao1->track[i].offset,
                  dao1->track[i].end);
         }
      }
      else if (data == MAKE_ID('E','T','N','F'))
      {
         _d(Lvl_Info, "%s: ETNF Tag", (int)__PRETTY_FUNCTION__);
         int32 num = (size / sizeof(nrg_etnf));
         nrg_etnf *etnf = (nrg_etnf*)new char[size];
         DOS->Read(fh, etnf, size);
         
         for (int i=0; i<num; i++)
         {
            _d(Lvl_Info, "%s: ETNF: start=%ld length=%ld type=%ld sector=%ld other=%lx", (int)__PRETTY_FUNCTION__, etnf[i].start, etnf[i].length, etnf[i].type, etnf[i].secpos, etnf[i].dummy);
         }
         
         delete etnf;
      }
      else if (data == MAKE_ID('E','T','N','2'))
      {
         _d(Lvl_Info, "%s: ETN2 Tag", (int)__PRETTY_FUNCTION__);
         int32 num = (size / sizeof(nrg_etn2));
         nrg_etn2 *etnf = (nrg_etn2*)new char[size];
         DOS->Read(fh, etnf, size);
         
         for (int i=0; i<num; i++)
         {
            _d(Lvl_Info, "%s: ETN2: start=%ld length=%ld type=%ld sector=%ld other=%lx", (int)__PRETTY_FUNCTION__, etnf[i].start, etnf[i].length, etnf[i].type, etnf[i].secpos, etnf[i].dummy);
         }

         delete etnf;
      }
      else if (data == MAKE_ID('N','E','R','O'))
      {
         _d(Lvl_Info, "%s: NERO Tag", (int)__PRETTY_FUNCTION__);
         DOS->Seek(fh, size, OFFSET_CURRENT);
      }
      else if (data == MAKE_ID('N','E','R','5'))
      {
         _d(Lvl_Info, "%s: NER5 Tag", (int)__PRETTY_FUNCTION__);
         DOS->Seek(fh, size, OFFSET_CURRENT);
      }
      else if (data == MAKE_ID('S','I','N','F'))
      {
         _d(Lvl_Info, "%s: SINF Tag", (int)__PRETTY_FUNCTION__);
         DOS->Seek(fh, size, OFFSET_CURRENT);
      }
      else if (data == MAKE_ID('M','T','Y','P'))
      {
         _d(Lvl_Info, "%s: MTYP Tag", (int)__PRETTY_FUNCTION__);
         DOS->Seek(fh, size, OFFSET_CURRENT);
      }
      else if (data == MAKE_ID('E','N','D','!'))
      {
         _d(Lvl_Info, "%s: END! Tag", (int)__PRETTY_FUNCTION__);
         break;
      }
         
   }
   while (true);

   if (res == true)
      res = buildTrackList(cue1, rc); 
   if ((dao2 != 0) && (res == true))
      res = updateTrackData(dao2, rc);
   if ((dao1 != 0) && (res == true))
      res = updateTrackData(dao2, rc);

   if (0 != cue1)
      delete cue1;
   if (0 != dao1)
      delete dao1;
   if (0 != dao2)
      delete dao2;

   DOS->Close(fh);
   return res;
}

bool                 rNRGSession::buildTrackList(nrg_cuex *cue, EDtError &rc)
{
   if (cue == 0)
   {
      rc = DT_FileFormatNotSupported;
      return false;
   }

   for (int i=0;; i++)
   {
      track *t = 0;
      index *x = 0;

      _d(Lvl_Info, "%s: CUEX: type=%02lx track=%02lx adrctl=%02lx res=%02lx sector=%08lx", (int)__PRETTY_FUNCTION__, cue[i].adr_ctl, cue[i].track, cue[i].index, cue[i].res, cue[i].sector);

      if (cue[i].track == 0)
         continue;
         
      if (tracks.Count() > 0)
         tracks[-1]->getIndex(-1)->setEndAddress(cue[i].sector);
      
      if (cue[i].track == 0xaa)
         break;

      for (int j=0; j<tracks.Count(); j++)
      {
         if (tracks[j]->getNumber() == ((cue[i].track >> 4) * 10 + (cue[i].track & 15)))
         {
            t = tracks[j];
            break;
         }
      }

      if (t == 0)
      {
         t = track::newTrack(f, cue[i].track);
         tracks << t;

         switch (cue[i].adr_ctl & 0xc0)
         {
            case 0x40:
               t->setType(Data_Mode1);
               break;
            case 0x00:
               t->setType(Data_Audio);
               break;
            default:
               rc = DT_FileFormatNotSupported;
               return false;
         }
      }

      x = index::newIndex(cue[i].index, cue[i].sector);
      t->addIndex(x);

   }

   return true;
}
   
bool                 rNRGSession::updateTrackData(struct nrg_daox* dao, EDtError &rc)
{
   if (dao == 0)
   {
      rc = DT_FileFormatNotSupported;
      return false;
   }

   _d(Lvl_Info, "%s: DAO: ftrk=%ld, ltrk=%ld", (int)__PRETTY_FUNCTION__, dao->first_track, dao->last_track);
   _d(Lvl_Info, "%s: DAO: MCN=%02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx", (int)__PRETTY_FUNCTION__, dao->mcn[0], dao->mcn[1], dao->mcn[2], dao->mcn[3], dao->mcn[4], dao->mcn[5], dao->mcn[6], dao->mcn[7], dao->mcn[8], dao->mcn[9], dao->mcn[10], dao->mcn[11], dao->mcn[12], dao->mcn[13]);

   for (int i=0; i<=(dao->last_track-dao->first_track); i++)
   {
      track *t = 0;

      _d(Lvl_Info, "%s: DAO: track=%ld secsize=%ld mode=%ld type=%ld pregap=%lx data=%lx end=%lx", 
            (int)__PRETTY_FUNCTION__, 
            i+dao->first_track,
            dao->track[i].secsize,
            dao->track[i].mode,
            dao->track[i].type,
            dao->track[i].pregap,
            dao->track[i].offset,
            dao->track[i].end);

      for (int j=0; j<tracks.Count(); j++)
      {
         if (tracks[j]->getNumber() == (i + dao->first_track))
         {
            t = tracks[j];
            break;
         }
      }

      if (t == 0)
      {
         rc = DT_FileMalformed;
         return false;
      }

      t->setSectorSize(dao->track[i].secsize);
      switch (dao->track[i].mode)
      {
         case 0:
            t->setType(Data_Mode1);
            break;
         case 1:
            t->setType(Data_Mode2Form1);
            break;
         case 2:
            t->setType(Data_Mode2Form2);
            break;
         case 3:
         case 6:
            t->setType(Data_Mode2);
            break;
         case 7:
            t->setType(Data_Audio);
            break;
         default:
            rc = DT_FileFormatNotSupported;
            return false;
      }
      t->setDataStart(dao->track[i].pregap);
      t->setDataEnd(dao->track[i].end);
      t->setDataMode2(dao->track[i].mode);

      _d(Lvl_Info, "%s: Track %ld: %ld - %ld, mode %ld", (int)__PRETTY_FUNCTION__, i, dao->track[i].pregap, dao->track[i].end, dao->track[i].mode);
   }
   return true;
}

bool                 rNRGSession::updateTrackData(struct nrg_daoi* dao, EDtError &rc)
{
   if (dao == 0)
   {
      rc = DT_FileFormatNotSupported;
      return false;
   }

   _d(Lvl_Info, "%s: DAO: ftrk=%ld, ltrk=%ld", (int)__PRETTY_FUNCTION__, dao->first_track, dao->last_track);
   _d(Lvl_Info, "%s: DAO: MCN=%02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx", (int)__PRETTY_FUNCTION__, dao->mcn[0], dao->mcn[1], dao->mcn[2], dao->mcn[3], dao->mcn[4], dao->mcn[5], dao->mcn[6], dao->mcn[7], dao->mcn[8], dao->mcn[9], dao->mcn[10], dao->mcn[11], dao->mcn[12], dao->mcn[13]);

   for (int i=0; i<=(dao->last_track-dao->first_track); i++)
   {
      track *t = 0;

      _d(Lvl_Info, "%s: DAO: track=%ld secsize=%ld mode=%ld type=%ld pregap=%lx data=%lx end=%lx", 
            (int)__PRETTY_FUNCTION__, 
            i+dao->first_track,
            dao->track[i].secsize,
            dao->track[i].mode,
            dao->track[i].type,
            dao->track[i].pregap,
            dao->track[i].offset,
            dao->track[i].end);

      for (int j=0; j<tracks.Count(); j++)
      {
         if (tracks[j]->getNumber() == (i + dao->first_track))
         {
            t = tracks[j];
            break;
         }
      }

      if (t == 0)
      {
         rc = DT_FileMalformed;
         return false;
      }

      t->setSectorSize(dao->track[i].secsize);
      switch (dao->track[i].mode)
      {
         case 0:
            t->setType(Data_Mode1);
            break;
         case 1:
            t->setType(Data_Mode2Form1);
            break;
         case 2:
            t->setType(Data_Mode2Form2);
            break;
         case 3:
         case 6:
            t->setType(Data_Mode2);
            break;
         case 7:
            t->setType(Data_Audio);
            break;
         default:
            rc = DT_FileFormatNotSupported;
            return false;
      }
      t->setDataStart(dao->track[i].pregap);
      t->setDataEnd(dao->track[i].end);
      t->setDataMode2(dao->track[i].mode);
   }
   return true;
}

void                 rNRGSession::cleanUp()
{
}

void                 rNRGSession::dispose()
{
   delete this;
}

bool                 rNRGSession::isAudio()
{
   return static_isAudio();
}

bool                 rNRGSession::isData()
{
   return static_isData();
}

bool                 rNRGSession::fillOptItem(IOptItem *disc)
{

   if (disc->getItemType() != Item_Disc)
      return false;

   disc->setFlags(DIF_Disc_MasterizeCD | DIF_Disc_CloseDisc);
   IOptItem *sess = disc->addChild();

   sess->setCDTDirector(arranger);
   sess->setCDTComposer(composer);
   sess->setCDTArtist(performer);
   sess->setCDTMessage(message);
   sess->setCDTTitle(title);
   sess->setCDTLyrics(songwriter);

   for (int i=0; i<tracks.Count(); i++)
   {
      IOptItem *trak = sess->addChild();
      tracks[i]->fillItem(trak);
   }
   return true;
}

const char          *rNRGSession::getTrackName()
{
   return static_getName();
}

uint32               rNRGSession::getBlockCount()
{
   return blockCount;
}

uint16               rNRGSession::getBlockSize()
{
   return blockSize;
}

const char          *rNRGSession::static_getName()
{
   return "Nero Burning ROM Image";
}

bool                 rNRGSession::static_isAudio()
{
   return false;
}

bool                 rNRGSession::static_isData()
{
   return false;
}

bool                 rNRGSession::static_isSession()
{
   return true;
}

bool                 rNRGSession::checkFile(const char* file, EDtError &rc)
{
   BPTR   fh;
   uint32 data;
   uint32 size;
   int    res = true;

   rc = DT_UnableToOpenFile;

   fh = DOS->Open(const_cast<char*>(file), MODE_OLDFILE);
   if (0 == fh)
      return false;

   rc = DT_OK;

   DOS->Seek(fh, -4, OFFSET_END);
   size = DOS->Seek(fh, 0, OFFSET_CURRENT) + 4;    // fancy, innit

   DOS->Read(fh, &data, 4);
   data = L2BE(data);
   if ((size - data) > 4096)
      return false;

   DOS->Seek(fh, data, OFFSET_BEGINNING);

   do
   {
      DOS->Read(fh, &data, 4);
      data = L2BE(data);
      if ((data == MAKE_ID('C','D','T','X')) ||
          (data == MAKE_ID('C','U','E','X')) ||
          (data == MAKE_ID('C','U','E','S')) ||
          (data == MAKE_ID('D','A','O','X')) ||
          (data == MAKE_ID('D','A','O','I')) ||
          (data == MAKE_ID('E','T','N','F')) ||
          (data == MAKE_ID('E','T','N','2')) ||
          (data == MAKE_ID('N','E','R','O')) ||
          (data == MAKE_ID('N','E','R','5')) ||
          (data == MAKE_ID('S','I','N','F')) ||
          (data == MAKE_ID('M','T','Y','P')))
      {
         DOS->Read(fh, &data, 4);
         data = L2BE(data);
         DOS->Seek(fh, data, OFFSET_CURRENT);
      }
      else if (data == MAKE_ID('E','N','D','!'))
      {
         break;
      }
      else
      {
         res = false;
         break;
      }
   }
   while (true);

   DOS->Close(fh);
   return res;
}

DbgHandler*          rNRGSession::getDebug()
{
   return dbg;
}

void                 rNRGSession::setDebug(DbgHandler* nd)
{
   dbg = nd;
}

