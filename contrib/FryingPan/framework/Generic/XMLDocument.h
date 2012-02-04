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

#ifndef XMLDATA_H_
#define XMLDATA_H_

#include "LibrarySpool.h"
#include "String.h"
#include "VectorT.h"

#include <libdata/expat.h>

/*
 * this class relies on expat2.library (both binary and include files) that i have created...
 */

struct Expat2IFace;

namespace GenNS
{
   class XMLAttribute 
   {
   protected:
      String                  sName;
      String                  sValue;

   protected:
      virtual void            Initialize(const char* sN, const char* sV);
      virtual void            Initialize(const char* sN, int64 lV);

   public:
      XMLAttribute();
      XMLAttribute(const char* sName, const char* sValue);
      XMLAttribute(const char* sName, int64 sValue);
      virtual                ~XMLAttribute();

      virtual String         &GetName();
      virtual String         &GetValue();
      virtual void            SetName(const char* sName);
      virtual void            SetValue(const char* sValue);
      virtual void            SetValue(uint64 sValue);
   };

   class XMLElement : public XMLAttribute
   {
   protected:
      VectorT<XMLElement*>    hChildren;
      VectorT<XMLAttribute*>  hAttributes;

   protected:
      virtual void            Initialize(const char* sN, const char* sV);
      static bool             freeAttr(XMLAttribute* const& attr);
      static bool             freeNode(XMLElement* const& node);
   public:
      XMLElement();
      XMLElement(const char* sName, const char* sValue);
      virtual                ~XMLElement();

      virtual long            GetChildrenCount();
      virtual XMLElement     *GetChild(long i);
      virtual long            GetAttributesCount();
      virtual XMLAttribute   *GetAttribute(long i);
      virtual String         *GetChildValue(const char *sName);
      virtual String         *GetAttributeValue(const char *sName);
      virtual XMLElement     *FindChild(const char* sName);
      virtual XMLAttribute   *FindAttribute(const char* sName);

      virtual XMLElement     *AddChild(const char* sName, const char* sValue);
      virtual XMLAttribute   *AddAttribute(const char* sName, const char* sValue);
      virtual XMLAttribute   *AddAttribute(const char* sName, int64 lValue);
      virtual XMLElement     *AddChild(XMLElement* pElem);
      virtual XMLAttribute   *AddAttribute(XMLAttribute *pAttr);
      virtual void            RemChild(XMLElement* pElem);
      virtual void            RemAttribute(XMLAttribute* pAttr);
   };

   class XMLDocument : public XMLElement
   {
   public:
      enum XMLError
      {
         XErr_OK     =  0,          // all ok
         XErr_NoExpat,              // unable to open expat2.library
         XErr_NoFile,               // unable to open file
         XErr_NoMemory,             // not enough memory
         XErr_CorruptedXML,         // corrupted xml file
         XErr_NoParser              // unable to create parser
      };

   protected:
      Expat2IFace         *pExpat;
      String               sFileName;
      XML_Parser           pParser;
      XMLError             eError;
      int                  level;

      VectorT<XMLElement*> hParse;

      static void          fXMLStartHandler(void* userData, const XML_Char *name, const XML_Char **atts);
      static void          fXMLEndHandler(void* userData, const XML_Char *name);
      static void          fXMLContentHandler(void* userData, const XML_Char *begin, int length);
      void                 XMLStartHandler(const XML_Char *name, const XML_Char **atts);
      void                 XMLEndHandler(const XML_Char *name);
      void                 XMLContentHandler(const XML_Char *begin, int length);
      void                 XMLWriteElement(BPTR fh, XMLElement* pElem);
   public:
      XMLDocument();
      XMLDocument(const char* sName);
      virtual             ~XMLDocument();

      bool                 ReadXML(const char *sName);
      bool                 WriteXML(const char *sName);
      bool                 ReadXML(BPTR file);
      bool                 WriteXML(BPTR file);
      XMLError             GetError();
   };
};

#endif /*XMLDATA_H_*/
