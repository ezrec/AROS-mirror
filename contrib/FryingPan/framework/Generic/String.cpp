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

#include "String.h"
#include "LibrarySpool.h"
#include <libclass/exec.h>
#include <libclass/dos.h>
#include <libclass/utility.h>
#include <LibC/LibC.h>
#include "Debug.h"

using namespace GenNS;

#if defined(mc68000) && !defined(__AROS__)
   static unsigned long ___vFormatFunc[] = 
      { 0x16c04e75, 0x4e754e75 };
#endif

struct RawDoFmtStruct
{
   char  *buff;
   int    currlen;
   int    maxlen;
};



void String::Update()
{
   lLength = strlen((const char*)sContents);
}

void String::SetLength(unsigned long lLen)
{
   if (lLen >= lMaxLen)
      ReallocBuf(lLen + 1);
   if (lLen >= lMaxLen)
      lLen = lMaxLen - 1;
      
   lLength = lLen;
   sContents[lLen] = 0;
}

String::String() 
{
   lMaxLen   = 0;
   lLength   = 0;
   sContents = 0;
   Assign((char*)0);
};

String::String(const char* s) 
{
   lMaxLen   = 0;
   lLength   = 0;
   sContents = 0;
   Assign(s);
};

String::String(const String &s) 
{
   lMaxLen   = 0;
   lLength   = 0;
   sContents = 0;
   Assign(s.Data());
};

String::~String()
{
   if (sContents) 
      delete sContents;
};

const char* String::Data(void) const
{
   if (!sContents) return "";
   return (char*)sContents;
};

void String::Clone(const char* sSrc, unsigned int lLen)
{
   if (lLen >= lMaxLen)                // not enough space?
      AllocBuf(lLen + 1);              // make enough.
   if (lLen >= lMaxLen)                // failed?
      return;                          // return immediately.
   if (lLen > 0)                       // if there is something to copy,
      memcpy(sContents, sSrc, lLen);   // literally CLONE memory
   lLength = lLen;                     // update length ;)
   sContents[lLen] = 0;                // pad with zeros.
}

void String::Assign(const char *s)
{
//   VPrintf("Assigning %s (%lx)\n", ARRAY((int)s));
   if (0 == s)                         // string not passed?
      Clone(0, 0);                     // create an empty string (NO REALLOC)
   else                                // otherwise
      Clone(s, strlen(s));             // clone what we have.
//   VPrintf("Assigned %s (%lx)\n", ARRAY((int)sContents));
};

void String::Assign(const String *s)
{
   if (0 == s)                         // if no string has been passed
      Clone(0, 0);                     // create empty one
   else                                // otherwise
      Clone(s->Data(), s->Length());   // use what we have.
};

int String::Length() const
{
   return lLength;                     // this one is easy.
};

String::operator char*(void) const
{
   return (char *)Data();                      // this one is easy, too
};

String::operator unsigned char*(void) const
{
   return (unsigned char*)Data();      // humm
};
 
String &String::operator = (const String & sStr)
{
   Assign(const_cast<String*>(&sStr)); // okay...
   return *this;                       //
}

String &String::operator = (const int64 sVal)
{
   if ((sVal & 0xffffffff) == sVal)
   {
      FormatStr("%ld", ARRAY((ULONG)sVal));      // aint hard i guess..
   }
   else
   {
      FormatStr("0x%lx%08lx", ARRAY((ULONG)((sVal >> 32) & 0xffffffff), (ULONG)(sVal & 0xffffffff)));
   }
   return *this;                       // is it..
}

int String::operator < (const String sStr) const
{
   return 0 > strncmp(Data(), sStr.Data(), Length());   // humm..
}

int String::operator == (const String sStr) const
{
   return 0 == strncmp(Data(), sStr.Data(), Length());   // humm..
}

int String::operator < (const char* sStr) const
{
   return 0 > strncmp(Data(), sStr, Length());          // compare only what we are aware of.
}

int String::operator == (const char* sStr)  const
{
   return 0 == strncmp(Data(), sStr, Length());          // compare only what we are aware of.
}

int String::FormatStr(const char *sFmtStr, IPTR pParams)
{
   char *temp = new char[65535];
   ASSERT(Exec != 0);
#if defined(__amiga__) & defined(mc68000)
   Exec->RawDoFmt(sFmtStr, pParams, &___vFormatFunc, temp);
#elif defined(__AMIGAOS4__)
   Exec->RawDoFmt(sFmtStr, pParams, 0, temp);
   //VSNPrintf((uint8*)temp, 65535, (uint8*)sFmtStr, pParams);
#elif !defined(__LINUX__)
   Exec->RawDoFmt(sFmtStr, (void*)pParams, 0, temp);
#else
   vsprintf(temp, sFmtStr, (va_list)pParams);
#endif
   Assign(temp);
   delete [] temp;
   return 0;
}

