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

#ifndef _RRSTRUCTURES_H_
#define _RRSTRUCTURES_H_

#include <Generic/Types.h>
#include <Generic/String.h>

using namespace GenNS;

class RRExt 
{
   uint8             id[2];
   uint8             flags;
   uint8             sets;

public:
   enum
   {
      Set_Flags = 1
   };
public:
                     RRExt(uint8 id1, uint8 id2, uint8 flags=0, uint8 sets=Set_Flags);
   virtual          ~RRExt();
   virtual uint8     Length();
   virtual void      SetFlags(uint8 flags);
   virtual uint8     GetFlags();
   virtual uint8    *PutData(uint8* loc);
};

class RR_NM : public RRExt
{
   String            name;
public:
                     RR_NM(const char* name);
   virtual void      SetName(const char* name);                     
   virtual uint8     Length();
   virtual uint8    *PutData(uint8* loc);
};

class RR_RR : public RRExt
{
public:
   enum 
   {
      Use_PX   = 0x01,
      Use_NM   = 0x08,
      Use_TF   = 0x80
   };
public:
                     RR_RR(uint8 flags);
};

class RR_SP : public RRExt
{
public:
                     RR_SP();
   virtual uint8     Length();
   virtual uint8    *PutData(uint8* loc);
};

class RR_AS : public RRExt
{
   uint8    protection;
   String   comment;
public:
                           RR_AS(const char* comment, uint8 protection);
   virtual uint8           Length();
   virtual void            SetFlags(uint8 flags);
   virtual void            SetElemComment(const char* comment);
   virtual const String   &GetElemComment() const;
   virtual uint8          *PutData(uint8* loc);
};

class RR_PX : public RRExt
{
protected:
   enum 
   {
      S_IXOTH  =  0000001,
      S_IWOTH  =  0000002,
      S_IROTH  =  0000004,
      S_IXGRP  =  0000010,
      S_IWGRP  =  0000020,
      S_IRGRP  =  0000040,
      S_IXUSR  =  0000100,
      S_IWUSR  =  0000200,
      S_IRUSR  =  0000400,

      S_ISVTX  =  0001000,    // save swapped text even after use
      S_ISGID  =  0002000,    // set group id on execution
      S_ISUID  =  0004000,    // set user id on execution

      S_IFIFO  =  0010000,    // fifo
      S_IFCHR  =  0020000,    // character device
      S_IFDIR  =  0040000,    // directory
      S_IFBLK  =  0060000,    // block device
      S_IFREG  =  0100000,    // regular file
      S_IFLNK  =  0120000,    // symbolic link
      S_IFSOCK =  0140000,    // socket
   };

protected:
   uint32            flags;
   uint8             prot;
   bool              dir;
public:
                     RR_PX(uint8 flags=0, bool dir=false);
   virtual uint8     Length();
   virtual uint8    *PutData(uint8* loc);
   virtual void      SetFlags(uint8 flags);
   virtual void      SetDir(bool dir);
   virtual uint8     GetFlags();
};

class RR_CE : public RRExt
{
protected:
   uint8             version;
   uint32            block;
   uint32            offset;
   uint32            length;

public:
                     RR_CE();
   virtual uint8     Length();
   virtual uint8    *PutData(uint8* loc);
   void              setBlock(uint32 block);
   void              setOffset(uint32 offset);
   void              setLength(uint32 length);
};


#endif
