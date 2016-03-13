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

#ifndef _UTILITYIFACE_H_
#define _UTILITYIFACE_H_

#if defined (_PROTO_UTILITY_H) || defined (PROTO_UTILITY_H)
#error "Please remove proto/utility includes first!"
#endif

#define _PROTO_UTILITY_H
#define PROTO_UTILITY_H

#include <libclass/exec.h>

#define MOS_DIRECT_OS
#include <GenericLib/Calls.h>
#undef MOS_DIRECT_OS


extern class UtilityIFace *Utility;

#include <utility/tagitem.h>

   BEGINDECL(Utility, "utility.library")
#ifdef __AMIGAOS4__
      FUNC1(struct TagItem*,        AllocateTagItems,       5, uint32,              numTags, d0);
      PROC2(                        ApplyTagChanges,        6, struct TagItem*,     list,    a0,   const struct TagItem*,  changes, a1);
      FUNC1(struct TagItem*,        CloneTagItems,          7, struct TagItem*,     list,    a0);
      PROC3(                        FilterTagChanges,       8, struct TagItem*,     changes, a0,   struct TagItem*,        orig,    a1, uint32,                apply,   d0);
      FUNC3(uint32,                 FilterTagItems,         9, struct TagItem*,     list,    a0,   Tag*,                   farray,  a1, uint32,                logic,   d0);
      FUNC2(struct TagItem*,        FindTagItem,           10, Tag,                 value,   d0,   const struct TagItem*,  list,    a0);
      PROC1(                        FreeTagItems,          11, struct TagItem*,     list,    a0);
      FUNC3(IPTR,                 GetTagData,            12, Tag,                 value,   d0,   IPTR,                 deflt,   d1, const struct TagItem*, list,    a0);
      PROC3(                        MapTags,               13, struct TagItem*,     list,    a0,   struct TagItem*,        maplist, a1, uint32,                maptype, d0);
      FUNC1(struct TagItem*,        NextTagItem,           14, struct TagItem**,    tags,    a0);
      FUNC3(uint32,                 PackBoolTags,          15, uint32,              initflg, d0,   struct TagItem*,        list,    a0, const struct TagItem*, boolmap, a1);
      PROC2(                        RefreshTagItemClones,  16, struct TagItem*,     clone,   a0,   struct TagItem*,        orig,    a1);
      FUNC2(bool,                   TagInArray,            17, Tag,                 value,   d0,   const Tag*,             array,   a0);
      FUNC3(IPTR,                 CallHookPkt,           18, const struct Hook*,  hook,    a0,   IPTR,                  param1,  a2, IPTR,                 param2,  a1);
      PROC0(                        dummy1,                19);
      FUNC2(bool,                   AddNamedObject,        20, struct NamedObject*, namespc, a0,   struct NamedObject*,    object,  a1);
      FUNC2(struct NamedObject*,    AllocNamedObjectA,     21, const char*,         name,    a0,   const struct TagItem*,  tags,    a1);
      PROC0(                        dummy2,                22);
      FUNC1(int32,                  AttemptRemNamedObject, 23, struct NamedObject*, object,  a0);
      FUNC3(struct NamedObject*,    FindNamedObject,       24, struct NamedObject*, namespc, a0,   const char*,            name,    a1, struct NamedObject*,   prev,    a2);
      FUNC1(struct NamedObject*,    FreeNamedObject,       25, struct NamedObject*, object,  a0);
      FUNC1(const char*,            NamedObjectName,       26, struct NamedObject*, object,  a0);
      PROC1(                        ReleaseNamedObject,    27, struct NamedObject*, object,  a0);
      PROC2(                        RemNamedObject,        28, struct NamedObject*, object,  a0,   struct Message*,        msg,     a1);
      PROC2(                        Amiga2Date,            29, uint32,              amidate, d0,   struct ClockData*,      data,    a0);
      FUNC1(uint32,                 CheckDate,             30, struct ClockData*,   date,    a0);
      FUNC1(uint32,                 Date2Amiga,            31, struct ClockData*,   date,    a0);
      FUNC2(struct SkipList*,       CreateSkipList,        32, const struct Hook*,  hook,    a0,   int32,                  levels,  d0);
      PROC1(                        DeleteSkipList,        33, struct SkipList*,    list,    a0);
      FUNC2(struct SkipNode*,       FindSkipNode,          34, const SkipList*,     list,    a0,   IPTR,                  key,     a1);
      FUNC1(struct SkipNode*,       GetFirstSkipNode,      35, const SkipList*,     list,    a0);
      FUNC2(struct SkipNode*,       GetNextSkipNode,       36, const SkipList*,     list,    a0,   struct SkipNode*,       node,    a1);
      FUNC3(struct SkipNode*,       InsertSkipNode,        37, struct SkipList*,    list,    a0,   IPTR,                  key,     a1, uint32,                dtsize,  d0);
      FUNC2(bool,                   RemoveSkipNode,        38, struct SkipList*,    list,    a0,   IPTR,                  key,     a1);
      FUNC1(struct SplayTree*,      CreateSplayTree,       39, const struct Hook*,  hook,    a0);
      PROC1(                        DeleteSplayTree,       40, struct SplayTree*,   tree,    a0);
      FUNC2(struct SplayNode*,      FindSplayNode,         41, const SplayTree*,    tree,    a0,   IPTR,                  key,     a1);
      FUNC3(struct SplayNode*,      InsertSplayNode,       42, struct SplayTree*,   tree,    a0,   IPTR,                  key,     a1, uint32,                dtsize,  d0);
      FUNC2(struct SplayNode*,      RemoveSplayNode,       43, struct SplayTree*,   tree,    a0,   IPTR,                  key,     a1);
      FUNC2(struct Node*,           FindNameNC,            44, const struct List*,  list,    a0,   const char*,            name,    a1);
      FUNC0(uint32,                 GetUniqueID,           45);
      PROC1(                        Digest_SHA_Final,      46, struct MessageDigest_SHA*, sha, a0);
      PROC1(                        Digest_SHA_Init,       47, struct MessageDigest_SHA*, sha, a0);
      PROC3(                        Digest_SHA_Update,     48, struct MessageDigest_SHA*, sha, a0, IPTR,                  data,    a1, int32,                 len,     d0);
      FUNC3(uint32,                 PackStructureTags,     49, uint8*,              pack,    a0,   uint32*,                packtab, a1, struct TagItem*,       list,    a2);
      FUNC3(uint32,                 UnpackStructureTags,   50, uint8*,              pack,    a0,   uint32*,                packtab, a1, struct TagItem*,       list,    a2);
      FUNC1(uint32,                 Random,                51, struct RandomState*, state,   a0);
      FUNC3(IPTR,                  SetMem,                52, IPTR,               dest,    a0,   uint8,                  fillch,  d0, int32,                 len,     d1);
      FUNC2(int32,                  Stricmp,               53, const char*,         s1,      a0,   const char*,            s2,      a1);
      FUNC3(int32,                  Strlcpy,               54, char*,               dst,     a0,   const char*,            src,     a1, int32,                 len,     d0);
      FUNC3(int32,                  Strlcat,               55, char*,               dst,     a0,   const char*,            src,     a1, int32,                 len,     d0);
      FUNC3(int32,                  Strnicmp,              56, const char*,         s1,      a0,   const char*,            s2,      a1, int32,                 len,     d0);
      FUNC1(uint32,                 ToLower,               57, uint8,               chr,     d0);
      FUNC1(uint32,                 ToUpper,               58, uint8,               chr,     d0);
      FUNC2(const char*,            VASPrintf,             59, const char*,         fmt,     a0,   IPTR,                  args,    a1);
      PROC0(                        dummy3,                60);
      FUNC4(const char*,            VSSPrintf,             59, char*,               buffer,  a0,   int32,                  buflen,  d0, const char*,           fmt,     a1,   IPTR,               args,    a2);
      PROC0(                        dummy4,                60);
