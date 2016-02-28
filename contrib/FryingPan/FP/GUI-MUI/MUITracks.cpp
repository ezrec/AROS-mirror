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

#include "MUITracks.h"
#include <libclass/intuition.h>
#include <Generic/ConfigParser.h>
#include <FP/IEngine.h>
#include "Globals.h"
#include "MUITracksISOBuilder.h"
#include "MUITracksDataAudio.h"
#include "MUITracksSession.h"
#include <libraries/mui.h>

/*
 * localization area
 */
enum Loc
{
   loc_CreateNewDisc =  lg_Tracks,
   loc_ImportImage

};

static class Localization::LocaleSet LocaleSets[] =
{
   {  loc_CreateNewDisc,   "Create New Disc",            "CCL_CREATENEWDISC"  },
   {  loc_ImportImage,     "Import Existing Disc Image", "CCL_IMPORTIMAGE"    },
   
   {  Localization::LocaleSet::Set_Last, 0, 0                                 }
};

static const char* LocaleGroup = "TRACKS";

MUITracks::MUITracks(ConfigParser *parent, Globals &glb) :
   Glb(glb)
{
   all         = 0;
   Config      = new ConfigParser(parent, "PageTracks", 0);
   ISOBuilder  = new MUITracksISOBuilder(Config, Glb);
   DataAudio   = new MUITracksDataAudio(Config, Glb);
   Session     = new MUITracksSession(Config, Glb);
   hHkButton.Initialize(this, &MUITracks::onButton);
   setButtonCallBack(hHkButton.GetHook());
   glb.Loc.Add((Localization::LocaleSet*)LocaleSets, LocaleGroup);
}

MUITracks::~MUITracks()
{
   delete DataAudio;
   delete ISOBuilder;
   delete Session;
   delete Config;
}

bool MUITracks::start()
{
   ISOBuilder->start();
   DataAudio->start();
   Session->start();

   Glb.WriteSelection << this;
   return true;
}

void MUITracks::stop()
{
   Glb.WriteSelection >> this;
   DataAudio->stop();
   ISOBuilder->stop();
   Session->stop();
   all = 0;
}

unsigned long *MUITracks::getObject()
{
   if (0 != all)
      return (unsigned long *)all;

   static const char *options[3] =
   {
      0
   };
   options[0] = Glb.Loc[loc_CreateNewDisc];
   options[1] = Glb.Loc[loc_ImportImage];

   static const char  *titles[3] = 
   {
      0,
   };
   titles[0] = ISOBuilder->getName(),
   titles[1] = DataAudio->getName(),

   all = VGroup,
      Child,                  muiCycle(options, 0, Btn_CompositionType, 0),
      Child,                  page = PageGroup,
         Child,                  RegisterGroup(titles),
            Child,                  ISOBuilder->getObject(),
            Child,                  DataAudio->getObject(),
         End,
         Child,                  Session->getObject(),
      End,
      Child,                  infogauge = (Object *)muiGauge("...", Btn_InfoGauge),
   End;

   return (unsigned long *)all;
}

void MUITracks::update()
{
   DataAudio->update();
   ISOBuilder->update();
   Session->update();
   layoutUpdate();
}

void MUITracks::layoutUpdate()
{
   IEngine *eng = Glb.CurrentEngine->ObtainRead();
   if (NULL != eng)
   {
      muiSetText(Btn_InfoGauge, eng->getLayoutInfo());
      uint32 s1=0, s2=0;

      s1 = eng->getDiscSize();
      switch (Glb.WriteSelection)
      {
         case Globals::Select_Tracks:
            s2 = eng->getLayoutSize();
            break;

         case Globals::Select_Session:
            s2 = 0;
            if (eng->getSessionContents() != 0)
               s2 = eng->getSessionContents()->getEndAddress();
            break;
      }
      
      if (s1 > 0)
      {
         while (s1 >  2048)
         {
            s1 >>= 1;
            s2 >>= 1;
         }  
         muiSetValue(Btn_InfoGauge, (s2*65535)/s1);
      }
      else
      {
         muiSetValue(Btn_InfoGauge, 0);
      }
   }
   Glb.CurrentEngine->Release();
}
   
void MUITracks::onPropChange(PropertyT<Globals::eWriteSelect>* p, const Globals::eWriteSelect & t)
{
   if (p == &Glb.WriteSelection)
      layoutUpdate();
}

void MUITracks::enableISO()
{
   ISOBuilder->enableISO();
}

void MUITracks::disableISO()
{
   ISOBuilder->disableISO();
}

uint32 MUITracks::onButton(BtnId id, void* option)
{
   switch (id)
   {
      case Btn_CompositionType:
         {
            IPTR sel = (IPTR)option;
            DoMtd((Object *)page, ARRAY(MUIM_Set, MUIA_Group_ActivePage, sel));
            if ((IPTR)option == 0)
               Glb.WriteSelection = Globals::Select_Tracks;
            else if ((IPTR)option == 1)
               Glb.WriteSelection = Globals::Select_Session;
         };
         break;

      case Btn_InfoGauge:
         break;
   }

   return 0;
}

