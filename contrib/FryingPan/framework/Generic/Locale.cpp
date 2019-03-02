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

#include "Locale.h"
#include <libclass/locale.h>
#include "Types.h"
#include <libraries/locale.h>
#include <libclass/dos.h>
#include <dos/dos.h>



GenNS::Localization::Localization(Localization::LocaleSet set[], const char* grp) :
   deflt("*** Undefined ***", "NO_GROUP", "NO_ID"),
   hash(deflt)
{
   locale = LocaleIFace::GetInstance(0);
   lang = 0;

   if (locale != 0)
      lang = locale->OpenLocale(0);

   if (set != 0)
      Add(set, grp);
}

GenNS::Localization::~Localization()
{
   if (lang != 0)
      locale->CloseLocale(lang);

   locale->FreeInstance();
}

void GenNS::Localization::Add(GenNS::Localization::LocaleSet set[], const char* grp)
{
   int i = 0;
   if (set == 0)
      return;

   locale_set ls("", grp, 0);

   while (set[i].key != (IPTR)GenNS::Localization::LocaleSet::Set_Last)
   {
      ls.id  = set[i].locale_id;
      split(set[i].value, ls.accel[0], ls.str);
      //request("Info", "%s = %s + %s", "Ok", ARRAY((uint)set[i].value, (uint)&ls.accel, (uint)ls.str.Data()));
      hash.Add(set[i].key, ls);
      ++i;
   }
}

const GenNS::String& GenNS::Localization::operator[] (uint val)
{
   return hash.GetVal(val).str;
}

bool GenNS::Localization::ReadCatalog(const char* name, sint version)
{
   LocaleIFace *locale = LocaleIFace::GetInstance(0);
   bool res = false;

   if (locale != 0)
   {
      Catalog *cat = locale->OpenCatalogA(0, name, TAGARRAY(OC_Version, static_cast<IPTR>(version), TAG_DONE, 0));
      if (cat != 0)
      {
         res = true;
         for (int i=0; i<hash.Count(); i++)
         {
            IPTR k = hash.GetKey(i);
            const char *t = locale->GetCatalogStr(cat, k, 0);
            if (t == 0)
               continue;
            locale_set &set = const_cast<locale_set&>(hash.GetVal(k));
            split(t, set.accel[0], set.str);
         }
         locale->CloseCatalog(cat);
      }      
      locale->FreeInstance();
   }

   return res;
}
      
const GenNS::String& GenNS::Localization::Str(IPTR key)
{
   return (*this)[key];
}

bool GenNS::Localization::ExportCD(const char* filename, sint version)
{
   ASSERT(DOS != 0);
   if (DOS == 0)
      return false;

   BPTR fh = DOS->Open(filename, MODE_NEWFILE);

   if (fh != 0)
   {
      String s;
      DOS->VFPrintf(fh, ";#version %ld\n", (void *)ARRAY(static_cast<IPTR>(version)));
      DOS->VFPrintf(fh, ";#language english\n", 0);
      DOS->VFPrintf(fh, ";\n", 0);
      for (int i=0; i<hash.Count(); i++)
      {
         IPTR key = hash.GetKey(i);
         const char* g;
         const locale_set &set = hash.GetVal(key);
         
         if (set.group == 0)
            g = "GLOBAL";
         else
            g = set.group;

         if (set.id == 0)
            s.FormatStr("%s_%ld", ARRAY((IPTR)g, key));
         else
            s.FormatStr("%s_%s", ARRAY((IPTR)g, (IPTR)set.id));

         DOS->VFPrintf(fh, "%s (%ld//)\n", (void *)ARRAY((IPTR)s.Data(), key));
         s = set.str;
         s.Substitute("\n", "\\n");
         s.Substitute("\033", "\\033");
         if (set.accel[0] == 0)
            DOS->VFPrintf(fh, "%s\n", (void *)ARRAY((IPTR)s.Data()));
         else
            DOS->VFPrintf(fh, "%lc&%s\n", (void *)ARRAY((IPTR)set.accel[0], (IPTR)s.Data()));
         DOS->VFPrintf(fh, ";\n", 0);
      }

      DOS->Close(fh);
      return true;
   }
   return false;
}

