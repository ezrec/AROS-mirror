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

#include "Event.h"

Event::Event(EngineMessage m)
{
   msg = m;
   job = 0;
   cmd = Engine::Cmd_NoOperation;
   type = Ev_Notify;
}

Event::Event(Engine::Command m)
{
   msg = Eng_Update;
   job = 0;
   cmd = m;
   type = Ev_Action;
}

Event::Event(Job *j)
{
   job = j;
   msg = Eng_Update;
   cmd = Engine::Cmd_NoOperation;
   type = Ev_Job;
}

Event::~Event()
{
   if (job)
      delete job;
}

Event::EventType Event::getEventType()
{
   return type;
}

EngineMessage Event::getMessage()
{
   return msg;
}

Engine::Command Event::getAction()
{
   return cmd;
}

Job* Event::getJob()
{
   return job;
}

