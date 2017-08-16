/*
 * Amiga Generic Set - set of libraries and includes to ease sw development for all Amiga platforms
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __LINUX__
#include "Debug.h"
#include <LibC/LibC.h>
#include <libclass/utility.h>
#include <libclass/dos.h>
#include <dos/datetime.h>

using namespace GenNS;

   DbgMaster *DbgMaster::DebugMaster = 0;
   long       DbgMaster::lNumInstances = 0; 

  
DbgMsg::DbgMsg(const DbgHandler *pParent, DbgLevel lvl, const char *sFmtStr, IPTR vFmtArgs, void *bData, int lLen)
   : DbgCmd(pParent, lvl)
{
   text     = 0;
   data     = 0;
   len      = 0;
   
   if (0 != sFmtStr)
   {
      text = new String();
      text->FormatStr(sFmtStr, vFmtArgs);
   }

   if (0 != bData)
   {
      len  = lLen < 2048 ? lLen : 2048;
      data = new char[lLen];
      Exec->CopyMem(bData, data, lLen);
   }
};

DbgMsg::~DbgMsg()
{
   if (0 != data) delete [] (char const*)data;
   if (0 != text) delete text;
};

const char* DbgMsg::Text()
{
   if (text == 0)
      return 0;
   return text->Data();
}

APTR DbgMsg::Data()
{
   return data;
}

uint16 DbgMsg::Length()
{
   return len;
};

void DbgMsg::Perform()
{
   parent->DumpMsg(this);
}

DbgHandler::DbgHandler(int loud, const char *file)
{
   next   = 0;
   silent = loud ? 0 : 1;
   fh1    = 0;
   fh2    = 0;
   nodate = 0;
   flags  = 0xffff;
   name   = file;

   stask  = Exec->FindTask(0);
   dest   = DbgMaster::GetDebug();

   dest->DoAsync(DbgMaster::Cmd_InitH, this);
}

void DbgHandler::InitH()
{
   BPTR tmp = 0;
   char var[128];
   if (0 < DOS->GetVar("TBS_LOGEVENTS", var, 64, 0))
   {
      String s(var);
      flags = s.ToLong();
   }
   
   if (0 < DOS->GetVar("TBS_VERBOSELOG", var, 64, 0))
   {
      String s(var);
      silent = (s.ToLong() == 0 ? true : silent);
   }
   else
      silent = true;

   if (0 < DOS->GetVar("TBS_LOGDIR", var, 128, 0))
   {
      tmp = DOS->Lock(var, ACCESS_READ);
      if (tmp != 0)
         DOS->UnLock(tmp);
      else
         strcpy(var, "RAM:");
   }
   else
      strcpy(var, "RAM:");


   if (!dest) 
      return;
   
   String *i = new String();
   String *j = new String();

   *i = var;
   i->AddPath("logs");

   if (BPTR lock = DOS->Lock(i->Data(), ACCESS_READ)) 
   {
      DOS->UnLock(lock);
   } 
   else {
      DOS->UnLock(DOS->CreateDir(i->Data()));
   }

   j->FormatStr("%s_%05ld.txt", ARRAY((uint32)name.Data(), Utility->GetUniqueID()));
   i->AddPath(j->Data());
   DOS->DeleteFile(i->Data());
   fh1 = DOS->Open(i->Data(), MODE_READWRITE);
   i->FormatStr("con:0/10//80/Debug: %s/auto/close/wait", ARRAY((IPTR)name.Data()));
   if (!silent) fh2 = DOS->Open(i->Data(), MODE_NEWFILE);
   delete i;
   delete j;
};

DbgHandler::~DbgHandler()
{
   dest->DoSync(DbgMaster::Cmd_ExitH, this);
   DbgMaster::FreeDebug();
};

void DbgHandler::ExitH()
{
   if (fh2 != 0) 
      DOS->Close(fh2);
   if (fh1 != 0)
      DOS->Close(fh1);
}

void DbgHandler::PutDate() const
{
   if ((!fh1) && (!fh2)) 
      return;

   unsigned char  DT[16];
   unsigned char  TM[16];
   unsigned char  DAY[16];
   DateTime       CDT;

   CDT.dat_StrDate = (UBYTE*)&DT;
   CDT.dat_StrTime = (UBYTE*)&TM;
   CDT.dat_StrDay  = (UBYTE*)&DAY;
   CDT.dat_Format  = FORMAT_CDN;
   CDT.dat_Flags   = 0;

   DOS->DateStamp(&CDT.dat_Stamp);
   DOS->DateToStr(&CDT);
   if (fh2 != 0) DOS->VFPrintf(fh2, "[%s] ", (void*)ARRAY((IPTR)&TM));
   if (fh1 != 0) DOS->VFPrintf(fh1, "[%s] ", (void*)ARRAY((IPTR)&TM));
}

void DbgHandler::DoAsync(DbgLevel l, const char* sFmtString, IPTR vFmtArgs, void* bMemBlock, int lMemLen) const
{
   if (0 == this)
      return;
   if (!(flags & l))
      return;
   dest->DoAsync(0, new DbgMsg(this, l, sFmtString, vFmtArgs, bMemBlock, lMemLen));
}

void DbgHandler::DoSync(DbgLevel l, const char* sFmtString, IPTR vFmtArgs, void* bMemBlock, int lMemLen) const
{
   if (0 == this)
      return;

   if (!(flags & l))
      return;
   dest->DoSync(0, new DbgMsg(this, l, sFmtString, vFmtArgs, bMemBlock, lMemLen));
}

void DbgHandler::DumpMsg(DbgMsg* m) const
{
   if ((0 == fh1) &&
       (0 == fh2))
   {
      return;
   }
   
   String *pS      = new String();

   if (0 != m->Data())
   {
      unsigned char *bMemBuf = (unsigned char*)m->Data();
      long           lCurr   = 0;
      long           lMax    = m->Length();

      *pS = "[**DATA**]  ===";
      if (0 != m->Text())
      {
         *pS += " ";
         *pS += m->Text();
         *pS += " ===";
      }
      while (pS->Length() < 90)
      {
         *pS += "=";
      }
            
      if (fh2 != 0) DOS->FPuts(fh2, pS->Data());
      if (fh1 != 0) DOS->FPuts(fh1, pS->Data());
      
      while (lCurr < lMax)
      {
         int lLen;
         int i;
         
         lLen = lMax - lCurr;                   // calculate size of
         if (lLen > 16) lLen = 16;              // data to be printed in this line
         
         if (0 == (lCurr % 16))                 // place enter
         {                                      // as we are
            if (fh2 != 0) DOS->FPuts(fh2, "\n");           // :)
            if (fh1 != 0) DOS->FPuts(fh1, "\n");              //
         }                                      //
         
         if (fh2 != 0) DOS->FPuts(fh2, "[**DATA**]  ");
         if (fh1 != 0) DOS->FPuts(fh1, "[**DATA**]  ");

         for (i=0; i<lLen; i++)
         {
            if (fh2 != 0) DOS->VFPrintf(fh2, "%02lx ", (void*)ARRAY(bMemBuf[lCurr+i]));
            if (fh1 != 0) DOS->VFPrintf(fh1, "%02lx ", (void*)ARRAY(bMemBuf[lCurr+i]));
         }

         for (;i<17; i++)
         {
            if (fh2 != 0) DOS->FPuts(fh2, "   ");
            if (fh1 != 0) DOS->FPuts(fh1, "   ");
         }

         for (i=0; i<lLen; i++)
         {
            unsigned char c = bMemBuf[lCurr+i];
            if (((c>0x1f) && (c<0x80)) ||
                 (c>0x9f))
            {
               if (fh2 != 0) DOS->FPutC(fh2, c);
               if (fh1 != 0) DOS->FPutC(fh1, c);
            }
            else
            {
               if (fh2 != 0) DOS->FPutC(fh2, '.');
               if (fh1 != 0) DOS->FPutC(fh1, '.');
            }
         }
         
         lCurr += lLen;
      }
      
      if (fh2 != 0) DOS->FPuts(fh2, "\n");                // :)
      if (fh1 != 0) DOS->FPuts(fh1, "\n");                   //
      if (fh2 != 0) DOS->FPuts(fh2, pS->Data());
      if (fh1 != 0) DOS->FPuts(fh1, pS->Data());

   }
   else
   {
      if (0 != m->Text())
      {
         PutDate();
         if (fh2 != 0) DOS->FPuts(fh2, m->Text());                 // no end-of-line here!
         if (fh1 != 0) DOS->FPuts(fh1, m->Text());                    // no end-of-line here!
      }   
   }
   
   if (fh2 != 0) DOS->FPuts(fh2, "\n");
   if (fh1 != 0) DOS->FPuts(fh1, "\n");
   if (fh2 != 0) DOS->Flush(fh2);
   if (fh1 != 0) DOS->Flush(fh1);
   delete pS; 
}


DbgMaster::DbgMaster(void)
{
   pThread = new Thread("TBS Debug SubProcess");
   hHkProc.Initialize(this, &DbgMaster::ProcMsgs);
   pThread->SetHandler(hHkProc.GetHook());
}

DbgMaster::~DbgMaster(void)
{
   delete pThread;
}

bool DbgMaster::CleanUp(void)
{
   if (0 == DbgMaster::DebugMaster)
      return true;
   
   if (0 != DbgMaster::lNumInstances)
      return false;
      
   delete DbgMaster::DebugMaster;
   DbgMaster::DebugMaster = 0;
   return true;
}

unsigned long DbgMaster::ProcMsgs(DbgMaster::Cmd lCmd, void* M)
{
   switch (lCmd)
   {
      case Cmd_Message:
         {
            DbgCmd *m = (DbgCmd*)M;
            m->Perform();
            delete m;
         }
         break;

      case Cmd_InitH:
         {
            DbgHandler *h = (DbgHandler*)M;
            h->InitH();
         }
         break;

      case Cmd_ExitH:
         {
            DbgHandler *h = (DbgHandler*)M;
            h->ExitH();
         }
         break;
   }
   return 0;
}


Thread *DbgMaster::GetDebug()
{
   if (DbgMaster::DebugMaster == 0)
   {
      DbgMaster::DebugMaster = new DbgMaster();
   }
   ++DbgMaster::lNumInstances;
   return DbgMaster::DebugMaster->pThread;
}

void DbgMaster::FreeDebug()
{
   --DbgMaster::lNumInstances;
}

#endif
