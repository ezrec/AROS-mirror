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

#ifndef _ISOSTRUCTURES_H_
#define _ISOSTRUCTURES_H_

#include <Generic/Types.h>
#include "Xlators/Translator.h"
#include "Xlators/TransAChars.h"
#include "Xlators/TransDChars.h"
#include <dos/dos.h>
#include <Generic/String.h>
#include <LibC/LibC.h>


/*
 * VIRTUAL MEMBERS ARE NOT ALLOWED HERE
 */

/*
 * 7.3.x - longword storage
 * don't change it to L2BE/L2LE/W2BE/W2LE as sometimes the longwords are located at unaligned addresses (makes problems)
 */
class val731
{
   uint8 lVal[4];
   
public:
   void   setVal(uint32 lV)
   {
      lVal[0] = lV & 0xff;
      lVal[1] = (lV >> 8) & 0xff;
      lVal[2] = (lV >> 16) & 0xff;
      lVal[3] = (lV >> 24) & 0xff;
   }

   uint32 getVal() const
   {
      return ((lVal[0]) | (lVal[1] << 8) | (lVal[2] << 16) | (lVal[3] << 24));
   }
} PACKED;

class val732
{
   uint8 lVal[4];
   
public:
   void   setVal(uint32 lV)
   {
      lVal[0] = (lV >> 24) & 0xff;
      lVal[1] = (lV >> 16) & 0xff;
      lVal[2] = (lV >> 8) &0xff;
      lVal[3] = lV & 0xff;
   }

   uint32 getVal() const
   {
      return ((lVal[0] << 24) | (lVal[1] << 16) | (lVal[2] << 8) | lVal[3]);
   }
} PACKED;

class val733
{
   val731 lVal1;
   val732 lVal2;
   
public:
   void   setVal(uint32 lV)
   {
      lVal1.setVal(lV);
      lVal2.setVal(lV);
   }

   uint32 getVal() const
   {
      return lVal2.getVal();
   }
} PACKED;

/*
 * 7.2.x - word storage
 */
class val721
{
   uint8 lVal[2];
   
public:
   void   setVal(uint16 lV)
   {
      lVal[0] = lV & 0xff;
      lVal[1] = (lV >> 8) & 0xff;
   }

   uint16 getVal() const
   {
      return ((lVal[0]) | (lVal[1]<<8));
   }
} PACKED;

class val722
{
   uint8 lVal[2];
   
public:
   void   setVal(uint16 lV)
   {
      lVal[0] = (lV >> 8) & 0xff;
      lVal[1] = lV & 0xff;
   }

   uint16 getVal() const
   {
      return ((lVal[1]) | (lVal[0]<<8));
   }
} PACKED;

class val723
{
   val721 lVal1;
   val722 lVal2;
   
public:
   void   setVal(uint16 lVal)
   {
      lVal1.setVal(lVal);
      lVal2.setVal(lVal);
   }

   uint16 getVal() const
   {
      return lVal2.getVal();
   }
} PACKED;

/*
 * date storage
 */
class Date
{
public:
   uint16 Year;
   uint8  Month;
   uint8  Day;
   uint8  Hour;
   uint8  Minute;
   uint8  Second;
   uint8  Hundredth;

public:
                     Date(const struct DateStamp *pDS);
                     Date();
   void              decompose(const struct DateStamp *pDS);
   struct DateStamp  compose();
} PACKED;

class ISODate
{
   uint8  year[4];
   uint8  month[2];
   uint8  day[2];
   uint8  hour[2];
   uint8  minute[2];
   uint8  second[2];
   uint8  hundredth[2];
   uint8  zero;
   
public:
   void              setDate(const struct DateStamp *pDS);   
   struct DateStamp  getDate() const;
} PACKED;

class ISOShortDate
{
   uint8  year;    // -1900
   uint8  month;   // 0-11?
   uint8  day;     // 0-31?
   uint8  hour;    // 0-23
   uint8  minute;  // 0-59
   uint8  second;  // 0-59
   uint8  hundredth; // 0.

public:
   void              setDate(const struct DateStamp *pDS);   
   struct DateStamp  getDate() const;
} PACKED;


/*
 * ISOCString
 *    ISO Constant-Length String
 *    features constant length and translator 
 */

