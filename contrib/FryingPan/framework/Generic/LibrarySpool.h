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

#ifndef _LIBRARYSPOOL_H_
#define _LIBRARYSPOOL_H_
#include "Generic.h"

/** 
 * \file LibrarySpool.h 
 * \brief This class handles the library manipulation - actually very much reduced due to recent implementations. \b Deprecated.
 * \details This file defines a very very basic singleton object that frees you from opening the four libraries:
 * - Exec
 * - DOS
 * - Intuition
 * - Utility
 *
 * on your own. Since it is a singleton, you will want to call it as:
 * \code LibrarySpool::Init(); \endcode
 * at the beginning of your code and then - when you're done - just call
 * \code LibrarySpool::Exit(); \endcode
 * \n
 * As this class is deprecated, you should access the libraries you need yourself by using:
 * \code
 * // declare the bases:
 * class ExecIFace *Exec;
 * class DOSIFace *DOS;
 * class IntuitionIFace *Intuition;
 * class UtilityIFace *Utility;
 *
 * ...
 *
 * // open the libraries at the beginning of your code
 * Exec = ExecIFace::GetInstance();
 * DOS = DOSIFace::GetInstance(37);
 * Intuition = IntuitionIFace::GetInstance(37);
 * Utility = UtilityIFace::GetInstance(37);
 * 
 * // do your stuff here..
 * 
 * // close the libraries at the end
 * Utility->FreeInstance();
 * Intuition->FreeInstance();
 * DOS->FreeInstance();
 * Exec->FreeInstance();
 * \endcode
 */

//! Utility library access interface
extern class UtilityIFace *Utility;
//! DOS library access interface
extern class DOSIFace *DOS;
//! Exec library access interface
extern class ExecIFace *Exec;
//! Intuition library access interface
extern class IntuitionIFace *Intuition;

namespace GenNS
{
   /**
    * \class LibrarySpool
    * \brief Singleton object - Use method ::Init() to initialize library bases and obtain the pointer to the object. 
    * Use method ::Exit() to clean up libraries and free the object. 
    * Class is \b deprecated.
    */
   class LibrarySpool 
   {
      static LibrarySpool    *LibSpool;

   protected:
      LibrarySpool();
      virtual                ~LibrarySpool();

   public:
      /**
       * \fn static LibrarySpool *Init();
       * Use this method at the beginning of your code to initialize library access interfaces and obtain the pointer to the singleton object. 
       * Do \b NOT call more than once.
       */
      static LibrarySpool    *Init();

      /**
       * \fn static void Exit();
       * Use this method at the end of your code to dispose library access interfaces and the class itself.
       * Do \b NOT call more than once.
       */
      static void             Exit();
   };
};
#endif //_LIBRARYSPOOL_H_
