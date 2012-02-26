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

#ifndef _ENGINE_EVENT_H_
#define _ENGINE_EVENT_H_

#include <Generic/Generic.h>
#include "../IEngine.h"
#include "Engine.h"
#include "Jobs/Job.h"

class Event
{
public:
   enum EventType
   {
      Ev_Notify,
      Ev_Job,
      Ev_Action
   };
protected:
   EventType         type;
   EngineMessage     msg;
   Engine::Command   cmd;
   Job              *job;
public:
                     Event(EngineMessage m);
                     Event(Engine::Command m);
                     Event(Job *j);
   virtual          ~Event();                  
   EventType         getEventType();
   EngineMessage     getMessage();
   Engine::Command   getAction();
   Job              *getJob();
};

#endif