void String::AddPath(const char* sElement)
{
   if ((strlen(sElement) + lLength) >= (lMaxLen-4))    // if you need,
      ReallocBuf(strlen(sElement) + lLength + 5);  // get more mem
   if ((strlen(sElement) + lLength) >= (lMaxLen-4))    // if we failed
      return;                                      // abort.
   DOS->AddPart((char*)sContents, sElement, lMaxLen);          // otherwise update
   Update();
}

void String::AllocBuf(unsigned int lSize)
{
   unsigned char *sOld;
   
   if (lSize <= lMaxLen)                  // dont care if we dont have to
      return;                             // :)
   
   sOld        = sContents;               // backup old stuff. 
   sContents   = new unsigned char[lSize+1];

   if (0 != sContents)                    // if everything went smooth
   {                                      //
      if (0 != sOld)                      //
         delete [] sOld;
      lMaxLen = lSize + 1;                // and update the size.
   }
   else
   {
      request("Low memory error", "Unable to allocate %ld bytes of memory", "Continue", ARRAY(lSize));
      sContents = sOld;                   // revert changes, 
   }
   sContents[0] = 0;                      // also update the length
   lLength = 0;                           // etc...
}

void String::ReallocBuf(unsigned int lSize)
{
   unsigned char *sOld;
   
   if (lSize <= lMaxLen)                  // dont care if we dont have to
      return;                             // :)
   
   sOld        = sContents;               // backup old stuff. 
   sContents   = new unsigned char[lSize + 1]; // get fresh piece of mem :P

   if (0 != sContents)                    // if everything went smooth
   {                                      //
      if (0 != sOld)                      //
      {
         memcpy(sContents, sOld, lLength);// copy the whole old block of memory :)
         delete [] sOld;
      }
      else
      {
         lLength = 0;
      }
      lMaxLen = lSize + 1;                // and update the size.
   }
   else
   {
      request("Low memory error", "Unable to allocate %ld bytes of memory", "Continue", ARRAY(lSize));
      sContents = sOld;                   // revert changes, 
   }
   sContents[lLength] = 0;                // make sure no garbage follows.
}

String String::operator + (const char *sStr)
{
   String s(*this);                       // this should be easy
   s += sStr;                             // i hope..
   return s;                              // :P
}

String String::operator + (const String & sStr)
{
   String s(*this);                       // same here.
   s += sStr.Data();                      // hope i dont screw up
   return s;                              // or else bad things may happen
}

String &String::operator += (const char *sStr)
{
   if ((strlen(sStr)+lLength) >= lMaxLen) // if you need
      ReallocBuf(strlen(sStr)+lLength+1); // get more mem
   if ((strlen(sStr)+lLength) >= lMaxLen) // if you fail
      return *this;                       // do not proceed.

   strncat((char*)sContents, sStr, lMaxLen);     // update as much as we can take.
   Update();
   return *this;
}

String &String::operator += (const char cChar)
{
   if ((1+lLength) >= lMaxLen)            // if you need
      ReallocBuf(lLength+16);             // get more mem
   if ((1+lLength) >= lMaxLen)            // if you fail
      return *this;                       // do not proceed.

   sContents[lLength++] = cChar;
   sContents[lLength] = 0;
   Update();
   return *this;
}

unsigned long String::TrimChars(const char* sChars)
{
   int lNewStart = 0;
   int lNewEnd   = Length();
   int lInPos;

   
   for (lInPos = 0; sChars[lInPos]; ++lInPos)
   {
      if (sContents[lNewStart] == sChars[lInPos])
      {
         ++lNewStart;
         if (lNewStart >= lNewEnd)
            break;
         lInPos = -1;
      }
   }

   for (lInPos = 0; sChars[lInPos]; ++lInPos)
   {
      if (sContents[lNewEnd-1] == sChars[lInPos])
      {
         --lNewEnd; 
         if (lNewEnd == 0)
            break;
         sContents[lNewEnd] = 0; 
         lInPos = -1;
      }
   }

   String sTemp((char*)&sContents[lNewStart]);
   *this = sTemp;      
   return Length();
}

char &String::operator [] (int lOffset)
{
   ASSERT(lOffset >= 0);
   ASSERT(lOffset <= (int)lMaxLen);
   return (char&)sContents[lOffset];
}

String String::SubString(int lFirst, int lLen)
{
   String   s;
   int      pos;
   
   if (lLen == -1)
   {
      lLen = Length() - lFirst;
   }

   s.AllocBuf(lLen+1);
   for (pos=0; pos<lLen; pos++)
   {
      if (!sContents[lFirst+pos])
         break;
      s[pos] = sContents[lFirst+pos];
   }
   s[pos] = 0;
   s.Update();
   return s;
}

