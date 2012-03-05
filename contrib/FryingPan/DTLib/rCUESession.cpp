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

#include "rCUESession.h"
#include <Generic/LibrarySpool.h>
#include <libclass/dos.h>
#include <Optical/IOptItem.h>
#include "Main.h"


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
   uint32            length;
   filetype          type;
   BPTR              fileptr;

private:
   file() 
   {
      fileptr = 0;
      type = Type_Unknown;
      length = 0;
      filename = "";
   };

   file(const file&) {};

public:

   static file *newFile(const String &cue_location, const String &filename, const String &type)
   {
      BPTR           fileptr;
      String         path;
      FileInfoBlock *fib;
      file          *f;   

      path = cue_location;
      path.AddPath(filename);

      fileptr = DOS->Open(path.Data(), MODE_OLDFILE);
      if (fileptr == 0)
         return 0;

      f = new file;
      f->filename = path;
      f->fileptr = fileptr;

      fib = new FileInfoBlock;
      DOS->ExamineFH(fileptr, fib);
      f->length = fib->fib_Size;
      delete fib;

      if (type == "BINARY")
         f->type = Type_Intel;
      else if (type == "MOTOROLA")
         f->type = Type_Motorola;
      else if (type == "AIFF")
         f->type = Type_AIFF;
      else if (type == "WAVE")
         f->type = Type_Wave;
      else if (type == "MP3")
         f->type = Type_MP3;
      else
         f->type = Type_Unknown;

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

   uint32 getFileSize() const
   {
      return length;
   }

   file::filetype getFileType() const
   {
      return type;
   }

   BPTR getFile() const
   {
      return fileptr;
   }

   void seek(uint32 offset) const
   {
      DOS->Seek(fileptr, offset, OFFSET_BEGINNING);
   }

   uint32 read(void* mem, uint32 len) const
   {
      return DOS->Read(fileptr, mem, len);
   }
};

class index
{
   uint16            num;     // index number
   uint32            loc;     // boundary: start
   uint32            cnt;     // and count

   file*             f;
   uint32            fileoffset;
   uint32            datalength;
   uint32            currpos;


   index()
   {
      num = 0;
      loc = 0;
      cnt = 0;
      currpos = 0;
   }
public:

   static index *newIndex(uint16 number, uint32 location)
   {
      index *i = new index();

      i->num = number;
      i->loc = location;
      i->cnt = 0xffffffff;

      return i;
   }

