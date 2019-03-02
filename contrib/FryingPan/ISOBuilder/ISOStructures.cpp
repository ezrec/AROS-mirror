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

#include "ISOStructures.h"
#include <Generic/String.h>
#include <LibC/LibC.h>

Date::Date(const struct DateStamp *pDS)
{
   decompose(pDS);
}

Date::Date()
{
   decompose(0);
}

void Date::decompose(const struct DateStamp *pDS)
{
   unsigned long  l;
   unsigned char  cDays[] = 
   {
      31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
   };

   if (0 != pDS)
   {
      l = pDS->ds_Days;
      Year = (l<<2) / 1461;            // / 365.25
      Year += 1978;
      
      if (0 == (Year & 3))
         cDays[1] = 29;
      else
         cDays[1] = 28;
       
      l = l - (((Year-1978) * 1461) >> 2);    // * 365.25
         
      for (Month=0; Month<12; Month++)
      {
         if (l < cDays[(int)Month])
            break;
         l -= cDays[(int)Month];
      }
      Month++;
      while (Month > 12)
      {
         Month -= 12;
         Year++;
      }

      Day = l+1;
   
      l = pDS->ds_Minute;
      Hour = l/60;
      Minute = l - (Hour*60);
      Second = pDS->ds_Tick / 50;
      Hundredth = (pDS->ds_Tick - (Second * 50))*2; 
   }
   else
   {
      Year  = 0;
      Month = 0;
      Day   = 0;
      Hour  = 0;
      Minute = 0;
      Second = 0;
      Hundredth = 0;
   }
}

struct DateStamp Date::compose()
{
   struct DateStamp  hDS;
   static char    cDays[] = 
   {
      31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
   };

   if (0 == (Year & 3))
      cDays[1] = 29;
   else
      cDays[1] = 28;
   
   hDS.ds_Tick = (Hundredth / 2) + (Second * 50);
   hDS.ds_Minute = Minute + (Hour * 60);
   hDS.ds_Days = (Year * 1461) >> 2;
   for (int i=0; i<Month; i++)
      hDS.ds_Days += cDays[i];
   hDS.ds_Days += Day;
   
   return hDS;
}



void ISODate::setDate(const struct DateStamp *pDS)
{
   String s;
   
   Date hDate(pDS);
   
   s.FormatStr("%04ld%02ld%02ld%02ld%02ld%02ld%02ld", ARRAY(
         hDate.Year, 
         hDate.Month,
         hDate.Day,
         hDate.Hour,
         hDate.Minute,
         hDate.Second,
         hDate.Hundredth));
         
   strcpy((char*)&year, s.Data());
}

struct DateStamp ISODate::getDate() const
{
   struct DateStamp  hDS;
   Date              hDT;
   
   hDT.Year       = ((year[0] - '0') * 1000) +
                    ((year[1] - '0') * 100)  +
                    ((year[2] - '0') * 10)   +
                    (year[3] - '0');
    
   hDT.Month      = ((month[0] - '0') * 10) + (month[1] - '0');
   hDT.Day        = ((day[0] - '0') * 10) + (day[1] - '0');
   
   hDT.Hour       = ((hour[0] - '0') * 10) + (hour[1] - '0');
   hDT.Minute     = ((minute[0] - '0') * 10) + (minute[1] - '0');
   hDT.Second     = ((second[0] - '0') * 10) + (second[1] - '0');
   hDT.Hundredth  = ((hundredth[0] - '0') * 10) + (hundredth[1] - '0');
   
   hDS = hDT.compose();
   return hDS;   
}

void ISOShortDate::setDate(const struct DateStamp *pDS)
{
   Date hDT(pDS);
   
   year     = hDT.Year - 1900;
   month    = hDT.Month;
   day      = hDT.Day;
   hour     = hDT.Hour;
   minute   = hDT.Minute;
   second   = hDT.Second;
   hundredth= hDT.Hundredth;
}

struct DateStamp ISOShortDate::getDate() const
{
   struct DateStamp hDS;
   Date hDT;
   
   hDT.Year    = year + 1900;
   hDT.Month   = month;
   hDT.Day     = day;
   hDT.Hour    = hour;
   hDT.Minute  = minute;
   hDT.Second  = second;
   hDT.Hundredth= hundredth;
   
   hDS = hDT.compose();
   return hDS;
}




