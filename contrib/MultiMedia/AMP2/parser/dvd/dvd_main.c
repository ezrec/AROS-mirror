/**
 * Copyright (C) 2001 Billy Biggs <vektor@dumbterm.net>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(__AROS__)
#include <powerpc/powerpc_protos.h>
#else
#include "aros-inc.h"
#endif


#include "dvdnav/dvdnav.h"
#include "dvdread/nav_read.h"
#include "dvdread/ifo_types.h"
#include "dvdread/dvd_reader.h"

#include "../mpeg/mpeg.h"
#include "../../main/main.h"
#include "../../main/buffer.h"
#include "../../main/prefs.h"
#include "../../common/core.h"
#include "../../refresh/osd.h" /* FIXME */

#include "spu.h"
extern unsigned char menu_color[4]; /* spu.c */
extern unsigned char menu_trans[4]; /* spu.c */

/* amigaos/video.c */
extern int dvd_input_hack; /* Actual AmigaOS RAWKEY code when key is released */

/* FIXME: Remove !!! */
extern unsigned char **pre_src;
extern double pre_sync_time;
#include "../../refresh/refresh.h"

extern int dvd_hack; /* FIXME: Remove */
extern int dvd_menu, dvd_x, dvd_y, dvd_w, dvd_h;

static dvdnav_t *dvdnav;
static unsigned char buf[2050];

