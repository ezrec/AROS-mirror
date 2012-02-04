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

#include "Xlate.h"
#include "Xlators/Translator.h"
#include "Xlators/TransAChars.h"
#include "Xlators/TransDChars.h"
#include "Xlators/TransRChars.h"
#include "Xlators/TransQChars.h"


String Xlate::aChars(String &source)
{
   Translator *pTrans = new TransAChars();
   String      sNew   = pTrans->translate(source);
   delete pTrans;
   return sNew;
}

String Xlate::dChars(String &source)
{
   Translator *pTrans = new TransDChars();
   String      sNew   = pTrans->translate(source);
   delete pTrans;
   return sNew;
}

String Xlate::qChars(String &source)
{
   Translator *pTrans = new TransQChars();
   String      sNew   = pTrans->translate(source);
   delete pTrans;
   return sNew;
}

String Xlate::rChars(String &source)
{
   Translator *pTrans = new TransRChars();
   String      sNew   = pTrans->translate(source);
   delete pTrans;
   return sNew;
}
