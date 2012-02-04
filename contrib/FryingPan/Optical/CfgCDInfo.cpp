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

#include "CfgCDInfo.h"
#include <Generic/XMLDocument.h>


static const char   *Cfg_QuickBlankMeas      = "QuickBlankMeas";
static const char   *Cfg_CompleteBlankMeas   = "CompleteBlankMeas";
static const char   *Cfg_QuickFormatMeas     = "QuickFormatMeas";
static const char   *Cfg_CompleteFormatMeas  = "CompleteFormatMeas";
static const char   *Cfg_Meas                = "Measurement";
static const char   *Cfg_MeasDiscSize        = "DiscSize";
static const char   *Cfg_MeasIOSpeed         = "IOSpeed";
static const char   *Cfg_MeasCount           = "Count";
static const char   *Cfg_MeasAvgTime         = "AverageTime";

CfgCDInfo::CfgCDInfo(ConfigParser *parent, const char *cls)
{
   XMLElement *xel, *t;
   
   config = new ConfigParser(parent, cls);
   qblankspeeds.FreeOnDelete(true);
   cblankspeeds.FreeOnDelete(true);
   qformatspeeds.FreeOnDelete(true);
   cformatspeeds.FreeOnDelete(true);

   xel = config->getElement()->FindChild(Cfg_QuickBlankMeas);
   if (0 != xel)
   {
      for (int i=0; i<xel->GetChildrenCount(); i++)
      {
         int32 size;
         int16 spd, time, cnt;

         t = xel->GetChild(i);
         if (0 != t)
         {
            size = t->GetAttributeValue(Cfg_MeasDiscSize)->ToLong();
            spd  = t->GetAttributeValue(Cfg_MeasIOSpeed)->ToLong();
            time = t->GetAttributeValue(Cfg_MeasAvgTime)->ToLong();
            cnt  = t->GetAttributeValue(Cfg_MeasCount)->ToLong();

            qblankspeeds << new CfgIOMeas(size, spd, cnt, time);
         }
      }
   }
   else
   {
      config->getElement()->AddChild(Cfg_QuickBlankMeas, 0);
   }

   xel = config->getElement()->FindChild(Cfg_CompleteBlankMeas);
   if (0 != xel)
   {
      for (int i=0; i<xel->GetChildrenCount(); i++)
      {
         int32 size;
         int16 spd, time, cnt;

         t = xel->GetChild(i);
         if (0 != t)
         {
            size = t->GetAttributeValue(Cfg_MeasDiscSize)->ToLong();
            spd  = t->GetAttributeValue(Cfg_MeasIOSpeed)->ToLong();
            time = t->GetAttributeValue(Cfg_MeasAvgTime)->ToLong();
            cnt  = t->GetAttributeValue(Cfg_MeasCount)->ToLong();

            cblankspeeds << new CfgIOMeas(size, spd, cnt, time);
         }
      }
   }
   else
   {
      config->getElement()->AddChild(Cfg_CompleteBlankMeas, 0);
   }

   xel = config->getElement()->FindChild(Cfg_QuickFormatMeas);
   if (0 != xel)
   {
      for (int i=0; i<xel->GetChildrenCount(); i++)
      {
         int32 size;
         int16 spd, time, cnt;

         t = xel->GetChild(i);
         if (0 != t)
         {
            size = t->GetAttributeValue(Cfg_MeasDiscSize)->ToLong();
            spd  = t->GetAttributeValue(Cfg_MeasIOSpeed)->ToLong();
            time = t->GetAttributeValue(Cfg_MeasAvgTime)->ToLong();
            cnt  = t->GetAttributeValue(Cfg_MeasCount)->ToLong();

            qformatspeeds << new CfgIOMeas(size, spd, cnt, time);
         }
      }
   }
   else
   {
      config->getElement()->AddChild(Cfg_QuickFormatMeas, 0);
   }

   xel = config->getElement()->FindChild(Cfg_CompleteFormatMeas);
   if (0 != xel)
   {
      for (int i=0; i<xel->GetChildrenCount(); i++)
      {
         int32 size;
         int16 spd, time, cnt;

         t = xel->GetChild(i);
         if (0 != t)
         {
            size = t->GetAttributeValue(Cfg_MeasDiscSize)->ToLong();
            spd  = t->GetAttributeValue(Cfg_MeasIOSpeed)->ToLong();
            time = t->GetAttributeValue(Cfg_MeasAvgTime)->ToLong();
            cnt  = t->GetAttributeValue(Cfg_MeasCount)->ToLong();

            cformatspeeds << new CfgIOMeas(size, spd, cnt, time);
         }
      }
   }
   else
   {
      config->getElement()->AddChild(Cfg_CompleteFormatMeas, 0);
   }
}

CfgCDInfo::~CfgCDInfo()
{
   delete config;
}