ISOPrimaryVolumeDescriptor::ISOPrimaryVolumeDescriptor()
{
   desc.hRootDirRecord.initialize();

   desc.lUnused        = 0;
   desc.lFileVersion   = 1;
   desc.lReserved      = 0;
   
   desc.hLogicalBlockSize.setVal(2048);

   desc.hVolumeSetSize.setVal(1);
   desc.hVolumeSeqNum.setVal(1);

   desc.hVolumeSpaceSize.setVal(0);
   desc.hPathTableSizeInBytes.setVal(0);
   desc.hLSBPathTableBlockNum.setVal(0);
   desc.hLSBOptPathTableBlockNum.setVal(0);
   desc.hMSBPathTableBlockNum.setVal(0);
   desc.hMSBOptPathTableBlockNum.setVal(0);

   setSystemID("Unknown");
   setVolumeID("New Disc");

   desc.hCreationDate.setDate(0);
   desc.hModificationDate.setDate(0);
   desc.hExpirationDate.setDate(0);
   desc.hEffectiveDate.setDate(0);
   
   desc.sUnused.setContents("");
   desc.sUnused2.setContents("");
   desc.sUnused3.setContents("");

   desc.sCopyrightFileID.setContents("");
   desc.sAbstractFileID.setContents("");
   desc.sBibliographicFileID.setContents("");
      
   setVolumeSetID("");
   setPublisherID("");
   setPreparerID("");
   setApplicationID("The Frying Pan - Amiga CD and DVD recording software");
   
   desc.sAppUse.setContents("");
   
   desc.hRootDirRecord.setFlags(2);
//   struct ISODirRecord              hRootDirRecord;
}

void ISOPrimaryVolumeDescriptor::setSystemID(const char *sSysID)
{
   systemid = sSysID;
   desc.sSystemID.setContents(sSysID);
}

const char* ISOPrimaryVolumeDescriptor::getSystemID() const
{
   return systemid.Data();
}

void ISOPrimaryVolumeDescriptor::setVolumeID(const char *sVolID)
{
   volumeid = sVolID;
   desc.sVolumeID.setContents(sVolID);
}

const char* ISOPrimaryVolumeDescriptor::getVolumeID() const
{
   return volumeid.Data();
}

void ISOPrimaryVolumeDescriptor::setVolumeSetID(const char *sVolSetID)
{
   volumesetid = sVolSetID;
   desc.sVolumeSetID.setContents(sVolSetID);
}

const char* ISOPrimaryVolumeDescriptor::getVolumeSetID() const
{
   return volumesetid.Data();
}

void ISOPrimaryVolumeDescriptor::setPublisherID(const char *sPubID)
{
   publisherid = sPubID;
   desc.sPublisherID.setContents(sPubID);
}

const char* ISOPrimaryVolumeDescriptor::getPublisherID() const
{
   return publisherid.Data();
}

void ISOPrimaryVolumeDescriptor::setPreparerID(const char *sPrepID)
{
   preparerid = sPrepID;
   desc.sPreparerID.setContents(sPrepID);
}

const char* ISOPrimaryVolumeDescriptor::getPreparerID() const
{
   return preparerid.Data();
}

void ISOPrimaryVolumeDescriptor::setApplicationID(const char* sAppID)
{
   applicationid = sAppID;
   desc.sApplicationID.setContents(sAppID);
}

const char* ISOPrimaryVolumeDescriptor::getApplicationID() const
{
   return applicationid.Data();
}

void ISOPrimaryVolumeDescriptor::setCreationDate(const struct DateStamp *pDate)
{
   desc.hCreationDate.setDate(pDate);
}

struct DateStamp ISOPrimaryVolumeDescriptor::getCreationDate() const 
{
   return desc.hCreationDate.getDate();
}

void ISOPrimaryVolumeDescriptor::setModificationDate(const struct DateStamp *pDate)
{
   desc.hModificationDate.setDate(pDate);
}

struct DateStamp ISOPrimaryVolumeDescriptor::getModificationDate() const 
{
   return desc.hModificationDate.getDate();
}

void ISOPrimaryVolumeDescriptor::setExpirationDate(const struct DateStamp *pDate)
{
   desc.hExpirationDate.setDate(pDate);
}

struct DateStamp ISOPrimaryVolumeDescriptor::getExpirationDate() const 
{
   return desc.hExpirationDate.getDate();
}

void ISOPrimaryVolumeDescriptor::setEffectiveDate(const struct DateStamp *pDate)
{
   desc.hEffectiveDate.setDate(pDate);
}

struct DateStamp ISOPrimaryVolumeDescriptor::getEffectiveDate() const 
{
   return desc.hEffectiveDate.getDate();  
}

void ISOPrimaryVolumeDescriptor::setVolumeSize(uint32 lSize)
{
   desc.hVolumeSpaceSize.setVal(lSize);
}

uint32 ISOPrimaryVolumeDescriptor::getVolumeSize() const
{
   return desc.hVolumeSpaceSize.getVal();
}

void ISOPrimaryVolumeDescriptor::setLSBPathTablePosition(uint32 lPosition)
{
   desc.hLSBPathTableBlockNum.setVal(lPosition);
   desc.hLSBOptPathTableBlockNum.setVal(0);
}