String String::LeftString(int lLen)
{
   return SubString(0, lLen);
}

String String::RightString(int lLen)
{
   return SubString (Length()-lLen, lLen);
}

bool String::Equals(const char* sOther)
{
   return (0 == strcmp((char*)sContents, sOther));
}

bool String::EqualsIgnoreCase(const char* sOther)
{
   return (0 == stricmp((char*)sContents, sOther));
}

int String::Compare(const char* sOther) const
{
   return (strcmp((char*)sContents, sOther));
}

int String::CompareIgnoreCase(const char* sOther) const
{
   return (stricmp((char*)sContents, sOther));
}

int32 String::ToLong()
{
   enum eType
   {
      E_Dec =  0,
      E_Bin,
      E_Oct,
      E_Hex
   };

   char  c;   
   int32 lValue = 0;
   eType lType = E_Dec;

   for (unsigned int i=0; sContents[i]; i++)
   {
      if (i >= lLength) 
         break;
      c = sContents[i];
      if ((lValue == 0) && (i == 1))
      {
         if (c == 'x')
            lType = E_Hex;
         else if (c == 'b')
            lType = E_Bin;
         else
            lType = E_Oct;
      }
      
      switch (lType)
      {
         case E_Dec:
            if ((c < '0') || (c > '9'))
               return lValue;
            lValue *= 10;
            lValue += (c-'0');
            break;
         case E_Bin:
            if ((c < '0') || (c > '1'))
               return lValue;
            lValue <<= 1;
            lValue += (c-'0');
            break;
         case E_Oct:
            if ((c < '0') || (c > '7'))
               return lValue;
            lValue <<= 3;
            lValue += (c-'0');
            break;
         case E_Hex:
            if (((c < '0') || (c > '9')) &&
                ((c < 'a') || (c > 'f')) &&
                ((c < 'A') || (c > 'F')))
               return lValue;

            if (c < '9')
               c -= '0';
            else if (c < 'F')
               c -= ('A'-10);
            else 
               c -= ('a'-10);

            lValue <<= 4;
            lValue += c;
            break;
      };      
   }
   return lValue;
}

int64 String::ToQuad()
{
   enum eType
   {
      E_Dec =  0,
      E_Bin,
      E_Oct,
      E_Hex
   };

   char  c;   
   int64 lValue = 0;
   eType lType = E_Dec;

   for (unsigned int i=0; sContents[i]; i++)
   {
      if (i >= lLength) 
         break;
      c = sContents[i];
      if ((lValue == 0) && (i == 1))
      {
         if (c == 'x')
            lType = E_Hex;
         else if (c == 'b')
            lType = E_Bin;
         else
            lType = E_Oct;
      }
      
      switch (lType)
      {
         case E_Dec:
            if ((c < '0') || (c > '9'))
               return lValue;
            lValue *= 10;
            lValue += (c-'0');
            break;
         case E_Bin:
            if ((c < '0') || (c > '1'))
               return lValue;
            lValue <<= 1;
            lValue += (c-'0');
            break;
         case E_Oct:
            if ((c < '0') || (c > '7'))
               return lValue;
            lValue <<= 3;
            lValue += (c-'0');
            break;
         case E_Hex:
            if (((c < '0') || (c > '9')) &&
                ((c < 'a') || (c > 'f')) &&
                ((c < 'A') || (c > 'F')))
               return lValue;

            if (c < '9')
               c -= '0';
            else if (c < 'F')
               c -= ('A'-10);
            else 
               c -= ('a'-10);

            lValue <<= 4;
            lValue += c;
            break;
      };      
   }
   return lValue;
}

void String::StrLCpy(const char* sSrc, int lNum)
{
   if (lNum >= (int)lMaxLen)
      ReallocBuf(lNum+2);

   int i = 0;

   if (sSrc)                                          //
   {                                                  //
      for (i=0; 0 != sSrc[i]; ++i)                    // while we have sth to copy
      {                                               //
         if (i >= lNum)                               // that will be tough. STOP ASAP
            break;                                    // 
         sContents[i] = sSrc[i];                      // copy while we have sth to copy
      }
   }
   
   while (i < (int)lMaxLen)                           // ERASE TO THE END
   {                                                  // 
      sContents[i] = 0;                               //
      i++;                                            // ...
   }                                                  //

   Update();
}
   
void String::BstrCpy(BSTR src)
{
#ifndef __AROS__   
   StrLCpy(&((char*)((IPTR)src<<2))[1], ((char*)((IPTR)src<<2))[0]);
#else
   *this = (char*)src;
#endif
}

