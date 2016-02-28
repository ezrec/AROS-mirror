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

#include <Generic/LibrarySpool.h>
#include "FileReq.h"
#include <libclass/dos.h>
#include <libraries/asl.h>
#include <libclass/muimaster.h>
#include <LibC/LibC.h>

FileReq::FileReq(const char *ttl)
{
   ttl            = title;
   lastPath       = "";
   multiSelect    = false;
   dirsOnly       = false;
   lastSaveFile   = "";
}

FileReq::~FileReq()
{
   lastOpenFiles.ForEach(&freeFiles);
}
   
bool FileReq::freeFiles(const char* const& file)
{
   delete file;
   return true;
}

void FileReq::setTitle(const char *ttl)
{
   title = ttl;
}

void FileReq::setPath(const char *pth)
{
   lastPath = pth;
}

void FileReq::setMultiSelect(bool state)
{
   multiSelect = state;
}

void FileReq::setDirsOnly(bool state)
{
   dirsOnly = state;
}

const char* FileReq::saveReq()
{
   bool           result = false;
   FileRequester* req    = (FileRequester*)MUIMaster->MUI_AllocAslRequest(ASL_FileRequest, 0);
   TagItem        tags[] =
   {
      { ASLFR_TitleText,        (IPTR)title.Data()     },
      { ASLFR_DoSaveMode,       true                  },
      { ASLFR_DoMultiSelect,    false                 },
      { ASLFR_DrawersOnly,      dirsOnly              },
      { ASLFR_InitialDrawer,    (IPTR)lastPath.Data()  },
      { TAG_DONE,               0                     },
   };

   lastSaveFile = "";

   if (NULL != req)
   {
      if (0 != MUIMaster->MUI_AslRequest(req, tags))
      {
         BPTR  lock = 0;

         lastPath = req->fr_Drawer;
         lock = DOS->Lock(req->fr_Drawer, ACCESS_READ);

         if (0 != lock)
         {
            char *temp = new char[1024];

            DOS->NameFromLock(lock, temp, 1024);
            if (false == dirsOnly)
            {
               DOS->AddPart(temp, req->fr_File, 1024);
            }

            lastSaveFile = temp;
            result       = true;
            delete [] temp;

            DOS->UnLock(lock);
         }
      }
      
      MUIMaster->MUI_FreeAslRequest(req);
   }

   if (result)
      return lastSaveFile.Data();
   return 0;
}

VectorT<const char*> &FileReq::openReq()
{
   FileRequester* req    = (FileRequester*)MUIMaster->MUI_AllocAslRequest(ASL_FileRequest, 0);
   TagItem        tags[] =
   {
      { ASLFR_TitleText,        (IPTR)title.Data()     },
      { ASLFR_DoSaveMode,       false                 },
      { ASLFR_DoMultiSelect,    multiSelect           },
      { ASLFR_DrawersOnly,      dirsOnly              },
      { ASLFR_InitialDrawer,    (IPTR)lastPath.Data()  },
      { TAG_DONE,               0                     }
   };

   lastOpenFiles.ForEach(&freeFiles).Empty();

   if (NULL != req)
   {
      if (0 != MUIMaster->MUI_AslRequest(req, tags))
      {
         BPTR  lock = 0;
         char *temp = 0;

         lastPath = req->fr_Drawer;

         if (multiSelect)
         {
            if (req->fr_NumArgs > 0)
            {
               for (int i=0; i<req->fr_NumArgs; i++)
               {
                  temp = new char[1024];
                  strncpy(temp, req->fr_Drawer, 1024);
                  DOS->AddPart(temp, (char*)req->fr_ArgList[i].wa_Name, 1024);
            
                  lock = DOS->Lock(temp, ACCESS_READ);
      
                  if (lock != 0)
                  {
                     DOS->NameFromLock(lock, temp, 1024);
                     lastOpenFiles << temp;
                     DOS->UnLock(lock);
                  }
                  else
                  {
                     delete [] temp;
                  }
               }
            }
            else
            {
               temp = new char[1024];
               strncpy(temp, req->fr_Drawer, 1024);
               lock = DOS->Lock(req->fr_Drawer, ACCESS_READ);

               if (lock != 0)
               {
                  DOS->NameFromLock(lock, temp, 1024);
                  lastOpenFiles << temp;
                  DOS->UnLock(lock);
               }
               else
               {
                  delete [] temp;
               }
            }
         }
         else
         {
            temp = new char[1024];
            strncpy(temp, req->fr_Drawer, 1024);
            if (false == dirsOnly)
            {
               DOS->AddPart(temp, req->fr_File, 1024);
            }
            
            lock = DOS->Lock(temp, ACCESS_READ);

            if (lock != 0)
            {
               DOS->NameFromLock(lock, temp, 1024);
               lastOpenFiles << temp;
               DOS->UnLock(lock);
            }
            else
            {
               delete [] temp;
            }
         }

      }
      
      MUIMaster->MUI_FreeAslRequest(req);
   }

   return lastOpenFiles;
}

const char* FileReq::getPath()
{
   return lastPath.Data();
}