void ISOPrimaryVolumeDescriptor::setMSBPathTablePosition(uint32 lPosition)
{
   desc.hMSBPathTableBlockNum.setVal(lPosition);
   desc.hMSBOptPathTableBlockNum.setVal(0);
}

void ISOPrimaryVolumeDescriptor::setPathTableSize(uint32 lSize)
{
   desc.hPathTableSizeInBytes.setVal(lSize);
}

void ISOPrimaryVolumeDescriptor::setRootSize(uint32 lSize)
{
   desc.hRootDirRecord.setSize(lSize);
}

void ISOPrimaryVolumeDescriptor::setRootPosition(uint32 lPos)
{
   desc.hRootDirRecord.setPosition(lPos);
}

void ISOPrimaryVolumeDescriptor::setDate(const struct DateStamp* date)
{
   desc.hRootDirRecord.setDate(date);
   setCreationDate(date);
   setModificationDate(date);
   setEffectiveDate(date);
}

void* ISOPrimaryVolumeDescriptor::getData()
{
   return &desc;
}



const char* ISOSupplementaryVolumeDescriptor::getVolumeID() const
{
   return sVolumeID.getContents();
}

void ISOSupplementaryVolumeDescriptor::setVolumeID(const char *sVolSetID)
{
   sVolumeID.setContents(sVolSetID);
}

ISOSupplementaryVolumeDescriptor::ISOSupplementaryVolumeDescriptor()
   : ISOBasicDescriptor(ISOBasicDescriptor::Desc_Supplementary)
{
   hRootDirRecord.initialize();
   padCopyright   = 0;
   padBiblio      = 0;
   padAbstract    = 0;
   lUnused        = 0;
   lFileVersion   = 1;
   lReserved      = 0;
  
   sEscapeSeqs.setContents("%/@");

   hLogicalBlockSize.setVal(2048);

   hVolumeSetSize.setVal(1);
   hVolumeSeqNum.setVal(1);

   hVolumeSpaceSize.setVal(0);
   hPathTableSizeInBytes.setVal(0);
   hLSBPathTableBlockNum.setVal(0);
   hLSBOptPathTableBlockNum.setVal(0);
   hMSBPathTableBlockNum.setVal(0);
   hMSBOptPathTableBlockNum.setVal(0);

   sSystemID.setContents("Unknown");
   sVolumeID.setContents("New Disc");

   hCreationDate.setDate(0);
   hModificationDate.setDate(0);
   hExpirationDate.setDate(0);
   hEffectiveDate.setDate(0);
   
   sUnused.setContents("");
   sUnused3.setContents("");

   sCopyrightFileID.setContents("");
   sAbstractFileID.setContents("");
   sBibliographicFileID.setContents("");
      
   sVolumeSetID.setContents("");
   sPublisherID.setContents("");
   sPreparerID.setContents("");
   sApplicationID.setContents("The Frying Pan - Amiga CD and DVD recording software");
   
   sAppUse.setContents("");

   hRootDirRecord.setFlags(2);
}

void ISOSupplementaryVolumeDescriptor::setLSBPathTablePosition(uint32 lPosition)
{
   hLSBPathTableBlockNum.setVal(lPosition);
   hLSBOptPathTableBlockNum.setVal(0);
}

void ISOSupplementaryVolumeDescriptor::setMSBPathTablePosition(uint32 lPosition)
{
   hMSBPathTableBlockNum.setVal(lPosition);
   hMSBOptPathTableBlockNum.setVal(0);
}

void ISOSupplementaryVolumeDescriptor::setPathTableSize(uint32 lSize)
{
   hPathTableSizeInBytes.setVal(lSize);
}

void ISOSupplementaryVolumeDescriptor::setRootSize(uint32 lSize)
{
   hRootDirRecord.setSize(lSize);
}

void ISOSupplementaryVolumeDescriptor::setRootPosition(uint32 lPos)
{
   hRootDirRecord.setPosition(lPos);
}

void ISOSupplementaryVolumeDescriptor::setVolumeSize(uint32 lSize)
{
   hVolumeSpaceSize.setVal(lSize);
}

uint32 ISOSupplementaryVolumeDescriptor::getVolumeSize() const
{
   return hVolumeSpaceSize.getVal();
}

void ISOSupplementaryVolumeDescriptor::setDate(const struct DateStamp* date)
{
   hRootDirRecord.setDate(date);
   hCreationDate.setDate(date);
   hModificationDate.setDate(date);
   hEffectiveDate.setDate(date);
}

void ISOSupplementaryVolumeDescriptor::setVolumeSetID(const char *sVolSetID)
{
   sVolumeSetID.setContents(sVolSetID);
}

