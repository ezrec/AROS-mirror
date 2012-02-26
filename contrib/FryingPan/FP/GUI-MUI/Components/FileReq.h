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

#ifndef _GUIMUI_COMPONENTS_FILEREQUESTER_H_
#define _GUIMUI_COMPONENTS_FILEREQUESTER_H_

#include <Generic/String.h>
#include <Generic/VectorT.h>

using namespace GenNS;

class FileReq
{
protected:
   String                        lastPath;
   String                        title;
   bool                          multiSelect;
   bool                          dirsOnly;
   String                        lastSaveFile;
   VectorT<const char*>          lastOpenFiles;

protected:
   static bool                   freeFiles(const char* const& file);

public:
                                 FileReq(const char* title);
   virtual                      ~FileReq();
   
   virtual VectorT<const char*>  &openReq();
   virtual const char*            saveReq();

   virtual void                   setTitle(const char*);
   virtual void                   setPath(const char*);
   virtual const char*            getPath();
   virtual void                   setMultiSelect(bool);
   virtual void                   setDirsOnly(bool);
};

#endif
