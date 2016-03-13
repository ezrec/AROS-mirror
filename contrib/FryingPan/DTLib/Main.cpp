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

#include "Main.h"
#include <exec/lists.h>
#include <GenericLib/Library.h>
#include <Generic/VectorT.h>
#include "ReaderHook.h"
#include "WriterHook.h"
#include "rRAWAudio.h"
#include "rSplitISOData.h"
#include "rAIFFAudio.h"
#include "rISOData.h"
#include "rMP3Audio.h"
#include "wRAWAudioData.h"
#include "wSplitISOData.h"
#include "wAIFFAudio.h"
#include "wWAVAudio.h"
#include "rWAVAudio.h"
#include "wCUESession.h"
#include "rCUESession.h"
#include "rNRGSession.h"
#include <LibC/LibC.h>
#include <Generic/LibrarySpool.h>

   LIBRARY("dtlibio.module", "$VER: dtlibio.module 1.0 Tomasz Wiszkowski", 1);
   GATE1(bool, dtlRegister,   MinList *, a0);
   GATE1(void, dtlUnregister, MinList *, a0);

   LIB_FT_Begin
      LIB_FT_Function(dtlRegister)
      LIB_FT_Function(dtlUnregister)
   LIB_FT_End

   VectorT< IRegHook* > *pReaders;
   VectorT< IRegHook* > *pWriters;
   LibrarySpool *pSpool;

   DbgHandler* dbg;

   uint32 StartupFlags = 0;

bool Lib_SetUp()
{
   __setup();
   LibrarySpool::Init();
   
   pReaders = new VectorT< IRegHook* >();
   pWriters = new VectorT< IRegHook* >();

   *pReaders << new ReaderHook<rISOData,        100>;
   *pReaders << new ReaderHook<rSplitISOData,   30>;
   *pReaders << new ReaderHook<rMP3Audio,       15>;
   *pReaders << new ReaderHook<rAIFFAudio,      10>;
   *pReaders << new ReaderHook<rWAVAudio,       10>;
   *pReaders << new ReaderHook<rRAWAudio,      -100>;
   
   *pReaders << new ReaderHook<rCUESession,    -100>;
   *pReaders << new ReaderHook<rNRGSession,    -120>;

   *pWriters << new WriterHook<wSplitISOData,   100>;
   *pWriters << new WriterHook<wWAVAudio,       15>;
   *pWriters << new WriterHook<wAIFFAudio,      10>;
   *pWriters << new WriterHook<wRAWAudioData,  -100>;

   *pWriters << new WriterHook<wCUESession,     100>;
   return true;
}

void Lib_CleanUp()
{
#if (0)
   for (int i=0; i<pReaders->Count(); i++)
      delete (*pReaders)[i];
   for (int i=0; i<pWriters->Count(); i++)
      delete (*pWriters)[i];
#endif
   delete pReaders;
   delete pWriters;

   LibrarySpool::Exit();
   __cleanup();
}

bool Lib_Acquire()
{
   return true;
}

void Lib_Release()
{
}

int main()
{
    request("Information", "This module is not executable\nIt is a part of FryingPan Package", "Ok", 0);
    return -1;
}

bool dtlRegister(MinList *pList)
{
   ASSERT(pList != 0);
   EList *eList = new EList(pList);

   for (int i=0; i<pReaders->Count(); i++)
   {
      eList->AddFeatured((*pReaders)[i], (*pReaders)[i]->getName(), (*pReaders)[i]->getPriority()); 
   }

   for (int i=0; i<pWriters->Count(); i++)
   {
      eList->AddFeatured((*pWriters)[i], (*pWriters)[i]->getName(), (*pWriters)[i]->getPriority()); 
   }

   delete eList;
   return true;
}

void dtlUnregister(MinList *pList)
{
   ASSERT(pList != 0);
   EList *eList = new EList(pList);

   for (int i=0; i<pReaders->Count(); i++)
   {
      eList->Rem((*pReaders)[i]);
   }
   
   for (int i=0; i<pWriters->Count(); i++)
   {
      eList->Rem((*pWriters)[i]);
   }
   
   delete eList;
}

// vim: ts=3 et
