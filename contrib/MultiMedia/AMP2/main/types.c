/*
 *
 * types.c
 *
 */

#include <stdio.h>

#if defined(__AROS__)
#include "aros-inc.h"
#include <aros/macros.h>
#endif

#include "main.h"
#include "types.h"

#define ID_MPEG_1B3 0x000001B3
#define ID_MPEG_1BA 0x000001BA

#define ID_FORM  0x464F524D
#define ID_LIST  0x4C495354
#define ID_PROP  0x50524F50

#define ID_RIFF  0x52494646
#define ID_AVI   0x41564920

#define ID_FLI   0x000011AF
#define ID_FLC   0x000012AF

#define ID_QT_MOOV 0x6D6F6F76
#define ID_QT_MDAT 0x6D646174
#define ID_QT_SKIP 0x736B6970
#define ID_QT_FREE 0x66726565

#define ID_NSF1 0x4E45534d
#define ID_NSF2 0x1A000000

#define FOURCC(a,b,c,d) ((a << 24) | (b << 16) | (c << 8) | (d))

#define ID_RM FOURCC('.', 'R', 'M', 'F')

int get_type(char *filename)
{
  unsigned long data[4], d0 = 0, d2 = 0;
  int type = UNKNOWN;
  FILE *file;

  file = fopen(filename, "rb");

  if (file != NULL)
  {
    fread(data, sizeof(unsigned long), 4, file);
    fclose(file);

    debug_printf("HEADER: %08lx, %08lx, %08lx, %08lx\n", data[0], data[1], data[2], data[3]);

#if defined(__AROS__)
	data[0] = AROS_BE2LONG(data[0]);
	data[1] = AROS_BE2LONG(data[1]);
	data[2] = AROS_BE2LONG(data[2]);
	data[3] = AROS_BE2LONG(data[3]);

	debug_printf("DECODED HEADER: %08lx, %08lx, %08lx, %08lx\n", data[0], data[1], data[2], data[3]);
#endif	  

    switch(data[0])
    {
      case ID_RM:
        type = RM_SYSTEM;
        break;

      case ID_FORM:
      case ID_LIST:
      case ID_PROP:
        type = IFF_ANIM;
        break;

      case ID_RIFF:
        if (data[2] == ID_AVI)
          type = AVI_ANIM;
        break;

      default:
        d0 = (data[0] >> 16) & 0xFFFF;
        d2 = (data[1] >> 16) & 0xFFFF;

        if ((d2 == ID_FLI) || (d2 == ID_FLC))
          type = FLIC_ANIM;

        if ((data[1] == ID_QT_MOOV) ||
            ((data[1] == ID_QT_MDAT) && (data[0] != 0)) ||
            (data[1] == ID_QT_SKIP) ||
            (data[1] == ID_QT_FREE))
          type = QT_ANIM;

        break;
    }

    if ((data[0] == ID_NSF1) && ((data[1] & 0xff000000) == ID_NSF2)) {
      type = NSF_AUDIO;
    }

    if ((type == UNKNOWN) && ((data[0] >> 16) == 0x0b77)) {
      type = AC3_AUDIO;
    }

    /* Check if MPEG audio file */
    if (type == UNKNOWN) {
      if((data[0]&0xffe00000) == 0xffe00000) {
        if((data[0]>>17)&3) {
          if(((data[0]>>12)&0xf) != 0xf) {
            if(((data[0]>>10)&0x3) != 0x3) {
              type = MPEG_AUDIO;
            }
          }
        }
      }
    }

    /* Maybe it's an MPEG ? */
    if (type == UNKNOWN) {
      unsigned int startCode;
      file = fopen(filename, "rb");
      fread(&startCode, 1, 4, file);

#if defined(__AROS__)
	startCode = AROS_BE2LONG(startCode);
#endif

	    for (;;) {
        unsigned char byte;

        /* MPEG system/video stream */
        if ((startCode & 0xffffff00) == 0x00000100) {
          if ((startCode&0xff) == 0xb3) {
            type = MPEG_VIDEO;
            break;
          } else if ((startCode&0xff) == 0xba) {
            type = MPEG_SYSTEM;
            break;
          } else {
            type = UNKNOWN;
            break;
          }
        }

        if (fread(&byte, 1, 1, file) != 1) {
          break;
        }

        startCode = (startCode<<8)|byte;
      }

      fclose(file);
    }
  }
  else
  {
    amp_printf("File \"%s\" not found!\n", filename);
    return FAIL;
  }

  return type;
}
