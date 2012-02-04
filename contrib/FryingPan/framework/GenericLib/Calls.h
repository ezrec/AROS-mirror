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

#include <exec/libraries.h>

#undef PROC0
#undef PROC1
#undef PROC2
#undef PROC3
#undef PROC4
#undef PROC5
#undef PROC6
#undef PROC7
#undef PROC8
#undef PROC9
#undef PROC10
#undef PROC11
#undef PROC12
#undef FUNC0
#undef FUNC1
#undef FUNC2
#undef FUNC3
#undef FUNC4
#undef FUNC5
#undef FUNC6
#undef FUNC7
#undef FUNC8
#undef FUNC9
#undef FUNC10
#undef FUNC11
#undef FUNC12
#undef BEGINDECL
#undef BEGINSHORTDECL
#undef ENDDECL

extern class ExecIFace* Exec;

#if defined(__AMIGAOS4__)
   #include "Calls_os4.h"
#elif defined(__AROS__)
   #include "Calls_aros.h"
#elif defined(__MORPHOS__)
   #include "Calls_mos.h"
#else
   #include "Calls_os3.h"
#endif