   void setSecCnt(uint32 scnt)
   {
      cnt = scnt;
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

    void debug(DbgHandler *__dbg)
   {
      if (__dbg == 0)
         return;

      _D(Lvl_Info, "---");
      _D(Lvl_Info, "Number:          %ld", num);
      _D(Lvl_Info, "Location:        %ld", loc);
      _D(Lvl_Info, "Sector Count:    %ld", cnt);
   }

   void setFile(file* fl)
   {
      f = fl;
   }

   file *getFile() const
   {
      return f;
   }

   void setFileOffset(uint32 fo)
   {
      fileoffset = fo;
   }

   void setDataLength(uint32 dl)
   {
      datalength = dl;
   }

   uint32 getFileOffset() const
   {
      return fileoffset;
   }

   uint32 getDataLength() const
   {
      return datalength;
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

   void setUp()
   {
      currpos = 0xffffffff;
   }

   uint32 read(void* mem, uint32 len)
   {
      if (currpos == datalength)
         return 0;
      if (currpos == 0xffffffff)
      {
         f->seek(fileoffset);
         currpos = 0;
      }

      len = len < (datalength - currpos) ? len : (datalength - currpos);
      f->read(mem, len);                  // i don't care if data is valid
      currpos += len;                     // if it's not, i won't interrupt the process
      return len;
   }
};

class track
{
   EDataType         type;          // type of data
   uint32            flags;         // additional flags (like "RAW")
   uint16            secsize;       // resulting from the two above fields
   uint16            pregap;        // in sectors
   uint16            postgap;       // in sectors
   String            isrc;          // ISRC number
   int16             number;        // track number
   VectorT<index*>   indices;       // indices

   int16             curridx;

   String            arranger;      // CDText
   String            composer;
   String            performer;
   String            message;
   String            title;
   String            songwriter;

   track() 
   {
      type = Data_Unknown;
      flags = 0;
      secsize = 0;
      pregap = 0;
      postgap= 0;
      number = 0;
      curridx = 0;
   };  
 
public:
   static track* newTrack(int32 num, const String &mode)
   {
      track *nt = new track;

      nt->number = num;

      if (mode == "AUDIO")
         nt->type = Data_Audio, nt->flags = 0, nt->secsize = 2352;
      else if (mode == "MDOE1/2048")
         nt->type = Data_Mode1, nt->flags = 0, nt->secsize = 2048;
      else if (mode == "MODE2/2336")
         nt->type = Data_Mode2, nt->flags = 0, nt->secsize = 2336;
      else if (mode == "MODE1/2352")
         nt->type = Data_Mode1, nt->flags = DIF_Common_RawData, nt->secsize = 2352;
      else if (mode == "MODE2/2352")
         nt->type = Data_Mode2, nt->flags = DIF_Common_RawData, nt->secsize = 2352;
      else
      {
         delete nt;
         return 0;
      }

      return nt;
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
      _D(Lvl_Info, "PostGap:         %ld", postgap);
      _D(Lvl_Info, "ISRC:            %s", (int32)isrc.Data());
      _D(Lvl_Info, "Arranger:        %s", (int32)arranger.Data());
      _D(Lvl_Info, "Composer:        %s", (int32)composer.Data());
      _D(Lvl_Info, "Performer:       %s", (int32)performer.Data());
      _D(Lvl_Info, "Message:         %s", (int32)message.Data());
      _D(Lvl_Info, "Title:           %s", (int32)title.Data());
      _D(Lvl_Info, "Song Writer:     %s", (int32)songwriter.Data());

      for (int32 i=0; i<indices.Count(); i++)
         indices[i]->debug(__dbg);
      
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

   void setPostGap(int32 gap)
   {
      postgap = gap;
   }

   int32 getPreGap() const
   {
      return pregap;
   }

   int32 getPostGap() const
   {
      return postgap;
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
      
      item->setStartAddress(indices[0]->getLocation() - pregap);
      for (int32 i=0; i<indices.Count(); i++)
      {
         IOptItem *indx = item->addChild();
         
         indices[i]->fillItem(indx);
         len += indices[i]->getSecCnt();
      }
      item->setDataBlockCount(len);

// FIXME: no postgap
                  
      // uint16            postgap;       // in sectors
      // String            isrc;          // ISRC number
   }

   void setUp()
   {
      for (int i=0; i<indices.Count(); i++)
         indices[i]->setUp();

      curridx = 0;
   }

   uint32 read(void* &mem, uint32 len)
   {
      uint32 ret = 0;

      len *= secsize;

      while (curridx < indices.Count())
      {
         ret += indices[curridx]->read(mem, len - ret);

         mem = &((char*)mem)[ret];
      
         if (ret == len)
            break;

         curridx++;
      }

      return ret / secsize;
   }
};



// TODO: 
// - use 'cdtextfile' parameter some day
// - apply params such as 'isrc', 'upc_ean', 'disc_id' 
// - make class track return the data to the reader!!!!



                     rCUESession::rCUESession(const char* sFileName, EDtError &rc)
{
   _createDebug(true, "rCUESession");
   name = sFileName;
   analyse(sFileName, rc);
}

                     rCUESession::~rCUESession()
{
   tracks.ForEach(&freeTrack);
   for (int i=0; i<files.Count(); i++)
      delete files[i];
   files.Empty();


   _destroyDebug();
}
   
bool                 rCUESession::freeTrack(track* const& t)
{
   delete t;
   return true;
}

IFileReader*         rCUESession::openRead(const char* sFile, EDtError &rc)
{
   IFileReader *out = 0;

   checkFile(sFile, rc);
   if (rc == DT_OK)
   {
      out = new rCUESession(sFile, rc);
      if (rc != DT_OK)
      {
         out->dispose();
         out = 0;
      }
   }
   return out;
}

const char*          rCUESession::getName()
{
   return static_getName();
}

bool                 rCUESession::readData(const IOptItem* item, void* mem, int len)
{
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

bool                 rCUESession::setUp()
{
   _d(Lvl_Info, "%s: seeking all items to the start position...", (int)__PRETTY_FUNCTION__);

   for (int i=0; i<tracks.Count(); i++)
      tracks[i]->setUp();

   for (int i=0; i<files.Count(); i++)
      DOS->Seek(files[i]->getFile(), 0, OFFSET_BEGINNING);

   currtrk = 0;

   return true;
}

bool                 rCUESession::analyse(const char* name, EDtError &rc)
{
   BPTR              fh;
   uint8             s[1024];
   String            path;
   bool              res = true;
   String            fileName;
   track            *nt = 0;           // new track
   index            *idx = 0;

   rc = DT_UnableToOpenFile;

   _d(Lvl_Info, "%s: starting analysis. File name: %s", (int)__PRETTY_FUNCTION__, (int)name);

   fh = DOS->Open(const_cast<char*>(name), MODE_OLDFILE);
   if (0 == fh)
      return false;

   BPTR lock = DOS->ParentOfFH(fh);
   DOS->NameFromLock(lock, (char*)s, sizeof(s));
   path = (char*)s;

   _d(Lvl_Info, "%s: current path for binaries: %s", (int)__PRETTY_FUNCTION__, (int)path.Data());

   rc = DT_OK;

   while (DOS->FGets(fh, (char*)s, sizeof(s)) != 0)
   {
      for (int i=0; s[i] != 0; i++)
      {
         if ((s[i] != 10) && (s[i] != 13) && (s[i] != '\t') && (s[i]<' '))
         {
            DOS->Close(fh);
            return false;
         }
         if ((s[i] >=128) && (s[i]<160))
         {
            DOS->Close(fh);
            return false;
         }
      }

      String str((char*)s);
      VectorT<String> v = str.Explode();
   
      if (v.Count() > 0)
      {
         str = v[0].UpperCase();
       
         if (str == "TRACK")
         {
            int32 tn;

            _d(Lvl_Info, "%s: parsing track", (int)__PRETTY_FUNCTION__);
            if (v[1][0] == '0')
               tn = v[1].SubString(1, -1).ToLong();
            else
               tn = v[1].ToLong();
            
            nt = track::newTrack(tn, v[2]);

            if (nt == 0)
            {
               _d(Lvl_Info, "%s: failed to create track", (int)__PRETTY_FUNCTION__);
               rc = DT_FileMalformed;
               res = false;
               break;
            }

            tracks << nt;
         }
         else if (str == "INDEX")
         {
            int32 tn;

            _d(Lvl_Info, "%s: parsing index", (int)__PRETTY_FUNCTION__);
            if ((files.Count() == 0) || (nt == 0))
            {
               _d(Lvl_Info, "%s: files or tracks not defined yet", (int)__PRETTY_FUNCTION__);
               rc = DT_FileMalformed;
               res = false;
               break;
            }

            uint32 pos = parseLocation(v[2]);
            if (idx != 0)
               idx->setSecCnt(pos-idx->getLocation());
 
            if (v[1][0] == '0')
               tn = v[1].SubString(1, -1).ToLong();
            else
               tn = v[1].ToLong();
            
           idx = index::newIndex(tn, pos);
            nt->addIndex(idx);
         }
         else if (str == "FILE")
         {
            file *f;
            _d(Lvl_Info, "%s: parsing file", (int)__PRETTY_FUNCTION__);
   
            f = file::newFile(path, v[1], v[2]);
            if (f == 0)
            {
               _d(Lvl_Info, "%s: failed to create file entry for %s", (int)__PRETTY_FUNCTION__, (int)v[1].Data());
               rc = DT_UnableToOpenFile;
               res = false;
               break;
            }

            files << f;
            idx = 0;
            
            if (f->getFileType() != file::Type_Intel)
            {
               _d(Lvl_Warning, "%s: Unsupported file format: %s", (int)__PRETTY_FUNCTION__, (int)v[2].Data());
               rc = DT_FileFormatNotSupported;
               res = false;
               break;
            }
         }
         else if (str == "FLAGS")
         {
            _d(Lvl_Info, "%s: flags are not supported at the moment.", (int)__PRETTY_FUNCTION__);
            rc = DT_FileFormatNotSupported;
            res = false;
            break;
         }
         else if (str == "REM")
         {
            _d(Lvl_Info, "%s: %s", (int)__PRETTY_FUNCTION__, (int)str.Data());
         }
         else if (str == "CATALOG")
         {
            _d(Lvl_Info, "%s: parsing catalog", (int)__PRETTY_FUNCTION__);
            catalog = v[1].ToQuad();
         }
         else if (str == "CDTEXTFILE")
         {
            _d(Lvl_Warning, "%s: CDTEXTFILE TAG IS NOT SUPPORTED!", (int)__PRETTY_FUNCTION__);
         }
         else if (str == "ISRC")
         {
            _d(Lvl_Info, "%s: reading ISRC", (int)__PRETTY_FUNCTION__);
            if (0 == nt)
            {
               rc = DT_FileMalformed;
               res = false;
               break;
            }
            nt->setISRC(v[1]);
         }
         else if (str == "PREGAP")
         {
            _d(Lvl_Info, "%s: parsing pregap", (int)__PRETTY_FUNCTION__);
            if (0 == nt)
            {
               rc = DT_FileMalformed;
               res = false;
               break;
            }
            nt->setPreGap(v[1].ToLong());
            _d(Lvl_Info, "%s: pregap size: %ld", (int)__PRETTY_FUNCTION__, v[1].ToLong());
         }
         else if (str == "POSTGAP")
         {
            _d(Lvl_Warning, "%s: postgaps not supported", (int)__PRETTY_FUNCTION__);
            rc = DT_FileFormatNotSupported;
            res = false;
            break;
         }
         else if (str == "ARRANGER")
         {
            _d(Lvl_Info, "%s: read arranger: %s", (int)__PRETTY_FUNCTION__, (int)v[1].Data());
            if (0 != nt)
               nt->setArranger(v[1]);
            else
               arranger = v[1];
         }
         else if (str == "COMPOSER")
         {
            _d(Lvl_Info, "%s: read composer: %s", (int)__PRETTY_FUNCTION__, (int)v[1].Data());
            if (0 != nt)
               nt->setComposer(v[1]);
            else
               composer = v[1];
         }
         else if (str == "DISC_ID")
         {
            _d(Lvl_Warning, "%s: ignoring disc id", (int)__PRETTY_FUNCTION__);
            disc_id = v[1];
         }
         else if (str == "GENRE")
         {
            _d(Lvl_Warning, "%s: ignoring genre", (int)__PRETTY_FUNCTION__);

            // safe to skip this one.
         }
         else if (str == "MESSAGE")
         {
            _d(Lvl_Info, "%s: read message: %s", (int)__PRETTY_FUNCTION__, (int)v[1].Data());
            if (0 != nt)
               nt->setMessage(v[1]);
            else
               message = v[1];
         }
         else if (str == "PERFORMER")
         {
            _d(Lvl_Info, "%s: read performer: %s", (int)__PRETTY_FUNCTION__, (int)v[1].Data());
            if (0 != nt)
               nt->setPerformer(v[1]);
            else
               performer = v[1];
         }
         else if (str == "SONGWRITER")
         {
            _d(Lvl_Info, "%s: read songwriter: %s", (int)__PRETTY_FUNCTION__, (int)v[1].Data());
            if (0 != nt)
               nt->setSongWriter(v[1]);
            else
               songwriter = v[1];
         }
         else if (str == "TITLE")
         {
            _d(Lvl_Info, "%s: read title: %s", (int)__PRETTY_FUNCTION__, (int)v[1].Data());
            if (0 != nt)
               nt->setTitle(v[1]);
            else
               title = v[1];
         }
         else if (str == "TOC_INFO")
         {
            _d(Lvl_Warning, "%s: TOC INFO not supported", (int)__PRETTY_FUNCTION__);
            rc = DT_FileFormatNotSupported;
            res = false;
            break;
         }
         else if (str == "TOC_INFO2")
         {
            _d(Lvl_Warning, "%s: TOC INFO not supported", (int)__PRETTY_FUNCTION__);
            rc = DT_FileFormatNotSupported;
            res = false;
            break;
         }
         else if (str == "UPC_EAN")
         {
            _d(Lvl_Warning, "%s: ignoring UPC", (int)__PRETTY_FUNCTION__);
            upc_ean = v[1];
         }
         else if (str == "SIZE_INFO")
         {
            _d(Lvl_Warning, "%s: alien SIZE INFO not supported", (int)__PRETTY_FUNCTION__);
            rc = DT_FileFormatNotSupported;
            res = false;
            break;
         }
         else
         {
            _d(Lvl_Error, "%s: Malformed tag %s. Aborting.", (int)__PRETTY_FUNCTION__, (int)v[0].Data());
            rc = DT_FileMalformed;
            res = false;
            break;
         }
      }

   }

   if (tracks.Count() == 0)
   {
      rc = DT_FileMalformed;
      res = false;
   }

   for (int32 i=0; i<tracks.Count(); i++)
   {
      if (tracks[i]->getIndexCount() == 0)
      {
         res = false;
         rc = DT_FileMalformed;
         break;
      }
   }

   _d(Lvl_Info, "%s: fixing references...", (int)__PRETTY_FUNCTION__);

   if (res == true)
   {
      uint32 len_so_far = 0;
      int32 sec_so_far = -150;
      int32 fno = 0;

      if ((tracks[0]->getIndex(0)->getNumber() != 0) && (tracks[0]->getPreGap() < 150))
         tracks[0]->setPreGap(150);

      for (int32 i=0; i<tracks.Count(); i++)
      {
         _d(Lvl_Info, "%s: Advancing to track %ld", (int)__PRETTY_FUNCTION__, i+1);
         track *t = tracks[i];

         sec_so_far += t->getPreGap();

         for (int32 j=0; j<t->getIndexCount(); j++)
         {
            _d(Lvl_Info, "%s: Checking index %ld", (int)__PRETTY_FUNCTION__, j+1);
            index *x = t->getIndex(j);

            if ((x->getNumber() == 0) && (t->getPreGap() != 0))
            {
               _d(Lvl_Warning, "%s: CUE specifies both pregap and index 0, it's a nonsense.", (int32)__PRETTY_FUNCTION__);
               rc = DT_FileMalformed;
               res = false;
               break;
            }

            x->setLocation(sec_so_far);
            x->setFile(files[fno]);
            x->setFileOffset(len_so_far);

            if (x->getSecCnt() == 0xffffffff)
            {
               _d(Lvl_Info, "%s: Altering index accordingly to track %ld", (int)__PRETTY_FUNCTION__, fno+1);
               x->setSecCnt((files[fno]->getFileSize() - len_so_far) / t->getSectorSize());
               x->setDataLength(x->getSecCnt() * t->getSectorSize());

               fno++;
               len_so_far = 0;
            }
            else
            {
               x->setDataLength(x->getSecCnt() * t->getSectorSize());
               len_so_far += x->getSecCnt() * t->getSectorSize();
            }

            sec_so_far += x->getSecCnt();
         }

         sec_so_far += t->getPostGap();
      
         if (res == false)
            break;
      }
   }

   for (int32 i=0; i<tracks.Count(); i++)
      tracks[i]->debug(getDebug());

   DOS->Close(fh);
   return res;
}

uint32               rCUESession::parseLocation(String &s)
{
   /*
    * NOTE:
    * despite MSF holds LBA+150 in normal case,
    * we are actually referring to data stored in a file
    * thus there is no decrement!
    */

   String c = s;
   int i = 0;
   uint32 loc = 0;
   uint32 tot = 0;

   while ((c[i] != ':') && (i < c.Length()))
   {
      loc = 10 * loc + c[i]-'0';
      i++;
   }

   tot = loc * 60;
   loc = 0;
   i++;

   while ((c[i] != ':') && (i < c.Length()))
   {
      loc = 10 * loc + c[i]-'0';
      i++;
   }

   tot = (tot + loc) * 75;
   loc = 0;
   i++;

   while ((c[i] != ':') && (i < c.Length()))
   {
      loc = 10 * loc + c[i]-'0';
      i++;
   }

   tot  = tot + loc;

   _d(Lvl_Info, "%s: Location parsed - %s = %ld", (int)__PRETTY_FUNCTION__, (int)s.Data(), tot);

   return tot;
}

void                 rCUESession::cleanUp()
{
}

void                 rCUESession::dispose()
{
   delete this;
}

bool                 rCUESession::isAudio()
{
   return static_isAudio();
}

bool                 rCUESession::isData()
{
   return static_isData();
}

bool                 rCUESession::fillOptItem(IOptItem *disc)
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

const char          *rCUESession::getTrackName()
{
   return static_getName();
}

uint32               rCUESession::getBlockCount()
{
   return blockCount;
}

uint16               rCUESession::getBlockSize()
{
   return blockSize;
}

const char          *rCUESession::static_getName()
{
   return "CUE Sheet Session";
}

bool                 rCUESession::static_isAudio()
{
   return false;
}

bool                 rCUESession::static_isData()
{
   return false;
}

bool                 rCUESession::static_isSession()
{
   return true;
}

bool                 rCUESession::checkFile(const char* file, EDtError &rc)
{
   BPTR  fh;
   uint8 s[1024];
   int   res = true;

   rc = DT_UnableToOpenFile;

   fh = DOS->Open(const_cast<char*>(file), MODE_OLDFILE);
   if (0 == fh)
      return false;

   rc = DT_OK;

   while (DOS->FGets(fh, (char*)s, sizeof(s)) != 0)
   {
      for (int i=0; s[i] != 0; i++)
      {
         if ((s[i] != 10) && (s[i] != 13) && (s[i] != '\t') && (s[i]<' '))
         {
            DOS->Close(fh);
            rc = DT_FileMalformed;
            return false;
         }
         if ((s[i] >=128) && (s[i]<160))
         {
            DOS->Close(fh);
            rc = DT_FileMalformed;
            return false;
         }
      }

      String str((char*)s);
      VectorT<String> v = str.Explode();
   
      if (v.Count() > 0)
      {
         str = v[0].UpperCase();

         if ((str == "TRACK") ||
             (str == "INDEX") ||
             (str == "FILE") ||
             (str == "FLAGS") ||
             (str == "REM") ||
             (str == "CATALOG") || 
             (str == "CDTEXTFILE") ||
             (str == "ISRC") ||
             (str == "POSTGAP") ||
             (str == "PREGAP") ||
             (str == "ARRANGER") ||
             (str == "COMPOSER") ||
             (str == "DISC_ID") ||
             (str == "GENRE") ||
             (str == "MESSAGE") ||
             (str == "PERFORMER") ||
             (str == "SONGWRITER") ||
             (str == "TITLE") ||
             (str == "TOC_INFO") ||
             (str == "TOC_INFO2") ||
             (str == "UPC_EAN") ||
             (str == "SIZE_INFO"))
         {
            continue;
         }
         else
         {
            rc = DT_FileMalformed;
            res = false;
            break;
         }
      }

   }

   DOS->Close(fh);
   return res;
}

DbgHandler*          rCUESession::getDebug()
{
   return dbg;
}

void                 rCUESession::setDebug(DbgHandler* nd)
{
   dbg = nd;
}