void dvd_main(void)
{
  int region, i, finished;
  int event, len, previous_button = -1;

int buttons = 0;

// MARO
     pci_t *pci;
     dsi_t *dsi;
     btni_t *btni;

unsigned long clut[16];

  int decoders_running = 0, size, pos, type;
  int no_video, no_audio;
  unsigned char *ptr, byte;
  double pts;
int id, pos_add, update_button_pos = 0;

int pause_timer = 0, pause_counter = 0;


  if (plugin_init() != CORE_OK) {
    return;
  }


dvd_menu = 1; /* FIXME */


  no_video = prefs.no_video;
  no_audio = prefs.no_audio;

  dvd_hack = 1; /* FIXME */

  printf("Opening DVD...\n");
  
  if (dvdnav_open(&dvdnav, "/dev/dvd") != DVDNAV_STATUS_OK) {
    printf("error opening DVD\n");
    return;
  }

  printf("open done\n");

  memset(clut, 0, sizeof(clut));

  subtitle_fopen(); /* AmiDog */

  strcat(prefs.window_title, "DVD");

  region = 0;
  dvdnav_get_region_mask(dvdnav, &region);
  printf("The DVD can be played in the following regions: ");
  for(i=0; i<8; i++) {
    if(region & (1<<i)) {
      printf("%i ", i+1);
    }
  }
  printf("\n");

  printf("Reading...\n");
  finished = 0;
  while(!finished) {
    int result = dvdnav_get_next_block(dvdnav, buf,
                           &event, &len);

    if(result == DVDNAV_STATUS_ERR) {
      printf("Error getting next block (%s)\n", dvdnav_err_to_string(dvdnav));
      goto get_out;
    }

    switch(event) {
     case DVDNAV_BLOCK_OK:
{
      pause_timer = 0; // no longer paused !!!

      ptr = &buf[0];

#define AC3_STREAM_ID   0xbd

      type = -1;
      pos_add = 0;

      switch (ptr[17]) {
        case VIDEO_STREAM_ID:
          if (no_video == PREFS_OFF) {
            type = 0;
          }
        break;

        case AUDIO_STREAM_ID:
          if (no_audio == PREFS_OFF) {
            type = 1;
          }
        break;

        case AC3_STREAM_ID:
          id = ptr[ptr[22] + 23]; /* Special "ID" */

/*
printf("ID: %d\n", id);
*/

          if (id == 0x20) {
            pos_add = 1;
            type = 2;
          }

#if 0
          /* 0x80 | TRACK */
          prefs.ac3_track = 0x80 + audio_track;
          /* 0x20 | TRACK */
          prefs.subtitle_track = 0x20 + subtitle_track;

          id = ptr[ptr[22] + 23]; /* Special "ID" */
          if (id > 0) {
            if (id == prefs.ac3_track) {
              printf("AC3 FOUND\n");
              if (tasks & AC3_TASK) {
                type = 1;
                pos_add = 4;
              }
            } else if (id == prefs.subtitle_track) {
              printf("SUBTITLE FOUND\n");
              type = 2;
              pos_add = 1;
            }
          }
#endif
          break;

        default:
          break;
      }

      if (type < 0) {
        break;
      }

      size = (ptr[18] << 8) + ptr[19];
      pts = -1.0;

      byte = ptr[21];

      /* 0x02 or 0x03 */
      if ((byte >> 6) >= 0x02) {
        /* 0x02 => 23 + 5 * 0 = 23, 0x03 => 23 + 5 * 1 = 28 */
        pos = 23 + 5 * ((byte >> 6) & 0x01);
        pts = BUF2PTS(&ptr[pos]);
      }

      pos = ptr[22] + 3 + pos_add;
      size -= pos;
      pos += 20;

      while (amp_fwrite(type, ptr + pos, size, pts, 0) == BUF_FALSE) {

/* Warning: Do not wait for subtitle */
if (type == 2) {
  break;
}

        /* Buffer full, can't write */
        if (decoders_running == 0) {
          if (no_video == PREFS_OFF) {
            if (plugin_video_init(SUBTYPE_MPEGX, FOURCC_NONE, 0, 0, 0, 0.0) != CORE_OK) {
              no_video = PREFS_ON;
            }
          }

          if (no_audio == PREFS_OFF) {
            if (plugin_audio_init(SUBTYPE_MPEGX, FOURCC_NONE, 0, 0, 0) != CORE_OK) {
              no_audio = PREFS_ON;
            }
          }

          if ((no_video == PREFS_ON) && (no_audio == PREFS_ON)) {
            goto get_out; /* EXIT */
          }

          if (plugin_start() != CORE_OK) {
            goto get_out; /* EXIT */
          }
          decoders_running = 1;
        }
      }
}

      break;
     case DVDNAV_NOP:
      /* Nothing */
      break;
     case DVDNAV_STILL_FRAME: 
       {
         dvdnav_still_event_t *still_event = (dvdnav_still_event_t*)buf;

         if (pause_timer == 0) {
           printf("stillframe: %d\n", still_event->length);
           pause_timer = still_event->length; // seconds to pause...
           pause_counter = 0;

           printf("refresh\n");
           refresh_image(pre_src, pre_sync_time);

           break;
         }

         if (pause_timer == 0xff) {
           // Wait forever
           pause_counter++;
           break;
         }

         if ((pause_timer != 0xff) && (pause_counter >= pause_timer)) { // FIXME !!!
           printf("Skipping still frame\n");
           dvdnav_still_skip(dvdnav);
           pause_timer = 0;
           break;
         }

         if (pause_timer) {
           pause_counter++;
           break;
         }
       }
      break;
     case DVDNAV_SPU_CLUT_CHANGE:
       {
unsigned char *ptr;

       printf("clut change\n");
       memcpy(clut, buf, 16*4);

/* FIXME */
spu_palette(buf);
/* FIXME */

ptr = buf;

for (i=0; i<16; i++) {
  int y, u, v, r, g, b;
  int crv, cbu, cgu, cgv;

  #define CLIP(i) ((i)<0) ? 0 : (((i)>255) ? 255 : (i))

  y = 76309 * (ptr[1] - 16); /* (255/219)*65536 */
  u = ptr[2] - 128;
  v = ptr[3] - 128;

  crv = 104597;
  cbu = 132201;
  cgu = 25675;
  cgv = 53279;

  r = (y + crv*v + 32768)>>16;
  g = (y - cgu*u - cgv*v + 32768)>>16;
  b = (y + cbu*u + 32786)>>16;

  r = CLIP(r);
  g = CLIP(g);
  b = CLIP(b);

  printf("%2d : %02x %02x %02x -> %02x %02x %02x\n", i, ptr[1], ptr[2], ptr[3], r, g, b);

  ptr += 4;
}

       }
      break;
     case DVDNAV_SPU_STREAM_CHANGE:
       {
       dvdnav_spu_stream_change_event_t *stream_event = (dvdnav_spu_stream_change_event_t*)buf;
       printf("SPU stream change to : %d, %d, %d\n", stream_event->physical_wide, stream_event->physical_letterbox, stream_event->physical_pan_scan);
#if 0
     dvdnav_spu_stream_change_event_t *stream_event = 
       (dvdnav_spu_stream_change_event_t*) (block);
        buf->content = block;
        buf->type = BUF_CONTROL_SPU_CHANNEL;
        buf->decoder_info[0] = stream_event->physical_wide;
     buf->decoder_info[1] = stream_event->physical_letterbox;
     buf->decoder_info[2] = stream_event->physical_pan_scan;
     dprint("SPU stream wide %d, letterbox %d, pan&scan %d\n",
       stream_event->physical_wide,
       stream_event->physical_letterbox,
       stream_event->physical_pan_scan);
     finished = 1;
#endif
       }
      break;
     case DVDNAV_AUDIO_STREAM_CHANGE:
       {
       dvdnav_audio_stream_change_event_t *stream_event = (dvdnav_audio_stream_change_event_t*)buf;
       printf("AUDIO stream change to : %d\n", stream_event->physical);
#if 0
     dvdnav_audio_stream_change_event_t *stream_event = 
      (dvdnav_audio_stream_change_event_t*) (block);
        buf->content = block;
        buf->type = BUF_CONTROL_AUDIO_CHANNEL;
        buf->decoder_info[0] = stream_event->physical;
     dprint("AUDIO stream %d\n", stream_event->physical);
     finished = 1;
#endif
       }
      break;
     case DVDNAV_HIGHLIGHT:
       {
       printf("Highlight\n");
       }
      break;
     case DVDNAV_VTS_CHANGE:
       {
       printf("VTS change\n");
       }
      break;
     case DVDNAV_CELL_CHANGE:
       {
     int tt,pt;
     dvdnav_current_title_info(dvdnav, &tt, &pt);
     printf("Cell change... (title, chapter) = (%i,%i)\n", tt,pt);
//     buttons = 0;
       }
      break;
     case DVDNAV_SEEK_DONE:
       {
       printf("seek done\n");
       }
      break;
     case DVDNAV_HOP_CHANNEL:
       {
       printf("hop channel\n");
       buttons = 0;
previous_button = -1;
       }
      break;
     case DVDNAV_NAV_PACKET:
       {
       pci = dvdnav_get_current_nav_pci(dvdnav);
       dsi = dvdnav_get_current_nav_dsi(dvdnav);

       if(pci->hli.hl_gi.btn_ns > 0) {
         if (buttons != pci->hli.hl_gi.btn_ns) {
           update_button_pos = 1;
           buttons = pci->hli.hl_gi.btn_ns;
           printf("Found %i buttons...\n", pci->hli.hl_gi.btn_ns);

dvdnav_button_select(dvdnav, 1); /* HUH ? */
previous_button = -1;

         }
       }
       }
      break;
     case DVDNAV_STOP:
       {
     finished = 1;
       }
     default:
      printf("Unknown event (%i)\n", event);
      finished = 1;
      break;
    }


    if (decoders_running) {
      int check;
      check = core_input(-1);

      if (check == INPUT_QUIT) {
        finished = 1; /* EXIT */
      }

#define KEY_LEFT 79
#define KEY_RIGHT 78
#define KEY_UP 76
#define KEY_DOWN 77
#define KEY_ENTER 68

      switch(dvd_input_hack) {
        case KEY_LEFT:
          dvdnav_left_button_select(dvdnav);
          break;

        case KEY_RIGHT:
          dvdnav_right_button_select(dvdnav);
          break;

        case KEY_UP:
          dvdnav_upper_button_select(dvdnav);
          break;

        case KEY_DOWN:
          dvdnav_lower_button_select(dvdnav);
          break;

        case KEY_ENTER:
          dvdnav_button_activate(dvdnav);
          break;

        default:
          break;
      }

#if 0
      switch(check) {
        case KEY_LEFT:
        case KEY_RIGHT:
        case KEY_UP:
        case KEY_DOWN:
        if (buttons > 0) {
          update_button_pos = 1;
        }
          break;

        default:
          break;
      }
#endif
    }

#define TEST

#if 1
  do {
          int button;

unsigned long clip_color[4], clip_trans[4];

          update_button_pos = 0;

          dvdnav_get_current_highlight(dvdnav, &button);

if (button == 0) {
  dvd_x = -1;
  dvd_y = -1;
  previous_button = -1;
  break;
}

          button--; /* */

if (button == previous_button) {
  break;
}
previous_button = button;

  pci = dvdnav_get_current_nav_pci(dvdnav);
  btni = &(pci->hli.btnit[button]);

          printf("Button %i top-left @ (%i,%i), bottom-right @ (%i,%i)\n", 
              button+1, btni->x_start, btni->y_start,
              btni->x_end, btni->y_end);
          dvd_x = btni->x_start;
          dvd_y = btni->y_start;
          dvd_w = btni->x_end - dvd_x + 1;
          dvd_h = btni->y_end - dvd_y + 1;


    if(btni->btn_coln != 0) {
      printf("normal button clut\n");
      for (i = 0;i < 4; i++) {
        int mode = 0; /* FIXME: What is this ? */
//      clip_color[i] = clut[0xf & (nav_pci->hli.btn_colit.btn_coli[button_ptr->btn_coln-1][mode] >> (16 + 4*i))];
//      clip_trans[i] = 0xf & (nav_pci->hli.btn_colit.btn_coli[button_ptr->btn_coln-1][mode] >> (4*i));

        clip_color[i] = 0xf & (pci->hli.btn_colit.btn_coli[btni->btn_coln-1][mode] >> (16 + 4*i));
        clip_trans[i] = 0xf & (pci->hli.btn_colit.btn_coli[btni->btn_coln-1][mode] >> (4*i));

/* FIXME */
menu_color[i] = clip_color[i];
menu_trans[i] = clip_trans[i];
/* FIXME */

printf("%d: color %ld alpha %ld\n", i, clip_color[i], clip_trans[i]);

        clip_color[i] = clut[clip_color[i]] & 0x00ffffff;
        clip_color[i] |= (((clip_trans[i] * 0xff) / 0x0f) << 24);
      }
    } else {
      printf("abnormal button clut\n");
      for (i = 0;i < 4; i++) {
//      overlay->clip_color[i] = overlay->color[i];
//      overlay->clip_trans[i] = overlay->trans[i];
      }
    }

    /* FIXME */
//    osd_init_palette(NULL, clip_color, 4);


// FIXME: !!!
if (pause_timer) {
  printf("refresh\n");
  refresh_image(pre_src, pre_sync_time);
}

  } while(0);
#endif

  /* disable button */
  if (buttons <= 0) {
    dvd_x = -1;
    dvd_y = -1;
  }

  }

get_out:

  plugin_exit();

  dvdnav_close(dvdnav);


subtitle_fclose(); /* AmiDog */

}
