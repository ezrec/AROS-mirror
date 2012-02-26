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

#ifndef _WCUESESSION_H_
#define _WCUESESSION_H_

#include "IFileWriter.h"
#include "DTLib.h"
#include <Generic/String.h>

using namespace GenNS;

class wCUESession : public IFileWriter
{
protected:
   const IOptItem               *sess;
   String                        name;
   BPTR                          fcue;
   BPTR                          fbin;

protected:
                                 wCUESession(const char* sFileName, const IOptItem *it, EDtError &rc);
   virtual                      ~wCUESession();
public:
   static IFileWriter           *openWrite(const char* sFile, const IOptItem *it, EDtError &rc);
   virtual const char*           getName();
   virtual bool                  writeData(void* pBuff, int pLen);
   virtual bool                  setUp();
   virtual void                  cleanUp();
   virtual void                  deleteFiles();
   virtual void                  dispose();
   virtual void                  setBlockCount(unsigned long);
   virtual void                  setBlockSize(unsigned short);
   static bool                   static_isAudio();
   static bool                   static_isData();
   static bool                   static_isSession();
   static const char            *static_getName();

};

#endif

