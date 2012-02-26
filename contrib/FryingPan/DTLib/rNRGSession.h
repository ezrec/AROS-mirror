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

#ifndef _RNRGSESSION_H_
#define _RNRGSESSION_H_

#include "IFileReader.h"
#include "DTLib.h"
#include <Generic/String.h>
#include <Generic/VectorT.h>
#include <Generic/LibrarySpool.h>
#include <Optical/Optical.h>
#include <Generic/Debug.h>

using namespace GenNS;

class rNRGSession : public IFileReader
{
protected:
   DbgHandler                   *dbg;

   const IOptItem               *sess;
   String                        name;
   uint16                        blockSize;
   uint32                        blockCount;
   VectorT<class track*>         tracks;
   uint64                        catalog;

   String                        arranger;
   String                        composer;
   String                        performer;
   String                        message;
   String                        title;
   String                        songwriter;
   String                        upc_ean;
   String                        disc_id;

   class file                   *f;
   int32                         currtrk;

protected:
   rNRGSession(const char* sFileName, EDtError &rc);
   virtual                      ~rNRGSession();
   bool                          analyse(const char* sFileName, EDtError &rc);

   bool                          buildTrackList(struct nrg_cuex*, EDtError &rc);
   bool                          updateTrackData(struct nrg_daox*, EDtError &rc);
   bool                          updateTrackData(struct nrg_daoi*, EDtError &rc);

   DbgHandler                   *getDebug();
   void                          setDebug(DbgHandler*);

protected:
   static bool                   freeTrack(track* const& t);

public:
   static IFileReader           *openRead(const char* sFile, EDtError &rc);
   virtual const char*           getName();
   virtual bool                  readData(const IOptItem* item, void* pBuff, int pLen);
   virtual bool                  setUp();
   virtual void                  cleanUp();
   virtual void                  dispose();
   virtual const char           *getTrackName();
   virtual uint32                getBlockCount();
   virtual uint16                getBlockSize();
   virtual bool                  isAudio();
   virtual bool                  isData();
   virtual bool                  fillOptItem(IOptItem *disc);
   static bool                   static_isAudio();
   static bool                   static_isData();
   static bool                   static_isSession();
   static const char            *static_getName();
   static bool                   checkFile(const char* sFileName, EDtError &rc);
};

#endif

