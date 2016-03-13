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

#ifndef CLISO_H_
#define CLISO_H_

#include "IBrowser.h"
#include "Structure/ClRoot.h"
#include "ISOStructures.h"
#include "Structure/ClRoot.h"
#include <Generic/HookT.h>
#include <Generic/VectorT.h>
#include <Generic/Debug.h>

using namespace GenNS;


class ClISO : public IBrowser
{
protected:
   DbgHandler                         *debug;

public:
   void                                setDebug(DbgHandler*);
   DbgHandler                         *getDebug();

protected:

   ClRoot                             *pRoot;
   ClDirectory                        *pCurrDir;
   VectorT<const Hook*>                hDataHooks;
   HookT<ClISO, IPTR, long>            hDataFetch;
   bool                                bAbortDataFlow;
   IPTR                                pMemBlk;
   unsigned long                       lMemBlkSize;
   unsigned long                       lCurrentPos;
protected:
   unsigned long                       fDataPass(IPTR Data, long lSize);

public:
                                       ClISO();
   virtual                            ~ClISO();

   virtual ClRoot                     *getRoot();
   virtual ClDirectory                *getParent();
   virtual ClDirectory                *getCurrDir();
   virtual void                        goRoot();
   virtual void                        goParent();
   virtual void                        destroy();
   virtual ClDirectory                *makeDir();
   virtual void                        setCurrDir(ClDirectory*);
   virtual unsigned long               validate();
   virtual bool                        isEmpty();

   virtual void                        setISOLevel(ISOLevel);
   virtual ISOLevel                    getISOLevel();

   void                                addDataHook(const Hook *pHook);
   void                                remDataHook(const Hook *pHook);
  
   bool                                recalculate(); 
   bool                                generate();
   long                                startDataFlow();
   void                                abortDataFlow();
};

#endif /*CLISO_H_*/