String &String::Substitute(const char* src, const char* dst)
{
   int32 sl = strlen(src);
   int32 l = 2048;
   int32 i, p=0;
   char *d = new char[l];
   
   for (i=0; sContents[i] != 0;)
   {
      if (sContents[i] == src[0])
      {
         if (strncmp((char*)&sContents[i], src, sl) == 0)
         {
            for (int j=0; dst[j] != 0; j++)
           {
               d[p++] = dst[j];
               if ((p+2) >= l)
               {
                  char *sd = new char[l<<1];
                  memcpy(sd, d, l);
                  l <<= 1;
                  delete [] d;
                  d = sd;
               }
            }
            i+= sl;
            continue;
         }
      }
      d[p++] = sContents[i++];
      if ((p+2) >= l)
      {
         char *sd = new char[l<<1];
         memcpy(sd, d, l);
         l <<= 1;
         delete [] d;
         d = sd;
      }
   } 

   d[p] = 0;
   Assign(d);
   delete [] d;
   return *this;
}

String &String::ToUTF8()
{
   char *d = new char[(Length()<<1)+1];
   int j=0;

   for (int i=0; ((sContents[i] != 0) && (i<Length())); i++)
   {
      if (((uint8)sContents[i]) < 0x80)
      {
         d[j++] = sContents[i];
      }
      else
      {
         d[j++] = 0xc0+(sContents[i] >> 6 & 3);
         d[j++] = 0x80+(sContents[i] & 0x3f);
      }
   }
   d[j] = 0;

   Assign(d);
   delete [] d;

   return *this;
}

String &String::FromUTF8()
{
   char *d = new char[Length()+1];
   int j=0;

   for (int i=0; ((sContents[i] != 0) && (i<Length())); i++)
   {
      if (((uint8)sContents[i]) < 0x80)
      {
         d[j++] = sContents[i];
      }
      else if (((uint8)sContents[i]) <0xdf)
      {
         d[j]    = (sContents[i++] & 3) << 6;
         d[j++] |= (sContents[i] & 0x3f);
      }
      else
      {
         d[j++] = '?';
      }
   }
   d[j] = 0;

   Assign(d);
   delete [] d;

   return *this;
}

// static functions & stuff...
/*
void String::strcat(char* pDst, const char* pSrc)
{
   if (0 == pDst)
      return;
   while (*pDst)  
   {
      pDst++;
   }
   strcpy(pDst, pSrc);
}

void String::strncat(char* pDst, const char* pSrc, long lLen)
{
   if (0 == pDst)                                     // no stirng?
      return;                                         // abort
   while (*pDst)                                      // 
   {
      ++pDst;                                         // find end of string
      --lLen;                                         // and reduce amount of bytes we can take
      if (0 >= lLen)                                  // if we reached our bottom limit
         return;                                      // abort.
   }
   strncpy(pDst, pSrc, lLen);                         // copy the remaining bytes.
}
*/
VectorT<String> String::Explode()
{
   VectorT<String> vec;
   register int    s;
   register int    e;

   vec.Empty();

   for (s = 0; s<Length();)
   {
      register unsigned char c = 0;
     
      /*
       * skip spaces, tabs, hard spaces
       */ 
      while (s<Length())
      {
         c = sContents[s];
         if ((c != ' ') &&
             (c != '\t') &&
             (c != 0xa0))
            break;
         s++; 
      }
      
      /*
       * ignore end of line etc
       */
      if ((s == Length()) || (c == 10) || (c == 13) || (c == 0))
         break;

      e = 0;

      if (c == '"')
      {
         s++;
         while ((s+e) < Length())
         {
            c = sContents[s+e];
            if ((c == 10) || (c == 13) || (c == '"') || (c == 0))
               break;
            e++;
         }
         if (c == '"')
         {
            vec << SubString(s, e);
            e++;
         }
      }
      else
      {
         while ((s+e) < Length())
         {
            c = sContents[s+e];
            if ((c == 10) || (c == 13) || (c == ' ') || (c == '\t') || (c == 160) || (c == 0))
               break;
            e++;
         }
         vec << SubString(s, e);
      }
   
      s = s+e+1;
   }

   return vec; 
}

String &String::UpperCase()
{
   ASSERT(Utility != 0);
   for (int i=0; i<Length(); i++)
   {
      sContents[i] = Utility->ToUpper(sContents[i]);
   }
   Update();
   return *this;
}

String &String::LowerCase()
{
   ASSERT(Utility != 0);
   for (int i=0; i<Length(); i++)
   {
      sContents[i] = Utility->ToLower(sContents[i]);
   }
   Update();
   return *this;
}

// global operators

String GenNS::operator + (const char *sStr1, const String sStr2)
{
   String sub(sStr1);
   sub += sStr2.Data();
   return sub;
}

