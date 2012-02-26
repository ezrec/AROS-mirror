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

#ifndef GENERIC_HOOKT_H_
#define GENERIC_HOOKT_H_

/**
 * \file HookT.h
 * \brief Specialization for Utility \b Hooks. This is the Hook you are looking for.
 * \details This specialization allows calling regular, static and virtual methods of any class. It works well with derived and overloaded virtual methods as well.
 */

#include "Generic.h"
#include "IHook.h"

namespace GenNS
{
   /**
    * \brief A template class that takes three arguments to set up the resulting Hook - method proxy.
    * \details Parameter definition:
    * \arg BaseClass - a class that holds a method to call when hook fires,
    * \arg Arg1 - typename of the first parameter a method needs,
    * \arg Arg2 - typename of the second parameter a method needs.
    */
   template <class BaseClass, typename Arg1, typename Arg2>
      class HookT : public IHook
   {
   public:
      /**
       * \typedef uint (BaseClass::*TCall)(Arg1 a1, Arg2 a2)
       * \brief This specifies a type of the nonvirtual method that can be called by the specialized class when hook fires. You usually should have no interest in this.
       */
      typedef uint(BaseClass::*TCall)(Arg1 a1, Arg2 a2);

      /**
       * \typedef uint (*TStaticCall)(Arg1 a1, Arg2 a2)
       * \brief This specifies a type of the static method or function that can be called by the specialized class when hook fires. You usually should have no interest in this.
       */
      typedef uint(*TStaticCall)(Arg1 a1, Arg2 a2);
   
   private:

      TCall                   function;
      TStaticCall             staticfunction;
      BaseClass              *baseClass;

   public:

      virtual uint Call(uint a1, uint a2)
      {
         if (0 != baseClass)
         {
            return (baseClass->*function)((Arg1)a1, (Arg2)a2);
         }
         else if (0 != staticfunction)
         {
            return (staticfunction)((Arg1)a1, (Arg2)a2);
         }
         return 0;
      }

   public:

      /**
       * \brief Call this function with the pointer to baseclass and parameter to initialize the hook \b before first use
       * \details example usage:
       * \code
       * class myHookCall
       * {
       *    HookT<myHookCall, someClass1*, someClass2*> hook;
       * public:
       *    myHookCall()
       *    {
       *       hook.Initialize(this, &myHookCall::call);
       *    }
       *
       *    uint call(someClass1* a1, someClass2* a2)
       *    {
       *       // ...
       *       return 0;
       *    }
       * }
       * \endcode
       * \sa InitializeStatic(TStaticCall)
       */
      void Initialize(BaseClass *pBase, TCall pFunction)
      {
         function          =  pFunction;
         baseClass         =  pBase;
         staticfunction    =  0;
      };                        

      /**
       * \brief Call this function with the pointer to baseclass and parameter to initialize the hook \b before first use
       * \details example usage:
       * \code
       * class myHookCall
       * {
       *    HookT<myHookCall, someClass1*, someClass2*> hook;
       * public:
       *    myHookCall()
       *    {
       *       hook.InitializeStatic(&myHookCall::call);
       *    }
       *
       *    static uint call(someClass1* a1, someClass2* a2)
       *    {
       *       // ...
       *       return 0;
       *    }
       * }
       * \endcode
       * \sa Initialize(BaseClass*, TCall)
       */
      void InitializeStatic(TStaticCall pFunction)
      {
         function          =  0;
         baseClass         =  0;
         staticfunction    =  pFunction;
      }

      /**
       * \brief This constructor requires calling no other method and allows you to specify the dynamic function right upon construction.
       * \details An example usage of this constructor is:
       * \code
       * class myHookCall
       * {
       *    HookT<myHookCall, someClass1*, someClass2*> hook;
       * public:
       *    myHookCall() :
       *       hook(this, &myHookCall::call)
       *    {
       *    }
       *
       *    uint call(someClass1* a1, someClass2* a2)
       *    {
       *       // ...
       *       return 0;
       *    }
       * }
       * \endcode
       * \sa Initialize(BaseClass*, TCall)
       */
      HookT(BaseClass *pBase, TCall pFunction)
      {
         Initialize(pBase, pFunction);
      }

      /**
       * \brief This constructor requires calling no other method and allows you to specify the static function right upon construction.
       * \details An example usage of this constructor is:
       * \code
       * class myHookCall
       * {
       *    HookT<myHookCall, someClass1*, someClass2*> hook;
       * public:
       *    myHookCall() :
       *       hook(&myHookCall::call)
       *    {
       *    }
       *
       *    static uint call(someClass1* a1, someClass2* a2)
       *    {
       *       // ...
       *       return 0;
       *    }
       * }
       * \endcode
       * \sa InitializeStatic(TStaticCall)
       */
      HookT(TStaticCall pFunction)
      {
         InitializeStatic(pFunction);
      }

      HookT()
      {
         Initialize((BaseClass*)0, (TCall)0);
      }

      virtual ~HookT()
      {
      }
   };
};

#endif /*HOOKT_H_*/