template <int stepSize, int size=1, char def=0>
class ISOCString
{
public:
   char           sContents[size*stepSize];

public: 
   int            setContents(const char* sValue, int lLen=-1)
   {
      int len, i, j;
      if (lLen > 0)
      {
         len = lLen*stepSize;
      }
      else
      {
         if (size == 1)          // who'd like a one-byte-long string!?
            len = strlen(sValue)*stepSize;
         else
            len = size*stepSize;
      }
            
      if (len < size*stepSize)
         len = size*stepSize;
         
      j = 0;
      i = 0;
         
      while (i<len)
      {
         if (stepSize > 1)
         {
            sContents[i] = 0;
            ++i;            
         }
         
         if (sValue[j] != 0)
         {
            sContents[i] = sValue[j];
            j++;
         }
         else
         {
            sContents[i] = def;     // space or zero or anything else
            ++i;
            break;
         }
         ++i;
      }

      while (i < len)
      {
         if (stepSize > 1)
         {
            sContents[i] = 0;
            ++i;
         }
         sContents[i] = def;
         ++i;
      }

      return i;
   }

   String         getContents() const
   {
      return (char*)&sContents;
   }
} __attribute__((packed));

template <int size=1, char def=0>
class ISOString : public ISOCString<1, size, def>
{
} __attribute__((packed));

template <int size=1, short def=0>
class ISOWString : public ISOCString<2, size, def>
{
} __attribute__((packed));


template <class StrType>
class ISOCDirRecord
{
public:
   enum DirRecordFlags
   {
      DRF_Exists     =     1,    // if set, entry shall not be reported to the user
      DRF_Directory  =     2,    // if set, entry is a directory
      DRF_Associated =     4,    // if set, entry is an associated file
      DRF_Record     =     8,    // if set, we have a record format [extended attribute record]
      DRF_Protection =    16,    // if set, we have owner and group id specified for the file [extended attribute record]
      DRF_MultiExtent=   128,    // if set, this is not the final dir record for the file
   };


public:
   uint8                    lLength;
   uint8                    lExtendedAttrLength;
   struct val733                    hExtent;
   struct val733                    hDataLength;
   struct ISOShortDate              hRecordingDate;
   uint8                    lFileFlags;
   uint8                    lFileUnitSize; // 0
   uint8                    lGapSize;
   struct val723                    hVolumeSeqNum;
   uint8                    lFileIDLength;
   StrType                          sFileID;
   
public:
   ISOCDirRecord()
   {
      initialize();
   }

   void              initialize()
   {
      hExtent.setVal(0);
      hDataLength.setVal(0);
      hRecordingDate.setDate(0);
      hVolumeSeqNum.setVal(1);
      lExtendedAttrLength  = 0;
      lLength              = 33;              // initially.
      lFileFlags           = 0;
      lFileUnitSize        = 0;
      lFileIDLength        = 0;
      lGapSize             = 0;

   }

   uint8            *getExtension()
   {
      return &((uint8*)this)[(lLength + 1) &~ 1];
   }

   void              setExtensionSize(int size)
   {
      lLength = (lLength+size+1)&~1;
   }

   void              setPosition(uint32 lBlockNum)
   {
      hExtent.setVal(lBlockNum);
   }
   
   void              setSize(uint32 lSize)
   {
      hDataLength.setVal(lSize);
   }

   int               getSize()
   {
      return (lLength+1) &~ 1;
   }

   void              setFlags(uint8 lFlags)
   {
      lFileFlags = lFlags;
   }

   void              setDate(const struct DateStamp *ds)
   {
      hRecordingDate.setDate(ds);
   }
} __attribute__((packed)) ;

class ISODirRecord : public ISOCDirRecord< ISOString< > >
{
public:
   static uint32             getLength(const char* sName, uint32 extsize=0);
   void                             setName(const char *sName);
   
} __attribute__((packed)) ;

class ISOWDirRecord : public ISOCDirRecord< ISOWString< > >
{
public:
   static uint32             getLength(const char* sName);
   void                             setName(const char *sName);
} __attribute__((packed)) ;

class ISODotDirRecord : public ISODirRecord
{
public:
   void              initialize()
   {
      ISODirRecord::initialize();
      lLength              = 34;
      setFlags(2);
      lFileIDLength        = 1;
      sFileID.sContents[0] = 0;
   }
} __attribute__((packed));

class ISODotDotDirRecord : public ISODirRecord
{
public:
   void              initialize()
   {
      ISODirRecord::initialize();
      lLength              = 34;
      setFlags(2);
      lFileIDLength        = 1;
      sFileID.sContents[0] = 1;
   }
} __attribute__((packed));



