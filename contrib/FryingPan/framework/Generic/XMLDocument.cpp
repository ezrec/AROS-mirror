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

#include "XMLDocument.h"

#include <libclass/expat.h>
#include <libdata/expat.h>
#include <libclass/dos.h>
#include <dos/dos.h>
#include <LibC/LibC.h>

using namespace GenNS;

XMLAttribute::XMLAttribute()
{
   Initialize("", "");
}

XMLAttribute::XMLAttribute(const char* sName, const char* sValue)
{
   Initialize(sName, sValue);
}

XMLAttribute::XMLAttribute(const char* sName, int64 sValue)
{
   Initialize(sName, sValue);
}

XMLAttribute::~XMLAttribute()
{
}

void XMLAttribute::Initialize(const char* sN, const char* sV)
{
   sName = sN;
   sValue = sV;
};

void XMLAttribute::Initialize(const char* sN, int64 sV)
{
   sName = sN;
   sValue = sV;
};

String &XMLAttribute::GetName() 
{
   return sName;
}

String &XMLAttribute::GetValue() 
{
   return sValue;
}

void XMLAttribute::SetName(const char* sName)
{
   this->sName = sName;
}

void XMLAttribute::SetValue(const char* sValue)
{
   this->sValue = sValue;
}

void XMLAttribute::SetValue(uint64 sValue)
{
   this->sValue = sValue;
}


XMLElement::XMLElement()
{
   Initialize("", "");
}

XMLElement::XMLElement(const char* sName, const char* sValue)
{
   Initialize(sName, sValue);
}

XMLElement::~XMLElement()
{
   hAttributes.ForEach(&XMLElement::freeAttr);
   hChildren.ForEach(&XMLElement::freeNode);
}

bool XMLElement::freeAttr(XMLAttribute* const& attr)
{
   delete attr;
   return true;
}

bool XMLElement::freeNode(XMLElement* const& node)
{
   delete node;
   return true;
}

void XMLElement::Initialize(const char* sN, const char* sV)
{
   XMLAttribute::Initialize(sN, sV);
   hChildren.Empty();   // ensure its empty
   hAttributes.Empty(); // !!
}

long XMLElement::GetChildrenCount()
{
   return hChildren.Count();
}

XMLElement *XMLElement::GetChild(long i)
{
   return hChildren[i];
}

long XMLElement::GetAttributesCount()
{
   return hAttributes.Count();
}

XMLAttribute *XMLElement::GetAttribute(long i)
{
   return hAttributes[i];
}

XMLElement *XMLElement::AddChild(const char* sName, const char* sValue)
{
   XMLElement *pNew = new XMLElement(sName, sValue);
   hChildren << pNew;
   return pNew;
}

XMLAttribute  *XMLElement::AddAttribute(const char* sName, const char* sValue)
{
   XMLAttribute *pNew = new XMLAttribute(sName, sValue);
   hAttributes << pNew;
   return pNew;
}

XMLAttribute *XMLElement::AddAttribute(const char* sNm, int64 lVal)
{
   XMLAttribute *pNew = new XMLAttribute(sNm, lVal);
   hAttributes << pNew;
   return pNew;
}

XMLElement *XMLElement::AddChild(XMLElement* pElem)
{
   hChildren << pElem;
   return pElem;
}

XMLAttribute *XMLElement::AddAttribute(XMLAttribute *pAttr)
{
   hAttributes << pAttr;
   return pAttr;
}

void XMLElement::RemChild(XMLElement* pElem)
{
   hChildren >> pElem;
}

void XMLElement::RemAttribute(XMLAttribute* pAttr)
{
   hAttributes >> pAttr;
}

XMLElement *XMLElement::FindChild(const char* sName)
{
   for (int i=0; i<hChildren.Count(); i++)
   {
      if (hChildren[i]->GetName().Equals(sName))
         return hChildren[i];
   }
   return 0;
}

XMLAttribute *XMLElement::FindAttribute(const char* sName)
{
   for (int i=0; i<hAttributes.Count(); i++)
   {
      if (hAttributes[i]->GetName().Equals(sName))
         return hAttributes[i];
   }
   return 0;
}

String *XMLElement::GetChildValue(const char *sName)
{
   XMLElement *pElem = FindChild(sName);
   if (0 == pElem)
      return 0;
   return &pElem->GetValue();
}

String *XMLElement::GetAttributeValue(const char *sName)
{
   XMLAttribute *pAttr = FindAttribute(sName);
   if (0 == pAttr)
      return 0;
   return &pAttr->GetValue();
}



XMLDocument::XMLDocument()
{
   pParser     = 0;
   eError      = XErr_OK;
   sFileName   = "";
   pExpat      = Expat2IFace::GetInstance(0);
}

XMLDocument::XMLDocument(const char *sName)
   : XMLElement(sName, "")
{
   pParser     = 0;
   eError      = XErr_OK;
   sFileName   = "";
   pExpat      = Expat2IFace::GetInstance(0);
}

