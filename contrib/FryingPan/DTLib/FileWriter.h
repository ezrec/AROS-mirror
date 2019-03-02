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

#ifndef FILEWRITER_H_
#define FILEWRITER_H_

#include <Generic/LibrarySpool.h>
#include <Optical/IOptItem.h>
#include <dos/dos.h>
#include "IFileWriter.h"
#include <Generic/String.h>
#include "DTLib.h"

class FileWriter : public IFileWriter
{
private:
   BPTR                       fh;
   String                     sFileName;
   bool                       isLE;
   uint16                     sectorSize;
   uint32                     sectorCount;

protected:
                                       FileWriter(const char* sFileName, EDtError &rc);
   
   BPTR                                getFile();
   const char*                         getFileName();
   
   void                                setLittleEndian(bool isLittle);
public:  // non pure-virtual
   virtual                            ~FileWriter();
   virtual bool                        writeData(void* pBuff, int len);
   virtual bool                        setUp();
   virtual void                        cleanUp();
   virtual const char     	           *getTrackName();
   virtual void                        dispose();
   virtual uint16                      getBlockSize();
   virtual uint32                      getBlockCount();
   virtual void                        deleteFiles();
   virtual void                        setBlockSize(unsigned short size);
   virtual void                        setBlockCount(unsigned long count);
};

#endif /*FILEREADER_H_*/
// vim: ts=3 et
