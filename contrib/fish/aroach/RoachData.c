/**********************************************************************/
/*                                                                    */
/*                            RoachData.c                             */
/*                                                                    */
/*                    Object file for roach images                    */
/*     © Copyright Stefan Winterstein 1993. Freely Distributable.     */
/*                                                                    */
/**********************************************************************/


#include "ARoach.h"

/* for AROS only */
#define __chip

#include "xbm/roach000.xbm"
#include "xbm/roach015.xbm"
#include "xbm/roach030.xbm"
#include "xbm/roach045.xbm"
#include "xbm/roach060.xbm"
#include "xbm/roach075.xbm"
#include "xbm/roach090.xbm"
#include "xbm/roach105.xbm"
#include "xbm/roach120.xbm"
#include "xbm/roach135.xbm"
#include "xbm/roach150.xbm"
#include "xbm/roach165.xbm"
#include "xbm/roach180.xbm"
#include "xbm/roach195.xbm"
#include "xbm/roach210.xbm"
#include "xbm/roach225.xbm"
#include "xbm/roach240.xbm"
#include "xbm/roach255.xbm"
#include "xbm/roach270.xbm"
#include "xbm/roach285.xbm"
#include "xbm/roach300.xbm"
#include "xbm/roach315.xbm"
#include "xbm/roach330.xbm"
#include "xbm/roach345.xbm"

#include "xbm/squish.xbm"

RoachMap roachPix[] = {
	{roach000_bits, roach000_height, roach000_width, 0.0, 0.0},
	{roach015_bits, roach015_height, roach015_width, 0.0, 0.0},
	{roach030_bits, roach030_height, roach030_width, 0.0, 0.0},
	{roach045_bits, roach045_height, roach045_width, 0.0, 0.0},
	{roach060_bits, roach060_height, roach060_width, 0.0, 0.0},
	{roach075_bits, roach075_height, roach075_width, 0.0, 0.0},
	{roach090_bits, roach090_height, roach090_width, 0.0, 0.0},
	{roach105_bits, roach105_height, roach105_width, 0.0, 0.0},
	{roach120_bits, roach120_height, roach120_width, 0.0, 0.0},
	{roach135_bits, roach135_height, roach135_width, 0.0, 0.0},
	{roach150_bits, roach150_height, roach150_width, 0.0, 0.0},
	{roach165_bits, roach165_height, roach165_width, 0.0, 0.0},
	{roach180_bits, roach180_height, roach180_width, 0.0, 0.0},
	{roach195_bits, roach195_height, roach195_width, 0.0, 0.0},
	{roach210_bits, roach210_height, roach210_width, 0.0, 0.0},
	{roach225_bits, roach225_height, roach225_width, 0.0, 0.0},
	{roach240_bits, roach240_height, roach240_width, 0.0, 0.0},
	{roach255_bits, roach255_height, roach255_width, 0.0, 0.0},
	{roach270_bits, roach270_height, roach270_width, 0.0, 0.0},
	{roach285_bits, roach285_height, roach285_width, 0.0, 0.0},
	{roach300_bits, roach300_height, roach300_width, 0.0, 0.0},
	{roach315_bits, roach315_height, roach315_width, 0.0, 0.0},
	{roach330_bits, roach330_height, roach330_width, 0.0, 0.0},
	{roach345_bits, roach345_height, roach345_width, 0.0, 0.0},
};

RoachMap squish = {squish_bits, squish_width, squish_height, 0.0, 0.0};

unsigned char ReverseBits [] = {
     0, 128, 64, 192, 32, 160, 96, 224, 16, 144, 80, 208, 48, 176, 112, 240,
     8, 136, 72, 200, 40, 168, 104, 232, 24, 152, 88, 216, 56, 184, 120, 248,
     4, 132, 68, 196, 36, 164, 100, 228, 20, 148, 84, 212, 52, 180, 116, 244,
     12, 140, 76, 204, 44, 172, 108, 236, 28, 156, 92, 220, 60, 188, 124, 252,
     2, 130, 66, 194, 34, 162, 98, 226, 18, 146, 82, 210, 50, 178, 114, 242,
     10, 138, 74, 202, 42, 170, 106, 234, 26, 154, 90, 218, 58, 186, 122, 250,
     6, 134, 70, 198, 38, 166, 102, 230, 22, 150, 86, 214, 54, 182, 118, 246,
     14, 142, 78, 206, 46, 174, 110, 238, 30, 158, 94, 222, 62, 190, 126, 254,
     1, 129, 65, 193, 33, 161, 97, 225, 17, 145, 81, 209, 49, 177, 113, 241,
     9, 137, 73, 201, 41, 169, 105, 233, 25, 153, 89, 217, 57, 185, 121, 249,
     5, 133, 69, 197, 37, 165, 101, 229, 21, 149, 85, 213, 53, 181, 117, 245,
     13, 141, 77, 205, 45, 173, 109, 237, 29, 157, 93, 221, 61, 189, 125, 253,
     3, 131, 67, 195, 35, 163, 99, 227, 19, 147, 83, 211, 51, 179, 115, 243,
     11, 139, 75, 203, 43, 171, 107, 235, 27, 155, 91, 219, 59, 187, 123, 251,
     7, 135, 71, 199, 39, 167, 103, 231, 23, 151, 87, 215, 55, 183, 119, 247,
     15, 143, 79, 207, 47, 175, 111, 239, 31, 159, 95, 223, 63, 191, 127, 255
};