#else
      FUNC2(struct TagItem*,        FindTagItem,            5, Tag,                 value,   d0,   const struct TagItem*,  list,    a0);
      FUNC3(IPTR,                 GetTagData,             6, Tag,                 value,   d0,   IPTR,                 deflt,   d1, const struct TagItem*, list,    a0);
      FUNC3(uint32,                 PackBoolTags,           7, uint32,              initflg, d0,   struct TagItem*,        list,    a0, const struct TagItem*, boolmap, a1);
      FUNC1(struct TagItem*,        NextTagItem,            8, struct TagItem**,    tags,    a0);
      PROC3(                        FilterTagChanges,       9, struct TagItem*,     changes, a0,   struct TagItem*,        orig,    a1, uint32,                apply,   d0);
      PROC3(                        MapTags,               10, struct TagItem*,     list,    a0,   struct TagItem*,        maplist, a1, uint32,                maptype, d0);
      FUNC1(struct TagItem*,        AllocateTagItems,      11, uint32,              numTags, d0);
      FUNC1(struct TagItem*,        CloneTagItems,         12, struct TagItem*,     list,    a0);
      PROC1(                        FreeTagItems,          13, struct TagItem*,     list,    a0);
      PROC2(                        RefreshTagItemClones,  14, struct TagItem*,     clone,   a0,   struct TagItem*,        orig,    a1);
      FUNC2(bool,                   TagInArray,            15, Tag,                 value,   d0,   const Tag*,             array,   a0);
      FUNC3(uint32,                 FilterTagItems,        16, struct TagItem*,     list,    a0,   Tag*,                   farray,  a1, uint32,                logic,   d0);
      FUNC3(IPTR,                 CallHookPkt,           17, const struct Hook*,  hook,    a0,   IPTR,                  param1,  a2, IPTR,                 param2,  a1);
      PROC2(                        Amiga2Date,            20, uint32,              amidate, d0,   struct ClockData*,      data,    a0);
      FUNC1(uint32,                 Date2Amiga,            21, struct ClockData*,   date,    a0);
      FUNC1(uint32,                 CheckDate,             22, struct ClockData*,   date,    a0);
      FUNC2(int32,                  SMult32,               23, int32,               arg1,    d0,   int32,                  arg2,    d1);
      FUNC2(uint32,                 UMult32,               24, uint32,              arg1,    d0,   uint32,                 arg2,    d1);
      FUNC2(int32,                  SDivMod32,             25, int32,               divident,d0,   int32,                  divisor, d1);
      FUNC2(uint32,                 UDivMod32,             26, uint32,              divident,d0,   uint32,                 divisor, d1);
      FUNC2(int32,                  Stricmp,               27, const char*,         s1,      a0,   const char*,            s2,      a1);
      FUNC2(int32,                  Strnicmp,              28, const char*,         s1,      a0,   const char*,            s2,      a1);
      FUNC1(uint32,                 ToUpper,               29, uint8,               chr,     d0);
      FUNC1(uint32,                 ToLower,               30, uint8,               chr,     d0);
      PROC2(                        ApplyTagChanges,       31, struct TagItem*,     list,    a0,   const struct TagItem*,  changes, a1);
      FUNC2(int32,                  SMult64,               33, int32,               arg1,    d0,   int32,                  arg2,    d1);
      FUNC2(uint32,                 UMult64,               34, uint32,              arg1,    d0,   uint32,                 arg2,    d1);
      FUNC3(uint32,                 PackStructureTags,     35, uint8*,              pack,    a0,   uint32*,                packtab, a1, struct TagItem*,       list,    a2);
      FUNC3(uint32,                 UnpackStructureTags,   36, uint8*,              pack,    a0,   uint32*,                packtab, a1, struct TagItem*,       list,    a2);
      FUNC2(bool,                   AddNamedObject,        37, struct NamedObject*, namespc, a0,   struct NamedObject*,    object,  a1);
      FUNC2(struct NamedObject*,    AllocNamedObjectA,     38, const char*,         name,    a0,   const struct TagItem*,  tags,    a1);
      FUNC1(int32,                  AttemptRemNamedObject, 39, struct NamedObject*, object,  a0);
      FUNC3(struct NamedObject*,    FindNamedObject,       40, struct NamedObject*, namespc, a0,   const char*,            name,    a1, struct NamedObject*,   prev,    a2);
      FUNC1(struct NamedObject*,    FreeNamedObject,       41, struct NamedObject*, object,  a0);
      FUNC1(const char*,            NamedObjectName,       42, struct NamedObject*, object,  a0);
      PROC1(                        ReleaseNamedObject,    43, struct NamedObject*, object,  a0);
      PROC2(                        RemNamedObject,        44, struct NamedObject*, object,  a0,   struct Message*,        msg,     a1);
      FUNC0(uint32,                 GetUniqueID,           45);
#endif

   ENDDECL


#endif /*_UTILITYIFACE_H_*/
