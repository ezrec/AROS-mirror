
// Rar_Extractor 0.2.1. See unrar_license.txt.

#ifndef RAR_COMMON_HPP
#define RAR_COMMON_HPP

#ifdef HAVE_CONFIG_H
	#undef RAR_HPP
	#include "config.h"
	#define RAR_HPP
#endif

#include <new>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// Max filename length
const int NM = 1024;

typedef unsigned char    byte;   //8 bits
typedef unsigned short   ushort; //preferably 16 bits, but can be more
typedef unsigned int     uint;   //32 bits or more

typedef unsigned long    uint32; //32 bits exactly
typedef          long    sint32; //signed 32 bits exactly

#define SHORT16(x) (sizeof(ushort)==2 ? (ushort)(x):((x)&0xffff))
#define UINT32(x)  (sizeof(uint32)==4 ? (uint32)(x):((x)&0xffffffff))

#define  Min(x,y) (((x)<(y)) ? (x):(y))

typedef long long Int64;

#define int64to32(x) ((uint)(x))
#define int32to64(high,low) ((((Int64)(high))<<32)+(low))
#define is64plus(x) (x>=0)

#define INT64MAX int32to64(0x7fffffff,0)

#include "Rar_Extractor.h"

typedef File_Reader Rar_Reader;
typedef Data_Writer Rar_Writer;

#include "array.hpp"
#include "headers.hpp"
#include "crc.hpp"
#include "getbits.hpp"
#include "rdwrfn.hpp"
#include "archive.hpp"
#include "rawread.hpp"
#include "compress.hpp"
#include "rarvm.hpp"
#include "suballoc.hpp"
#include "model.hpp"
#include "unpack.hpp"
#include "rs.hpp"

#endif