const char* ISOSupplementaryVolumeDescriptor::getVolumeSetID() const
{
   return sVolumeSetID.getContents();
}

void ISOSupplementaryVolumeDescriptor::setPublisherID(const char *sPubID)
{
   sPublisherID.setContents(sPubID);
}

const char* ISOSupplementaryVolumeDescriptor::getPublisherID() const
{
   return sPublisherID.getContents();
}

void ISOSupplementaryVolumeDescriptor::setPreparerID(const char *sPrepID)
{
   sPreparerID.setContents(sPrepID);
}

const char* ISOSupplementaryVolumeDescriptor::getPreparerID() const
{
   return sPreparerID.getContents();
}

void ISOSupplementaryVolumeDescriptor::setApplicationID(const char* sAppID)
{
   sApplicationID.setContents(sAppID);
}

const char* ISOSupplementaryVolumeDescriptor::getApplicationID() const
{
   return sApplicationID.getContents();   
}

void ISOSupplementaryVolumeDescriptor::setSystemID(const char *sSysID)
{
   sSystemID.setContents(sSysID);
}

const char* ISOSupplementaryVolumeDescriptor::getSystemID() const
{
   return sSystemID.getContents();
}





uint32 ISODirRecord::getLength(const char* sName, uint32 extsize)
{
   int sLength = strlen(sName);
   sLength += 33;    // sizeof(ISODirRecord) - sizeof(ISODirRecord::sFileID);
   sLength += 1;     // make the string length even - we really need that here
   sLength &= ~1;
   sLength += extsize;
   sLength += 1;     // make the string length even - we really need that here
   sLength &= ~1;
   return sLength;    
}

void ISODirRecord::setName(const char *sName)
{
   int lLen = strlen(sName);
   sFileID.setContents(sName, (lLen | 1));   // filename length must be odd!
   lFileIDLength        = lLen;
   lLength              = 33 + (lLen | 1);
   lExtendedAttrLength  = 0;
   lGapSize             = 0;
}

uint32 ISOWDirRecord::getLength(const char* sName)
{
   int sLength = strlen(sName);
   sLength <<= 1;
   sLength |= 1;     // make the string length even - we really need that here
   sLength += 33;    // sizeof(ISOWDirRecord) - sizeof(ISOWDirRecord::sFileID), but apparently we cant place that here :(
   return sLength;    
}

void ISOWDirRecord::setName(const char *sName)
{
   int lLen = strlen(sName) << 1;
   sFileID.setContents(sName, (lLen | 1));   // filename length must be odd!
   lFileIDLength        = lLen;
   lLength              = (lLen | 1) + 33;
   lExtendedAttrLength  = 0;
   lGapSize             = 0;   
}




uint32 ISOPathRecord::getISOLength(const char *sName)
{
   int sLength = strlen(sName);
   if (!sLength)
      ++sLength;
   ++sLength;
   sLength &= ~1;                      // number must be even
   sLength += 8;     // sizeof(ISOPathRecord) - sizeof(ISOPathRecord::sName);   
                     // this one is a little bit too long (two bytes to be more precise) see last union
   return sLength;
}



uint32 ISOPathRecord::getJolietLength(const char *sName)
{
   int sLength = strlen(sName);
   if (!sLength)
      ++sLength;
   sLength <<= 1;
   sLength += 8;     // sizeof(ISOPathRecord) - sizeof(ISOPathRecord::sName);   
                     // this one is a little bit too long (two bytes to be more precise) see last union
   return sLength;
}

void ISOPathRecord::setExtent(uint32 lExtent, bool bIsMSB)
{
   if (bIsMSB)
      hMSBExtent.setVal(lExtent);
   else
      hLSBExtent.setVal(lExtent);
}

void ISOPathRecord::setParent(unsigned short lNum, bool bIsMSB)
{
   if (bIsMSB)
      hMSBParent.setVal(lNum);
   else
      hLSBParent.setVal(lNum);
}

void ISOPathRecord::setISOName(const char *sNewName)
{
   if (sNewName != 0) 
   {
      lLength              = strlen(sNewName);
      sDirID.setContents(sNewName, (lLength + 1) & ~1);
      lExtendedAttrLength  = 0;
   }
   else
   {
      sDirID.setContents("");
      lLength              = 1;
      lExtendedAttrLength  = 0;
   }
}

void ISOPathRecord::setJolietName(const char *sNewName)
{
   if (sNewName != 0) 
   {
      sWideDirID.setContents(sNewName);
      lLength              = strlen(sNewName) << 1;
      lExtendedAttrLength  = 0;
   }
   else
   {
      sWideDirID.setContents("");
      lLength              = 1;
      lExtendedAttrLength  = 0;
   }
}




