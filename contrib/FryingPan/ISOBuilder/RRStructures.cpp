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

#include "RRStructures.h"
#include "ISOStructures.h"
#include <LibC/LibC.h>

RRExt::RRExt(uint8 id1, uint8 id2, uint8 flags, uint8 sets)
{
   this->id[0] =  id1;
   this->id[1] =  id2;
   this->flags =  flags;
   this->sets  =  sets;
}

RRExt::~RRExt()
{
};

uint8 RRExt::Length()
{
   return 5;
}

uint8* RRExt::PutData(uint8* loc)
{
   *loc++ = id[0];
   *loc++ = id[1];
   *loc++ = Length();
   *loc++ = 1;                

   if (sets & Set_Flags)
      *loc++ = flags;

   return loc;
};

void RRExt::SetFlags(uint8 flags)
{
   this->flags = flags;
}

uint8 RRExt::GetFlags()
{
   return flags;
}


RR_NM::RR_NM(const char* name) : 
   RRExt('N', 'M')
{
   SetName(name);
};

void RR_NM::SetName(const char* name)
{
   this->name = name;
}

uint8 RR_NM::Length()
{
   return RRExt::Length()+name.Length();
}

uint8 *RR_NM::PutData(uint8* loc)
{
   loc = RRExt::PutData(loc);
   strncpy((char*)loc, name.Data(), name.Length());
   loc += name.Length();
   return loc;
}


RR_RR::RR_RR(uint8 flags) :
   RRExt('R', 'R', flags)
{
};


RR_AS::RR_AS(const char* comment, uint8 protection) :
   RRExt('A', 'S')
{
   this->comment     = comment;
   this->protection  = protection;
}

uint8 RR_AS::Length()
{
   uint8 size = 0;
   uint8 flags = 0;

   size = comment.Length();
   if (size != 0)
   {
      size++;
      flags |= 2;
   }

   if (protection != 0)
   {
      size += 4;
      flags |= 1;
   }

   RRExt::SetFlags(flags);
   if (flags == 0)
      return 0;

   return RRExt::Length() + size;
}
   
uint8 *RR_AS::PutData(uint8* loc)
{
   uint8 flags = RRExt::GetFlags();

   if (flags == 0)
      return loc;

   loc = RRExt::PutData(loc);

   if (flags & 1)
   {
      *loc++ = 0;
      *loc++ = 0;
      *loc++ = 0;
      *loc++ = protection;
   }

   if (flags & 2)
   {
      *loc++   = comment.Length()+1;
      strncpy((char*)loc, comment.Data(), comment.Length());
      loc += comment.Length();
   }

   return loc;
}

void RR_AS::SetFlags(uint8 flags)
{
   protection = flags;
}

void RR_AS::SetElemComment(const char* comment)
{
   this->comment = comment;
}
   
const String &RR_AS::GetElemComment() const
{
   return comment;
}


RR_SP::RR_SP() : 
   RRExt('S', 'P')
{
}

uint8 RR_SP::Length()
{
   return 7;
}

uint8 *RR_SP::PutData(uint8* loc)
{
   *loc++ = 'S';
   *loc++ = 'P';
   *loc++ = 7;
   *loc++ = 1;
   *loc++ = 0xBE;
   *loc++ = 0xEF;
   *loc++ = 0x00;
   return loc;
}


RR_PX::RR_PX(uint8 prot, bool dir) :
   RRExt('P', 'X')
{
   SetFlags(prot);
   SetDir(dir);
}

uint8 RR_PX::Length()
{
   return 44;
}

uint8 *RR_PX::PutData(uint8* loc)
{
   val733 *v;

   *loc++ = 'P';
   *loc++ = 'X';
   *loc++ = 44;
   *loc++ = 1;
   v = (val733*)loc;
   v->setVal(flags);
   loc += sizeof(val733);

   v = (val733*)loc;
   v->setVal(0);
   loc += sizeof(val733);

   v = (val733*)loc;
   v->setVal(0);
   loc += sizeof(val733);

   v = (val733*)loc;
   v->setVal(0);
   loc += sizeof(val733);

   v = (val733*)loc;
   v->setVal(0);
   loc += sizeof(val733);

   return loc;
}

void RR_PX::SetFlags(uint8 p)
{
   prot = p;

   flags = 0;

   if ((prot & 8) == 0)
      flags |= S_IROTH | S_IRGRP | S_IRUSR;
   if ((prot & 4) == 0)
      flags |= S_IWOTH | S_IWGRP | S_IWUSR;
   if ((prot & 2) == 0)
      flags |= S_IXOTH | S_IXGRP | S_IXUSR;

   if (dir)
      flags |= S_IFDIR;
   else
      flags |= S_IFREG;
}

uint8 RR_PX::GetFlags()
{
   return prot;
}
   
void RR_PX::SetDir(bool dir)
{
   this->dir = dir;

   flags &= ~(S_IFDIR | S_IFREG);
   if (dir)
      flags |= S_IFDIR;
   else
      flags |= S_IFREG;
}

/*
 * RR_CE: content extension entry
 * no flags
 */
RR_CE::RR_CE() :
   RRExt('C', 'E', 0, 0)
{
}

void RR_CE::setBlock(uint32 block)
{
   this->block = (block);
}

void RR_CE::setOffset(uint32 offset)
{
   this->offset = (offset);
}

void RR_CE::setLength(uint32 length)
{
   this->length = (length);
}

uint8 RR_CE::Length()
{
   return 28;
}

uint8 *RR_CE::PutData(uint8* loc)
{
   val733 *v;

   loc = RRExt::PutData(loc);

   v = (val733*)loc;
   v->setVal(block);
   v++;
   v->setVal(offset);
   v++;
   v->setVal(length);

   return (uint8*) v;
}