XMLDocument::~XMLDocument()
{
   if (pExpat)
   {
      if (pParser)
         pExpat->XML_ParserFree(pParser);
      pExpat->FreeInstance();
   }
}

void XMLDocument::fXMLStartHandler(void* userData, const XML_Char *name, const XML_Char **atts)
{
   ((XMLDocument*)userData)->XMLStartHandler(name, atts);
}

void XMLDocument::fXMLEndHandler(void* userData, const XML_Char *name)
{
   ((XMLDocument*)userData)->XMLEndHandler(name);
}

void XMLDocument::fXMLContentHandler(void* userData, const XML_Char *begin, int length)
{
   ((XMLDocument*)userData)->XMLContentHandler(begin, length);
}

void XMLDocument::XMLStartHandler(const XML_Char *name, const XML_Char **atts)
{
   XMLElement *pLast;
   
//   MessageBox("Info", "On Start: %s", "Ok", ARRAY((int)name));

   level = hParse.Count();
   
   if (0 != level)
   {
      pLast = hParse[level-1]->AddChild((char*)name, "");  
   }
   else
   {
      pLast = this;
      Initialize((char*)name, "");     // implies: free(!)
   }
   
   for (int i=0; 0 != atts[i]; i+=2)
   {
      String s((char*)(atts[i+1]));
      pLast->AddAttribute((char*)atts[i], s.FromUTF8().Data());
   }

   hParse << pLast;
   
//   MessageBox("Info", "Done Start: %s", "Ok", ARRAY((int)name));
}

void XMLDocument::XMLEndHandler(const XML_Char *name)
{
   XMLElement *pLast;
//   MessageBox("Info", "On End: %s", "Ok", ARRAY((int)name));
   if (hParse.Count() == 0)
   {
      eError = XErr_CorruptedXML;
      return;
   }
   pLast = hParse[hParse.Count()-1];

   String s = pLast->GetValue();
   s.FromUTF8();
   pLast->SetValue(s.Data());
   
   XMLElement *pElem = hParse[(hParse.Count() - 1)];
   hParse >> pElem;
//   MessageBox("Info", "Done End: %s", "Ok", ARRAY((int)name));
}

void XMLDocument::XMLContentHandler(const XML_Char *begin, int length)
{
   XMLElement *pLast;
   
//   MessageBox("Info", "On Content", "Ok", 0);
   
   if (hParse.Count() == 0)
   {
      eError = XErr_CorruptedXML;
      return;
   }
   pLast = hParse[hParse.Count()-1];   // get last element

   String s1;
   
   // 1. get current data
   s1 = pLast->GetValue();
   
   // 2. get new data
   char *pBuff = new char[length+1];
   pBuff[length] = 0;
   strncpy(pBuff, begin, length);
   
   // 3. concatenate two strings
   s1 += pBuff;
   delete [] pBuff;
   
   // 4. set new data
   pLast->SetValue(s1.Data());
//   MessageBox("Info", "Done Content", "Ok", 0);
}
 
bool XMLDocument::ReadXML(const char *sName)
{
   BPTR  fh = 0;
   void *mem;
   int   len;

   ASSERT(NULL != pExpat);
   if (0 == pExpat)
   {
      eError = XErr_NoExpat;
      return false;
   }

   sFileName = sName;
   ASSERT(sFileName.Length() != 0);
   if (sFileName.Length() == 0)
   {
      eError = XErr_NoFile;
      return false;
   }

   eError = XErr_OK;
   level  = 0;
   
   if (pParser)
      pExpat->XML_ParserFree(pParser);
      
   pParser = 0;
   
   fh = DOS->Open(sFileName.Data(), MODE_OLDFILE);
   if (fh == 0)
   {
      eError = XErr_NoFile;
      return false;
   }
   
   ASSERT(0 != fh);
   pParser = pExpat->XML_ParserCreate(0);
   ASSERT(0 != pParser);
   if (pParser == 0)
   {
      DOS->Close(fh);
      eError = XErr_NoParser;
      return false;
   }
   
   hParse.Empty();
   pExpat->XML_SetUserData(pParser, this);
   pExpat->XML_SetElementHandler(pParser, &XMLDocument::fXMLStartHandler, &XMLDocument::fXMLEndHandler);
   pExpat->XML_SetCharacterDataHandler(pParser, &XMLDocument::fXMLContentHandler);
   
   do
   {
      len = 0;
      mem = pExpat->XML_GetBuffer(pParser, 8192);
      if (mem)
      {
         len = DOS->Read(fh, mem, 8192);
         pExpat->XML_ParseBuffer(pParser, len, 0 == len);
      }
      else
      {
         eError = XErr_NoMemory;
         break;
      }
   }
   while (0 != len);
   
   DOS->Close(fh);
   pExpat->XML_ParserFree(pParser);
   pParser = 0;
   if (eError == XErr_OK)
      return true;
   return false;
}

