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

#ifndef _REGHOOKSKELETON_H_
#define _REGHOOKSKELETON_H_

#include "DTLib.h"

class IFileReader;
class IFileWriter;
class IOptItem;


class IRegHook
{
public:
    virtual bool            checkFile(const char* fileName, EDtError &rc)                          = 0;
    virtual IFileReader    *openRead(const char* fileName, EDtError &rc)                           = 0;
    virtual IFileWriter    *openWrite(const char* fileName, const IOptItem* item, EDtError &rc)    = 0;
    virtual const char     *getName()                                                              = 0;
    virtual signed char     getPriority()                                                          = 0;
    virtual bool            isReader()                                                             = 0;
    virtual bool            isWriter()                                                             = 0;
    virtual bool            isAudio()                                                              = 0;
    virtual bool            isData()                                                               = 0;
    virtual bool            isSession()                                                            = 0;
};

#endif