class ISOPathRecord
{
   uint8  lLength;
   uint8  lExtendedAttrLength;
   union
   {
      struct val731  hLSBExtent;
      struct val732  hMSBExtent;
   } __attribute__((packed));
   
   union
   {
      struct val721  hLSBParent;
      struct val722  hMSBParent;
   } __attribute__((packed));

   union 
   {
      class ISOString< >  sDirID;
      class ISOWString< >  sWideDirID;
   } __attribute__((packed));

public:
   static uint32 getISOLength(const char *sName);
   static uint32 getJolietLength(const char *sName);

   void                 setExtent(uint32 lExtent, bool bIsMSB);
   void                 setParent(uint16 lNum, bool bIsMSB);
   void                 setISOName(const char *sName);
   void                 setJolietName(const char *sName);
} __attribute__((packed));


struct ISOBasicDescriptor
{
public:
   enum DescriptorType
   {
      Desc_BootRecord         =  0,
      Desc_Primary,
      Desc_Supplementary,
      Desc_VolumePartition,
      Desc_Terminator         =  255
   };

public:
   uint8     lType;
   uint8     sID[5];
   uint8     lVersion;

public: 
   ISOBasicDescriptor(DescriptorType type)
   {
      lType = type;
      sID[0] = 'C';
      sID[1] = 'D';
      sID[2] = '0';
      sID[3] = '0';
      sID[4] = '1';
      lVersion = 1;
   }
} __attribute__((packed));

struct ISOEndDescriptor : ISOBasicDescriptor
{
   char              lPad[2041];
public:
   ISOEndDescriptor() : ISOBasicDescriptor(ISOBasicDescriptor::Desc_Terminator)
   {
      memset(lPad, 0, sizeof(lPad));
   };
};

struct ISOBootRecordDescriptor : ISOBasicDescriptor
{
   ISOString<32, 32>   sSystemID;
   ISOString<32, 32>   sBootID;
   ISOString<1977, 0>   sReserved;
public:
   ISOBootRecordDescriptor(); 
} __attribute__((packed));

class ISOPrimaryVolumeDescriptor
{
   struct descriptor : public ISOBasicDescriptor
   {
      char                             lUnused;
      ISOString<32, 32>                sSystemID;
      ISOString<32, 32>                sVolumeID;
      ISOString<8, 0>                  sUnused;
      struct val733                    hVolumeSpaceSize;
      ISOString<32, 0>                 sUnused2;
      struct val723                    hVolumeSetSize;
      struct val723                    hVolumeSeqNum;
      struct val723                    hLogicalBlockSize;      
      struct val733                    hPathTableSizeInBytes;
      struct val731                    hLSBPathTableBlockNum;
      struct val731                    hLSBOptPathTableBlockNum;
      struct val732                    hMSBPathTableBlockNum;
      struct val732                    hMSBOptPathTableBlockNum;
  
      ISODotDirRecord                  hRootDirRecord;
   
      ISOString<128, 32>               sVolumeSetID;
      ISOString<128, 32>               sPublisherID;
      ISOString<128, 32>               sPreparerID;
      ISOString<128, 32>               sApplicationID;

      ISOString<37, 32>                sCopyrightFileID;
      ISOString<37, 32>                sAbstractFileID;
      ISOString<37, 32>                sBibliographicFileID;

      struct ISODate                   hCreationDate;
      struct ISODate                   hModificationDate;
      struct ISODate                   hExpirationDate;
      struct ISODate                   hEffectiveDate;   

      char                             lFileVersion;                    // 1!
      char                             lReserved;
      ISOString<512, 32>               sAppUse;
      ISOString<653, 0>                sUnused3;

      descriptor() : ISOBasicDescriptor(ISOBasicDescriptor::Desc_Primary)
      {
      }
   } __attribute__((packed));

   descriptor        desc;

   String            volumeid;
   String            volumesetid;
   String            publisherid;
   String            preparerid;
   String            applicationid;
   String            systemid;

public:

                     ISOPrimaryVolumeDescriptor();   
   void              setSystemID(const char *sSysID);
   const char*       getSystemID() const;
   void              setVolumeID(const char *sVolID);
   const char*       getVolumeID() const;

   void              setVolumeSize(uint32 lSize);
   uint32     getVolumeSize() const;
   