bool XMLDocument::WriteXML(const char *sName)
{
   BPTR  fh = 0;

   sFileName = sName;
   if (sFileName.Length() == 0)
   {
      eError = XErr_NoFile;
      return false;
   }

   eError = XErr_OK;
   
   fh = DOS->Open(sFileName.Data(), MODE_NEWFILE);
   if (fh == 0)
   {
      eError = XErr_NoFile;
      return false;
   }

   DOS->FPuts(fh, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
   level = 0;
   XMLWriteElement(fh, this);
   
   DOS->Close(fh);
   if (eError == XErr_OK)
      return true;
   return false;
}

/* FIXME: polaczyc w jedno */

bool XMLDocument::ReadXML(BPTR fh)
{
   void *mem;
   int   len;

   ASSERT(NULL != pExpat);
   if (0 == pExpat)
   {
      eError = XErr_NoExpat;
      return false;
   }

   eError = XErr_OK;
   level  = 0;
   
   if (pParser)
      pExpat->XML_ParserFree(pParser);
      
   pParser = 0;
   
   if (fh == 0)
   {
      eError = XErr_NoFile;
      return false;
   }


   ASSERT(0 != fh);
   pParser = pExpat->XML_ParserCreate(0);
   ASSERT(0 != pParser);
   if (pParser == 0)
   {
      DOS->Close(fh);
      eError = XErr_NoParser;
      return false;
   }
   
   hParse.Empty();
   pExpat->XML_SetUserData(pParser, this);
   pExpat->XML_SetElementHandler(pParser, &XMLDocument::fXMLStartHandler, &XMLDocument::fXMLEndHandler);
   pExpat->XML_SetCharacterDataHandler(pParser, &XMLDocument::fXMLContentHandler);
   
   do
   {
      len = 0;
      mem = pExpat->XML_GetBuffer(pParser, 8192);
      if (mem)
      {
         len = DOS->Read(fh, mem, 8192);
         if (0 != pExpat->XML_ParseBuffer(pParser, len, 0 == len))
         {
            eError = XErr_CorruptedXML;
            break;
         }
      }
      else
      {
         eError = XErr_NoMemory;
         break;
      }
   }
   while (0 != len);
   
   pExpat->XML_ParserFree(pParser);
   pParser = 0;
   if (eError == XErr_OK)
      return true;
   return false;
}

bool XMLDocument::WriteXML(BPTR fh)
{
   eError = XErr_OK;
   
   if (fh == 0)
   {
      eError = XErr_NoFile;
      return false;
   }

   DOS->FPuts(fh, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
   level = 0;
   XMLWriteElement(fh, this);
   
   if (eError == XErr_OK)
      return true;
   return false;
}

void XMLDocument::XMLWriteElement(BPTR fh, XMLElement* pElem)
{
   for (int i=0; i<level; i++)
      DOS->FPuts(fh, "\t");
   DOS->FPuts(fh, "<");
   DOS->FPuts(fh, (char*)pElem->GetName());
   for (long i=0; i<pElem->GetAttributesCount(); i++)
   {
      XMLAttribute *pAttr = pElem->GetAttribute(i);
      String        val = pAttr->GetValue();
      
      val.Substitute("&", "&amp;");
      val.Substitute("'", "&apos;");
      val.Substitute("\"", "&quot;");
      val.Substitute("<", "&lt;");
      val.Substitute(">", "&gt;");

      DOS->FPuts(fh, " ");
      DOS->FPuts(fh, (char*)pAttr->GetName());
      DOS->FPuts(fh, "=\"");
      DOS->FPuts(fh, (char*)(val.ToUTF8()));
      DOS->FPuts(fh, "\"");
   }
   
   if (0 == pElem->GetChildrenCount())
   {
      if (0 != strlen(pElem->GetValue()))
      {
         String val = pElem->GetValue();
         // we pust the data.
         val.Substitute("&", "&amp;");
         val.Substitute("'", "&apos;");
         val.Substitute("\"", "&quot;");
         val.Substitute("<", "&lt;");
         val.Substitute(">", "&gt;");

         DOS->FPuts(fh, ">");
         DOS->FPuts(fh, (char*)(val.ToUTF8()));
      }
      else
      {
         // it is all clear, we just close xml tag.
         DOS->FPuts(fh, " />\n");
         return;
      }
   }
   else
   {
      DOS->FPuts(fh, ">\n");
      ++level;
      for (long i=0; i<pElem->GetChildrenCount(); i++)
      {
         XMLElement *pEl = pElem->GetChild(i);
         XMLWriteElement(fh, pEl);
      }
      --level;
      for (int i=0; i<level; i++)
         DOS->FPuts(fh, "\t");
   }
   DOS->FPuts(fh, "</");
   DOS->FPuts(fh, (char*)pElem->GetName());
   DOS->FPuts(fh, ">\n");
}
 
XMLDocument::XMLError XMLDocument::GetError()
{
   return eError;
}