bool GenNS::Localization::ExportCT(const char* filename, sint version)
{
   ASSERT(DOS != 0);
   if (DOS == 0)
      return false;

   BPTR fh = DOS->Open(filename, MODE_NEWFILE);

   if (fh != 0)
   {
      String s;
      DOS->VFPrintf(fh, "##version %s %ld.0\n", (void *)ARRAY((IPTR)filename, static_cast<IPTR>(version)));
      DOS->VFPrintf(fh, "##language english\n", 0);
      DOS->VFPrintf(fh, ";\n", 0);
      for (int i=0; i<hash.Count(); i++)
      {
         IPTR key = hash.GetKey(i);
         const char* g;
         const locale_set &set = hash.GetVal(key);
         
         if (set.group == 0)
            g = "GLOBAL";
         else
            g = set.group;

         if (set.id == 0)
            s.FormatStr("%s_%ld", ARRAY((IPTR)g, key));
         else
            s.FormatStr("%s_%s", ARRAY((IPTR)g, (IPTR)set.id));

         DOS->VFPrintf(fh, "%s\n", (void *)ARRAY((IPTR)s.Data()));
         s = set.str;
         s.Substitute("\n", "\\n");
         s.Substitute("\033", "\\033");
         if (set.accel[0] == 0)
            DOS->VFPrintf(fh, "%s\n", (void *)ARRAY((IPTR)s.Data()));
         else
            DOS->VFPrintf(fh, "%lc&%s\n", (void *)ARRAY((IPTR)set.accel[0], (IPTR)s.Data()));
         DOS->VFPrintf(fh, ";\n", 0);
      }

      DOS->Close(fh);
      return true;
   }
   return false;
}
      
void GenNS::Localization::split(GenNS::String s, char &accelerator, GenNS::String &text)
{
   if (s[1] == '&')
   {
      accelerator = s[0];
      text = s.SubString(2, -1);
   }
   else
   {
      accelerator = '\0';
      text = s;
   }
}

const char GenNS::Localization::Accel(IPTR key)
{
   //request("Info", "accelerator: %s", "Ok", ARRAY((uint)hash.GetVal(key).accel[0]));
   return hash.GetVal(key).accel[0];
}

const char* GenNS::Localization::Shortcut(IPTR key)
{
   //request("Info", "shortcut: %s", "Ok", ARRAY((uint)&hash.GetVal(key).accel));
   return (const char*)(&hash.GetVal(key).accel);
}
      
GenNS::String GenNS::Localization::FormatNumber(sint integer, sint millionth)
{
   char temp[32];
  
   uint8 grp[] = { 3, 0 }; 
   const char* decp = lang ? lang->loc_DecimalPoint : ".";
   const char* gsep = lang ? lang->loc_GroupSeparator : " ";
   const char* fsep = lang ? lang->loc_FracGroupSeparator : " ";
   const uint8* ggrp = lang ? lang->loc_Grouping : (uint8*)&grp;
   const uint8* fgrp = lang ? lang->loc_FracGrouping : (uint8*)&grp;
   int gpos = 0;
   int part = 0;
   bool sign = false;
   uint pos = sizeof(temp);

   if (integer < 0)
   {
      sign = true;
      integer = -integer;
   }

   do 
   {
      --pos;
      /* 
       * handle group
       */
      if (gpos == *ggrp)
      {
         gpos = 0;
         ggrp++;
         if (!*ggrp)
            ggrp--;
         if (*gsep)
            temp[pos--] = *gsep;
      }
      gpos++;

      part = integer % 10;
      integer /= 10;
      temp[pos] = part + '0';
   } 
   while (integer != 0);

   if (sign)
      temp[--pos] = '-';

   /*
    * ok we have integer part parsed now
    */
   for (integer=0; pos<sizeof(temp); integer++, pos++)
      temp[integer] = temp[pos];
   pos = integer;

   /*
    * reverse digit order ;]
    * make life a little easier, too
    */
   integer = 0;
   for (int i=0; i<6; i++)
   {
      part = millionth % 10;
      integer <<= 4;
      integer |= part;
      millionth /= 10;
   }

   if (0 != integer)
   {
      temp[pos++] = *decp;
      gpos = 0;

      while (integer != 0)
      {
         if (gpos == *fgrp)
         {
            gpos = 0;
            fgrp++;
            if (!*fgrp)
               fgrp--;
            if (*fsep)                 // handle broken locales somehow
               temp[pos++] = *fsep;
         }
         gpos++;

         part = integer & 0xf;
         integer >>= 4;
         temp[pos++] = part + '0';
      }


   }

   temp[pos++] = 0;

   return String(temp);
}