   void              setVolumeSetID(const char *sVolSetID);
   const char*       getVolumeSetID() const;
   void              setPublisherID(const char *sPubID);
   const char*       getPublisherID() const;
   void              setPreparerID(const char *sPrepID);
   const char*       getPreparerID() const;
   void              setApplicationID(const char* sAppID);
   const char*       getApplicationID() const;
  
   void              setDate(const struct DateStamp *pDate);
   void              setCreationDate(const struct DateStamp *pDate);
   struct DateStamp  getCreationDate() const;
   void              setModificationDate(const struct DateStamp *pDate);
   struct DateStamp  getModificationDate() const;
   void              setExpirationDate(const struct DateStamp *pDate);
   struct DateStamp  getExpirationDate() const;
   void              setEffectiveDate(const struct DateStamp *pDate);
   struct DateStamp  getEffectiveDate() const;
   
   void              setLSBPathTablePosition(uint32 lPosition);
   void              setMSBPathTablePosition(uint32 lPosition);
   void              setPathTableSize(uint32 lSize);
   
   void              setRootSize(uint32 lSize);
   void              setRootPosition(uint32 lPos);

   void*             getData();
};

class ISOSupplementaryVolumeDescriptor : ISOBasicDescriptor
{
   char                             lUnused;
   ISOWString<16, 32>               sSystemID;           // twice less
   ISOWString<16, 32>               sVolumeID;
   ISOWString<4,  0>                sUnused;
   struct val733                    hVolumeSpaceSize;                // total number of blocks in this image
   ISOString<32,  0>                sEscapeSeqs;
   struct val723                    hVolumeSetSize;
   struct val723                    hVolumeSeqNum;
   struct val723                    hLogicalBlockSize;      
   struct val733                    hPathTableSizeInBytes;
   struct val731                    hLSBPathTableBlockNum;
   struct val731                    hLSBOptPathTableBlockNum;
   struct val732                    hMSBPathTableBlockNum;
   struct val732                    hMSBOptPathTableBlockNum;
  
   ISODotDirRecord                  hRootDirRecord;
   
   ISOWString<64, 32>               sVolumeSetID;
   ISOWString<64, 32>               sPublisherID;
   ISOWString<64, 32>               sPreparerID;
   ISOWString<64, 32>               sApplicationID;

   ISOWString<18, 32>               sCopyrightFileID;
   char                             padCopyright;
   ISOWString<18, 32>               sAbstractFileID;
   char                             padAbstract;
   ISOWString<18, 32>               sBibliographicFileID;
   char                             padBiblio;

   struct ISODate                   hCreationDate;
   struct ISODate                   hModificationDate;
   struct ISODate                   hExpirationDate;
   struct ISODate                   hEffectiveDate;   

   char                             lFileVersion;                    // 1!
   char                             lReserved;
   ISOString<512, 32>               sAppUse;
   ISOString<653, 0>                sUnused3;
   
public:
                     ISOSupplementaryVolumeDescriptor();
   void              setVolumeID(const char *sVolID);
   const char*       getVolumeID() const;
                        
   void              setSystemID(const char *sSysID);
   const char*       getSystemID() const;
   void              setVolumeSetID(const char *sVolSetID);
   const char*       getVolumeSetID() const;
   void              setPublisherID(const char *sPubID);
   const char*       getPublisherID() const;
   void              setPreparerID(const char *sPrepID);
   const char*       getPreparerID() const;
   void              setApplicationID(const char* sAppID);
   const char*       getApplicationID() const;

   void              setDate(const struct DateStamp *pDate);
   void              setVolumeSize(uint32 lSize);
   uint32     getVolumeSize() const;
   
   void              setLSBPathTablePosition(uint32 lPosition);
   void              setMSBPathTablePosition(uint32 lPosition);
   void              setPathTableSize(uint32 lSize);
   void              setRootSize(uint32 lSize);
   void              setRootPosition(uint32 lPos);
} __attribute__((packed));

struct ISOVolumePartitionDescriptor : ISOBasicDescriptor
{
   uint8                    lUnused;
   ISOString<32, 32>                sSystemID;
   ISOString<32, 32>                sVolumeID;
   struct val733                    hExtent;
   struct val733                    hNumExtents;
   uint8                    sUnused[1960];

public:
   ISOVolumePartitionDescriptor()
      : ISOBasicDescriptor(ISOBasicDescriptor::Desc_VolumePartition)
   {
   }
};
 

#endif //_ISOSTRUCTURES_H_