void CfgCDInfo::onWrite()
{
   XMLElement *xel, *t;

   xel = config->getElement()->FindChild(Cfg_QuickBlankMeas);
   while (xel->GetChildrenCount() > 0)
   {
      t = xel->GetChild(0);
      xel->RemChild(t);
   }
   for (int i=0; i<qblankspeeds.Count(); i++)
   {
      t = new XMLElement(Cfg_Meas, 0);
      t->AddAttribute(Cfg_MeasDiscSize, qblankspeeds[i]->getDiscSize());
      t->AddAttribute(Cfg_MeasIOSpeed,  qblankspeeds[i]->getIOSpeed());
      t->AddAttribute(Cfg_MeasCount,    qblankspeeds[i]->getCount());
      t->AddAttribute(Cfg_MeasAvgTime,  qblankspeeds[i]->getTime());

      xel->AddChild(t);
   }

   xel = config->getElement()->FindChild(Cfg_CompleteBlankMeas);
   while (xel->GetChildrenCount() > 0)
   {
      t = xel->GetChild(0);
      xel->RemChild(t);
   }
   for (int i=0; i<cblankspeeds.Count(); i++)
   {
      t = new XMLElement(Cfg_Meas, 0);
      t->AddAttribute(Cfg_MeasDiscSize, cblankspeeds[i]->getDiscSize());
      t->AddAttribute(Cfg_MeasIOSpeed,  cblankspeeds[i]->getIOSpeed());
      t->AddAttribute(Cfg_MeasCount,    cblankspeeds[i]->getCount());
      t->AddAttribute(Cfg_MeasAvgTime,  cblankspeeds[i]->getTime());

      xel->AddChild(t);
   }

   xel = config->getElement()->FindChild(Cfg_QuickFormatMeas);
   while (xel->GetChildrenCount() > 0)
   {
      t = xel->GetChild(0);
      xel->RemChild(t);
   }
   for (int i=0; i<qformatspeeds.Count(); i++)
   {
      t = new XMLElement(Cfg_Meas, 0);
      t->AddAttribute(Cfg_MeasDiscSize, qformatspeeds[i]->getDiscSize());
      t->AddAttribute(Cfg_MeasIOSpeed,  qformatspeeds[i]->getIOSpeed());
      t->AddAttribute(Cfg_MeasCount,    qformatspeeds[i]->getCount());
      t->AddAttribute(Cfg_MeasAvgTime,  qformatspeeds[i]->getTime());

      xel->AddChild(t);
   }

   xel = config->getElement()->FindChild(Cfg_CompleteFormatMeas);
   while (xel->GetChildrenCount() > 0)
   {
      t = xel->GetChild(0);
      xel->RemChild(t);
   }
   for (int i=0; i<cformatspeeds.Count(); i++)
   {
      t = new XMLElement(Cfg_Meas, 0);
      t->AddAttribute(Cfg_MeasDiscSize, cformatspeeds[i]->getDiscSize());
      t->AddAttribute(Cfg_MeasIOSpeed,  cformatspeeds[i]->getIOSpeed());
      t->AddAttribute(Cfg_MeasCount,    cformatspeeds[i]->getCount());
      t->AddAttribute(Cfg_MeasAvgTime,  cformatspeeds[i]->getTime());

      xel->AddChild(t);
   }

}


CfgIOMeas *CfgCDInfo::getQBlankMeas(uint32 size, uint16 spd)
{
   CfgIOMeas * meas = 0;

   for (int i=0; i<qblankspeeds.Count(); i++)
   {
      if ((qblankspeeds[i]->getDiscSize() == size) && (qblankspeeds[i]->getIOSpeed() == spd))
         return qblankspeeds[i];
   }

   meas = new CfgIOMeas(size, spd);
   qblankspeeds << meas;

   return meas;
}

CfgIOMeas *CfgCDInfo::getQFormatMeas(uint32 size, uint16 spd)
{
   CfgIOMeas * meas = 0;

   for (int i=0; i<qformatspeeds.Count(); i++)
   {
      if ((qformatspeeds[i]->getDiscSize() == size) && (qformatspeeds[i]->getIOSpeed() == spd))
         return qformatspeeds[i];
   }

   meas = new CfgIOMeas(size, spd);
   qformatspeeds << meas;

   return meas;
}

CfgIOMeas *CfgCDInfo::getCBlankMeas(uint32 size, uint16 spd)
{
   CfgIOMeas * meas = 0;

   for (int i=0; i<cblankspeeds.Count(); i++)
   {
      if ((cblankspeeds[i]->getDiscSize() == size) && (cblankspeeds[i]->getIOSpeed() == spd))
         return cblankspeeds[i];
   }

   meas = new CfgIOMeas(size, spd);
   cblankspeeds << meas;

   return meas;
}

CfgIOMeas *CfgCDInfo::getCFormatMeas(uint32 size, uint16 spd)
{
   CfgIOMeas * meas = 0;

   for (int i=0; i<cformatspeeds.Count(); i++)
   {
      if ((cformatspeeds[i]->getDiscSize() == size) && (cformatspeeds[i]->getIOSpeed() == spd))
         return cformatspeeds[i];
   }

   meas = new CfgIOMeas(size, spd);
   cformatspeeds << meas;

   return meas;
}

