/* 
 * Copyright (C) 2000 Rich Wareham <richwareham@users.sourceforge.net>
 * 
 * This file is part of libdvdnav, a DVD navigation library.
 * 
 * libdvdnav is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * libdvdnav is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
 *
 * $Id$
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/*
#define LOG_DEBUG
*/

#include <pthread.h>
#include <dvdnav.h>
#include "dvdnav_internal.h"
#include "read_cache.h"

#include <dvdread/nav_read.h>

#include <stdlib.h>
#include <stdio.h>

/*
 * NOTE:
 *      All NLCK_*() function are not mutex locked, this made them reusable in
 *      a locked context. Take care.
 *
 */

/* Current domain (backend to dvdnav_is_domain_() funcs) */
static int8_t NLCK_dvdnav_is_domain(dvdnav_t *this, domain_t domain) {
  dvd_state_t  *state;
  
  if((!this) || (!this->started) || (!this->vm))
    return -1;
  
  state = &(this->vm->state);

  if(!state)
    return -1;

  return (state->domain == domain) ? 1 : 0;
}

static int8_t _dvdnav_is_domain(dvdnav_t *this, domain_t domain) {
  int8_t        retval;
  
  pthread_mutex_lock(&this->vm_lock); 
  retval = NLCK_dvdnav_is_domain(this, domain);
  pthread_mutex_unlock(&this->vm_lock);
  
  return retval;
}

static int8_t NCLK_dvdnav_get_audio_logical_stream(dvdnav_t *this, uint8_t audio_num) {
  dvd_state_t *state;
  int8_t       logical = -1;
  
  if(!NLCK_dvdnav_is_domain(this, VTS_DOMAIN))
    audio_num = 0;
  
  state = &(this->vm->state);
  
  if(audio_num < 8) {
    if(state->pgc->audio_control[audio_num] & (1 << 15)) {
      logical = (state->pgc->audio_control[audio_num] >> 8) & 0x07;  
    }
  }
  
  return logical;
}

static int8_t NCLK_dvdnav_get_spu_logical_stream(dvdnav_t *this, uint8_t subp_num) {
  dvd_state_t   *state;
  ifo_handle_t  *vtsi;
  
  if(!this)
    return -1;
  
  state = &(this->vm->state);
  vtsi  = this->vm->vtsi;
  
  if(subp_num >= vtsi->vtsi_mat->nr_of_vts_subp_streams)
    return -1;
  
  return vm_get_subp_stream(this->vm, subp_num, 0);
}

static int8_t NLCK_dvdnav_get_active_spu_stream(dvdnav_t *this) {
  dvd_state_t  *state;
  int8_t        subp_num;
  int           stream_num;
  
  state      = &(this->vm->state);
  subp_num   = state->SPST_REG & ~0x40;
  stream_num = NCLK_dvdnav_get_spu_logical_stream(this, subp_num);
  
  if(stream_num == -1)
    for(subp_num = 0; subp_num < 32; subp_num++)
      if(state->pgc->subp_control[subp_num] & (1 << 31)) {
	stream_num = NCLK_dvdnav_get_spu_logical_stream(this, subp_num);
	break;
      }
  
  return stream_num;
}

uint8_t dvdnav_get_video_aspect(dvdnav_t *this) {
  uint8_t         retval;
  
  pthread_mutex_lock(&this->vm_lock); 
  retval = (uint8_t) vm_get_video_aspect(this->vm);
  pthread_mutex_unlock(&this->vm_lock); 
  
  return retval;
}

uint8_t dvdnav_get_video_scale_permission(dvdnav_t *this) {
  uint8_t         retval;
  
  pthread_mutex_lock(&this->vm_lock); 
  retval = (uint8_t) vm_get_video_scale_permission(this->vm);
  pthread_mutex_unlock(&this->vm_lock); 
  
  return retval;
}

dvdnav_status_t dvdnav_clear(dvdnav_t * this) {
  if (!this) {
    printerr("Passed a NULL pointer");
    return S_ERR;
  }
  /* clear everything except path, file, vm, mutex, readahead */

  /* path */
  if (this->file) DVDCloseFile(this->file);
  this->file = NULL;
  this->open_vtsN = -1;
  this->open_domain = -1;

  memset(&this->pci,0,sizeof(this->pci));
  memset(&this->dsi,0,sizeof(this->dsi));

  /* Set initial values of flags */
  this->position_current.still = 0;
  this->skip_still = 0;
  this->stop = 0;
  this->spu_clut_changed = 0;
  this->started=0;
  /* this->use_read_ahead */

  dvdnav_read_cache_clear(this->cache);
  
  return S_OK;
}

dvdnav_status_t dvdnav_open(dvdnav_t** dest, char *path) {
  dvdnav_t *this;
  
  /* Create a new structure */
  (*dest) = NULL;
  this = (dvdnav_t*)malloc(sizeof(dvdnav_t));
  if(!this)
   return S_ERR;
  memset(this, 0, (sizeof(dvdnav_t) ) ); /* Make sure this structure is clean */

  pthread_mutex_init(&this->vm_lock, NULL);
  /* Initialise the error string */
  printerr("");

  /* Initialise the VM */
  this->vm = vm_new_vm();
  if(!this->vm) {
    printerr("Error initialising the DVD VM");
    return S_ERR;
  }
  if(vm_reset(this->vm, path) == -1) {
    printerr("Error starting the VM / opening the DVD device");
    return S_ERR;
  }

  /* Set the path. FIXME: Is a deep copy 'right' */
  strncpy(this->path, path, MAX_PATH_LEN);

  dvdnav_clear(this);
 
  /* Pre-open and close a file so that the CSS-keys are cached. */
  this->file = DVDOpenFile(vm_get_dvd_reader(this->vm), 0, DVD_READ_MENU_VOBS);
  if (this->file) DVDCloseFile(this->file);
  this->file = NULL;
    
  //if(!this->started) {
  //  /* Start the VM */
  //  vm_start(this->vm);
  //  this->started = 1;
  //}

  /* Start the read-ahead cache. */
  this->cache = dvdnav_read_cache_new(this);
  
  (*dest) = this;
  return S_OK;
}

dvdnav_status_t dvdnav_close(dvdnav_t *this) {
  if(!this) {
    printerr("Passed a NULL pointer");
    return S_ERR;
  }
#ifdef LOG_DEBUG
  fprintf(stderr,"dvdnav:close:called\n");
#endif

  if (this->file) {
    DVDCloseFile(this->file);
#ifdef LOG_DEBUG
    fprintf(stderr,"dvdnav:close:file closing\n");
#endif
    this->file = NULL;
  }

  /* Free the VM */
  if(this->vm) {
    vm_free_vm(this->vm);
  }
  if (this->file) {
    DVDCloseFile(this->file);
#ifdef LOG_DEBUG
    fprintf(stderr,"dvdnav:close2:file closing\n");
#endif
    this->file = NULL;
  }
  pthread_mutex_destroy(&this->vm_lock);

  /* We leave the final freeing of the entire structure to the cache,
   * because we don't know, if there are still buffers out in the wild,
   * that must return first. */
  if(this->cache) {
    dvdnav_read_cache_free(this->cache);
  } else free(this);
  
  return S_OK;
}

dvdnav_status_t dvdnav_reset(dvdnav_t *this) {
  dvdnav_status_t result;

#ifdef LOG_DEBUG
  printf("dvdnav:reset:called\n");
#endif
  if(!this) {
    printerr("Passed a NULL pointer");
    return S_ERR;
  }
#ifdef LOG_DEBUG
  printf("getting lock\n");
#endif
  pthread_mutex_lock(&this->vm_lock); 
#ifdef LOG_DEBUG
  printf("reseting vm\n");
#endif
  if(vm_reset(this->vm, NULL) == -1) {
    printerr("Error restarting the VM");
    pthread_mutex_unlock(&this->vm_lock); 
    return S_ERR;
  }
#ifdef LOG_DEBUG
  printf("clearing dvdnav\n");
#endif
  result=dvdnav_clear(this);
#ifdef LOG_DEBUG
  printf("starting vm\n");
#endif
//  if(!this->started) {
//    /* Start the VM */
//    vm_start(this->vm);
//    this->started = 1;
//  }
#ifdef LOG_DEBUG
  printf("unlocking\n");
#endif
  pthread_mutex_unlock(&this->vm_lock); 
  return result;
}

dvdnav_status_t dvdnav_path(dvdnav_t *this, char** path) {
  if(!this || !path || !(*path)) {
    return S_ERR;
  }

  /* FIXME: Is shallow copy 'right'? */
  (*path) = this->path;

  return S_OK;
}

char* dvdnav_err_to_string(dvdnav_t *this) {
  if(!this) {
    /* Shold this be "passed a NULL pointer?" */
    return "Hey! You gave me a NULL pointer you naughty person!";
  }
  
  return this->err_str;
}

/**
 * Returns 1 if block contains NAV packet, 0 otherwise.
 * Precesses said NAV packet if present.
 *
 * Most of the code in here is copied from xine's MPEG demuxer
 * so any bugs which are found in that should be corrected here also.
 */
int dvdnav_decode_packet(dvdnav_t *this, uint8_t *p, dsi_t* nav_dsi, pci_t* nav_pci) {
  int            bMpeg1=0;
  uint32_t       nHeaderLen;
  uint32_t       nPacketLen;
  uint32_t       nStreamID;
/* uint8_t       *p_start=p; */


  if (p==NULL) {
    fprintf(stderr,"Passed a NULL pointer.\n");
    return 0;
  }

  /* dprint("Checking packet...\n"); */

  if (p[3] == 0xBA) { /* program stream pack header */

    int nStuffingBytes;

    /* xprintf (VERBOSE|DEMUX, "program stream pack header\n"); */

    bMpeg1 = (p[4] & 0x40) == 0;

    if (bMpeg1) {
      p   += 12;
    } else { /* mpeg2 */
      nStuffingBytes = p[0xD] & 0x07;
      p += 14 + nStuffingBytes;
    }
  }


  if (p[3] == 0xbb) { /* program stream system header */
    int nHeaderLen;

    nHeaderLen = (p[4] << 8) | p[5];
    p += 6 + nHeaderLen;
  }

  /* we should now have a PES packet here */

  if (p[0] || p[1] || (p[2] != 1)) {
    fprintf(stderr,"demux error! %02x %02x %02x (should be 0x000001) \n",p[0],p[1],p[2]);
    return 0;
  }

  nPacketLen = p[4] << 8 | p[5];
  nStreamID  = p[3];

  nHeaderLen = 6;
  p += nHeaderLen;

  if (nStreamID == 0xbf) { /* Private stream 2 */
/*
 *   int i;
 *    printf("dvdnav:nav packet=%u\n",p-p_start-6);
 *   for(i=0;i<80;i++) {
 *     printf("%02x ",p[i-6]);
 *   }
 *   printf("\n");
 */
    if(p[0] == 0x00) {
      navRead_PCI(nav_pci, p+1);
    }

    p += nPacketLen;

    /* We should now have a DSI packet. */
    if(p[6] == 0x01) {
      nPacketLen = p[4] << 8 | p[5];
      p += 6;
      /* dprint("NAV DSI packet\n");  */
      navRead_DSI(nav_dsi, p+1);

    } 
    return 1;
  }
  return 0;
}

/* DSI is used for most angle stuff. 
 * PCI is used for only non-seemless angle stuff
 */ 
int dvdnav_get_vobu(dvdnav_t *self, dsi_t* nav_dsi, pci_t* nav_pci, dvdnav_vobu_t* vobu) {
  uint32_t next;
  int angle, num_angle;

  vobu->vobu_start = nav_dsi->dsi_gi.nv_pck_lbn; /* Absolute offset from start of disk */
  vobu->vobu_length = nav_dsi->dsi_gi.vobu_ea; /* Relative offset from vobu_start */
     
  /*
   * If we're not at the end of this cell, we can determine the next
   * VOBU to display using the VOBU_SRI information section of the
   * DSI.  Using this value correctly follows the current angle,
   * avoiding the doubled scenes in The Matrix, and makes our life
   * really happy.
   *
   * vobu_next is an offset value, 0x3fffffff = SRI_END_OF_CELL
   * DVDs are about 6 Gigs, which is only up to 0x300000 blocks
   * Should really assert if bit 31 != 1
   */
  
  /* Relative offset from vobu_start */
  vobu->vobu_next = ( nav_dsi->vobu_sri.next_vobu & 0x3fffffff );  
  
  /* Old code -- may still be sueful one day 
  if(nav_dsi->vobu_sri.next_vobu != SRI_END_OF_CELL ) {
    vobu->vobu_next = ( nav_dsi->vobu_sri.next_vobu & 0x3fffffff );
  } else {
    vobu->vobu_next = vobu->vobu_length;
  } */
  
  dvdnav_get_angle_info(self, &angle, &num_angle);
#if 0
  /* FIMXE: The angle reset doesn't work for some reason for the moment */
  
  if((num_angle < angle) && (angle != 1)) {
    printf("OOOOOOO angle ends!\n");
    
    /* This is to switch back to angle one when we
     * finish with angles. */
    dvdnav_angle_change(self, 1);
  } 
#endif

  if(num_angle != 0) {
    next = nav_pci->nsml_agli.nsml_agl_dsta[angle-1];
    
    if(next != 0) {
      if((next & 0x3fffffff) != 0) {
	if(next & 0x80000000) {
	  vobu->vobu_next = - (int32_t)(next & 0x3fffffff);
	} else {
	  vobu->vobu_next = + (int32_t)(next & 0x3fffffff);
	}
      }
      
    } else if( nav_dsi->sml_agli.data[angle-1].address != 0 ) {
      next = nav_dsi->sml_agli.data[angle-1].address;
      vobu->vobu_length = nav_dsi->sml_pbi.ilvu_ea;
      
      if((next & 0x80000000) && (next != 0x7fffffff)) {
	vobu->vobu_next =  - (int32_t)(next & 0x3fffffff);
      } else {
	vobu->vobu_next =  + (int32_t)(next & 0x3fffffff);
      }
    }
  }

  return 1;
}

/* This is the main get_next_block function which actually gets the media stream video and audio etc.
 * The use of this function is optional, with the application programmer
 * free to implement their own version of this function
 * FIXME: Make the function calls from here public API calls.
 */

dvdnav_status_t dvdnav_get_next_block(dvdnav_t *this, unsigned char *buf,
 				      int *event, int *len) {
  unsigned char *block;
  dvdnav_status_t status;
  
  block = buf;
  status = dvdnav_get_next_cache_block(this, &block, event, len);
  if (block != buf) {
    /* we received a block from the cache, copy it, so we can give it back */
    memcpy(buf, block, DVD_VIDEO_LB_LEN);
    dvdnav_free_cache_block(this, block);
  }
  return status;
}
 
dvdnav_status_t dvdnav_get_next_cache_block(dvdnav_t *this, unsigned char **buf,
 					    int *event, int *len) {
  dvd_state_t *state;
  int result;
  if(!this || !event || !len || !buf || !*buf) {
    printerr("Passed a NULL pointer");
    return S_ERR;
  }
  pthread_mutex_lock(&this->vm_lock); 
  
  if(!this->started) {
    /* Start the VM */
    vm_start(this->vm);
    this->started = 1;
  }

  state = &(this->vm->state);
  (*event) = DVDNAV_NOP;
  (*len) = 0;
 
  /* Check the STOP flag */
  if(this->stop) {
    (*event) = DVDNAV_STOP;
    pthread_mutex_unlock(&this->vm_lock); 
    return S_OK;
  }

  /* Check the STILLFRAME flag */
  /* FIXME: Still cell, not still frame  */
  if(this->position_current.still != 0) {
    dvdnav_still_event_t still_event;

    still_event.length = this->position_current.still;

    (*event) = DVDNAV_STILL_FRAME;
    (*len) = sizeof(dvdnav_still_event_t);
    memcpy(*buf, &(still_event), sizeof(dvdnav_still_event_t));
 
    pthread_mutex_unlock(&this->vm_lock); 
    return S_OK;
  }

  vm_position_get(this->vm,&this->position_next);
  /**********
   fprintf(stderr, "POS-NEXT ");
   vm_position_print(this->vm, &this->position_next);
   fprintf(stderr, "POS-CUR ");
   vm_position_print(this->vm, &this->position_current);
  **********/

  if(this->position_current.hop_channel != this->position_next.hop_channel) {
    this->position_current.hop_channel = this->position_next.hop_channel;
    (*event) = DVDNAV_HOP_CHANNEL;
    (*len) = 0;
    pthread_mutex_unlock(&this->vm_lock); 
    return S_OK;
  }



  if(this->spu_clut_changed) {
    (*event) = DVDNAV_SPU_CLUT_CHANGE;
#ifdef LOG_DEBUG
    fprintf(stderr,"libdvdnav:SPU_CLUT_CHANGE\n");
#endif
    (*len) = sizeof(dvdnav_still_event_t);
    memcpy(*buf, &(state->pgc->palette), 16 * sizeof(uint32_t));
    this->spu_clut_changed = 0;
#ifdef LOG_DEBUG
    fprintf(stderr,"libdvdnav:SPU_CLUT_CHANGE returning S_OK\n");
#endif
    pthread_mutex_unlock(&this->vm_lock); 
    return S_OK;
  }
  
  if(this->position_current.spu_channel != this->position_next.spu_channel) {
    dvdnav_spu_stream_change_event_t stream_change;
    (*event) = DVDNAV_SPU_STREAM_CHANGE;
#ifdef LOG_DEBUG
    fprintf(stderr,"libdvdnav:SPU_STREAM_CHANGE\n");
#endif
    (*len) = sizeof(dvdnav_spu_stream_change_event_t);
    stream_change.physical_wide = vm_get_subp_active_stream(this->vm, 0);
    stream_change.physical_letterbox = vm_get_subp_active_stream(this->vm, 1);
    stream_change.physical_pan_scan = vm_get_subp_active_stream(this->vm, 2);
    memcpy(*buf, &(stream_change), sizeof( dvdnav_spu_stream_change_event_t));
    this->position_current.spu_channel = this->position_next.spu_channel;
#ifdef LOG_DEBUG
    fprintf(stderr,"libdvdnav:SPU_STREAM_CHANGE stream_id_wide=%d\n",stream_change.physical_wide);
    fprintf(stderr,"libdvdnav:SPU_STREAM_CHANGE stream_id_letterbox=%d\n",stream_change.physical_letterbox);
    fprintf(stderr,"libdvdnav:SPU_STREAM_CHANGE stream_id_pan_scan=%d\n",stream_change.physical_pan_scan);
#endif
    pthread_mutex_unlock(&this->vm_lock); 
    if (stream_change.physical_wide != -1 &&
        stream_change.physical_letterbox != -1 &&
        stream_change.physical_pan_scan != -1) {
#ifdef LOG_DEBUG
      fprintf(stderr,"libdvdnav:SPU_STREAM_CHANGE returning S_OK\n");
#endif
      return S_OK;
    }
  }
  
  if(this->position_current.audio_channel != this->position_next.audio_channel) {
    dvdnav_audio_stream_change_event_t stream_change;
    (*event) = DVDNAV_AUDIO_STREAM_CHANGE;
#ifdef LOG_DEBUG
    fprintf(stderr,"libdvdnav:AUDIO_STREAM_CHANGE\n");
#endif
    (*len) = sizeof(dvdnav_audio_stream_change_event_t);
    stream_change.physical= vm_get_audio_active_stream( this->vm );
    memcpy(*buf, &(stream_change), sizeof( dvdnav_audio_stream_change_event_t));
    this->position_current.audio_channel = this->position_next.audio_channel;
#ifdef LOG_DEBUG
    fprintf(stderr,"libdvdnav:AUDIO_STREAM_CHANGE stream_id=%d returning S_OK\n",stream_change.physical);
#endif
    pthread_mutex_unlock(&this->vm_lock); 
    return S_OK;
  }
     
  /* Check the HIGHLIGHT flag */
  /* FIXME: Use BUTTON instead of HIGHLIGHT. */
  if(this->position_current.button != this->position_next.button) {
    dvdnav_highlight_event_t hevent;

    hevent.display = 1;
    hevent.buttonN = this->position_next.button;

    this->position_current.button = this->position_next.button;
    
    (*event) = DVDNAV_HIGHLIGHT;
    (*len) = sizeof(hevent);
    memcpy(*buf, &(hevent), sizeof(hevent));
    pthread_mutex_unlock(&this->vm_lock); 
    return S_OK;
  }

  /* Check to see if we need to change the currently opened VOB */
  if((this->position_current.vts != this->position_next.vts) || 
     (this->position_current.domain != this->position_next.domain)) {
    dvd_read_domain_t domain;
    int vtsN;
    dvdnav_vts_change_event_t vts_event;
    
    if(this->file) {
      dvdnav_read_cache_clear(this->cache);
      DVDCloseFile(this->file);
      this->file = NULL;
    }

    vts_event.old_vtsN = this->open_vtsN;
    vts_event.old_domain = this->open_domain;
     
    /* Use the current DOMAIN to find whether to open menu or title VOBs */
    switch(this->position_next.domain) {
     case FP_DOMAIN:
     case VMGM_DOMAIN:
      domain = DVD_READ_MENU_VOBS;
      vtsN = 0;
      break;
     case VTSM_DOMAIN:
      domain = DVD_READ_MENU_VOBS;
      vtsN = this->position_next.vts; 
      break;
     case VTS_DOMAIN:
      domain = DVD_READ_TITLE_VOBS;
      vtsN = this->position_next.vts; 
      break;
     default:
      printerr("Unknown domain when changing VTS.");
      pthread_mutex_unlock(&this->vm_lock); 
      return S_ERR;
    }
    
    this->position_current.vts = this->position_next.vts; 
    this->position_current.domain = this->position_next.domain;
    dvdnav_read_cache_clear(this->cache);
    this->file = DVDOpenFile(vm_get_dvd_reader(this->vm), vtsN, domain);
    vts_event.new_vtsN = this->position_next.vts; 
    vts_event.new_domain = this->position_next.domain; 

    /* If couldn't open the file for some reason, moan */
    if(this->file == NULL) {
      printerrf("Error opening vtsN=%i, domain=%i.", vtsN, domain);
      pthread_mutex_unlock(&this->vm_lock); 
      return S_ERR;
    }

    /* File opened successfully so return a VTS change event */
    (*event) = DVDNAV_VTS_CHANGE;
    memcpy(*buf, &(vts_event), sizeof(vts_event));
    (*len) = sizeof(vts_event);

    /* On a VTS change, we want to disable any highlights which
     * may have been shown (FIXME: is this valid?) */
    this->spu_clut_changed = 1;
    this->position_current.cell = -1; /* Force an update */
    this->position_current.spu_channel = -1; /* Force an update */
    this->position_current.audio_channel = -1; /* Force an update */;
     
    pthread_mutex_unlock(&this->vm_lock); 
    return S_OK;
  }
  /* FIXME: Don't really need "cell", we only need vobu_start */
  if( (this->position_current.cell != this->position_next.cell) ||
      (this->position_current.cell_restart != this->position_next.cell_restart) ||
      (this->position_current.vobu_start != this->position_next.vobu_start) ||
      (this->position_current.vobu_next != this->position_next.vobu_next) ) {
    this->position_current.cell = this->position_next.cell;
    this->position_current.cell_restart = this->position_next.cell_restart;
    /* vobu_start changes when PGC or PG changes. */
    this->position_current.vobu_start = this->position_next.vobu_start;
    this->position_current.vobu_next = this->position_next.vobu_next;
    /* FIXME: Need to set vobu_start, vobu_next */
    this->vobu.vobu_start = this->position_next.vobu_start; 
    /* vobu_next is use for mid cell resumes */
    this->vobu.vobu_next = this->position_next.vobu_next; 
    this->vobu.vobu_length = 0; 
    this->vobu.blockN = this->vobu.vobu_length + 1; 
    /* Make blockN > vobu_lenght to do expected_nav */
    (*event) = DVDNAV_CELL_CHANGE;
    (*len) = 0;
    pthread_mutex_unlock(&this->vm_lock); 
    return S_OK;
  }
 
 
  if (this->vobu.blockN > this->vobu.vobu_length) {
    /* End of VOBU */

    if(this->vobu.vobu_next == SRI_END_OF_CELL) {
      /* End of Cell from NAV DSI info */
#ifdef LOG_DEBUG
      fprintf(stderr, "Still set to %x\n", this->position_next.still);
#endif
      this->position_current.still = this->position_next.still;

      if( this->position_current.still == 0 || this->skip_still ) {
        vm_get_next_cell(this->vm);
        vm_position_get(this->vm,&this->position_next);
        /* FIXME: Need to set vobu_start, vobu_next */
        this->position_current.still = 0; /* still gets activated at end of cell */
        this->skip_still = 0;
        this->position_current.cell = this->position_next.cell;
        this->position_current.vobu_start = this->position_next.vobu_start;
        this->position_current.vobu_next = this->position_next.vobu_next;
        this->vobu.vobu_start = this->position_next.vobu_start; 
        /* vobu_next is use for mid cell resumes */
        this->vobu.vobu_next = this->position_next.vobu_next; 
        this->vobu.vobu_length = 0; 
        this->vobu.blockN = this->vobu.vobu_length + 1; 
        /* Make blockN > vobu_next to do expected_nav */
        (*event) = DVDNAV_CELL_CHANGE;
        (*len) = 0;
        pthread_mutex_unlock(&this->vm_lock); 
        return S_OK;
      } else {
        dvdnav_still_event_t still_event;
        still_event.length = this->position_current.still;
        (*event) = DVDNAV_STILL_FRAME;
        (*len) = sizeof(dvdnav_still_event_t);
        memcpy(*buf, &(still_event), sizeof(dvdnav_still_event_t));
        pthread_mutex_unlock(&this->vm_lock); 
        return S_OK;
      }

      /* Only set still after whole VOBU has been output. */
      /*
       if(this->position_next.still != 0) {
	this->position_current.still = this->position_next.still;
       }
       */

    }
    /* Perform the jump if necessary (this is always a 
     * VOBU boundary). */

    //result = DVDReadBlocks(this->file, this->vobu.vobu_start + this->vobu.vobu_next, 1, buf);
    result = dvdnav_read_cache_block(this->cache, this->vobu.vobu_start + this->vobu.vobu_next, 1, buf);

    if(result <= 0) {
      printerr("Error reading NAV packet.");
      pthread_mutex_unlock(&this->vm_lock); 
      return S_ERR;
    }
    /* Decode nav into pci and dsi. */
    /* Then get next VOBU info. */
    if(dvdnav_decode_packet(this, *buf, &this->dsi, &this->pci) == 0) {
      printerr("Expected NAV packet but none found.");
      pthread_mutex_unlock(&this->vm_lock); 
      return S_ERR;
    }
    dvdnav_get_vobu(this, &this->dsi,&this->pci, &this->vobu); 
    this->vobu.blockN=1;
    /* FIXME: We need to update the vm state->blockN with which VOBU we are in.
     *        This is so RSM resumes to the VOBU level and not just the CELL level.
     *        This should be implemented with a new Public API call.
     */
    /* We cache one past the end of the VOBU, 
     * in the hope it might catch the next NAV packet as well.
     * This reduces the amount of read commands sent to the DVD device.
     * A cache miss will only happen for 3 reasons.
     * 1) Seeking
     * 2) Menu change
     * 3) The next VOBU does not immeadiately follow the current one. E.g. Multi Angles, ILVU.
     */
    dvdnav_pre_cache_blocks(this->cache, this->vobu.vobu_start+1, this->vobu.vobu_length+1);
    
    /* Successfully got a NAV packet */
    (*event) = DVDNAV_NAV_PACKET;
    (*len) = 2048; 
    pthread_mutex_unlock(&this->vm_lock); 
    return S_OK;
  }
  
  /* If we've got here, it must just be a normal block. */
  if(!this->file) {
    printerr("Attempting to read without opening file");
    pthread_mutex_unlock(&this->vm_lock); 
    return S_ERR;
  }

  result = dvdnav_read_cache_block(this->cache, this->vobu.vobu_start + this->vobu.blockN, 1, buf);
  if(result <= 0) {
    printerr("Error reading from DVD.");
    pthread_mutex_unlock(&this->vm_lock); 
    return S_ERR;
  }
  this->vobu.blockN++;
  (*len) = 2048;
  (*event) = DVDNAV_BLOCK_OK;

  pthread_mutex_unlock(&this->vm_lock); 
  return S_OK;
}

uint16_t dvdnav_audio_stream_to_lang(dvdnav_t *this, uint8_t stream) {
  audio_attr_t  attr;
  
  if(!this)
    return -1;
  
  pthread_mutex_lock(&this->vm_lock); 
  attr = vm_get_audio_attr(this->vm, stream);
  pthread_mutex_unlock(&this->vm_lock); 
  
  if(attr.lang_type != 1)
    return 0xffff;
  
  return attr.lang_code;
}

int8_t dvdnav_get_audio_logical_stream(dvdnav_t *this, uint8_t audio_num) {
  int8_t       retval;
  
  if(!this)
    return -1;
  
  pthread_mutex_lock(&this->vm_lock); 
  retval = NCLK_dvdnav_get_audio_logical_stream(this, audio_num);
  pthread_mutex_unlock(&this->vm_lock); 

  return retval;
}

uint16_t dvdnav_spu_stream_to_lang(dvdnav_t *this, uint8_t stream) {
  subp_attr_t  attr;
  
  if(!this)
    return -1;
  
  pthread_mutex_lock(&this->vm_lock); 
  attr = vm_get_subp_attr(this->vm, stream);
  pthread_mutex_unlock(&this->vm_lock); 
  
  if(attr.type != 1)
    return 0xffff;
  
  return attr.lang_code;
}

int8_t dvdnav_get_spu_logical_stream(dvdnav_t *this, uint8_t subp_num) {
  int8_t       retval;

  if(!this)
    return -1;

  pthread_mutex_lock(&this->vm_lock); 
  retval = NCLK_dvdnav_get_spu_logical_stream(this, subp_num);
  pthread_mutex_unlock(&this->vm_lock); 

  return retval;
}

int8_t dvdnav_get_active_spu_stream(dvdnav_t *this) {
  int8_t        retval;

  if(!this)
    return -1;
  
  pthread_mutex_lock(&this->vm_lock); 
  retval = NLCK_dvdnav_get_active_spu_stream(this);
  pthread_mutex_unlock(&this->vm_lock); 
  
  return retval;
}

/* First Play domain. (Menu) */
int8_t dvdnav_is_domain_fp(dvdnav_t *this) {
  return _dvdnav_is_domain(this, FP_DOMAIN);
}
/* Video management Menu domain. (Menu) */
int8_t dvdnav_is_domain_vmgm(dvdnav_t *this) {
  return _dvdnav_is_domain(this, VMGM_DOMAIN);
}
/* Video Title Menu domain (Menu) */
int8_t dvdnav_is_domain_vtsm(dvdnav_t *this) {
  return _dvdnav_is_domain(this, VTSM_DOMAIN);
}
/* Video Title domain (playing movie). */
int8_t dvdnav_is_domain_vts(dvdnav_t *this) { 
  return _dvdnav_is_domain(this, VTS_DOMAIN);
}

/* Generally delegate angle information handling to 
 * VM */
dvdnav_status_t dvdnav_angle_change(dvdnav_t *this, int angle) {
  int num, current;
  
  if(!this) {
    return S_ERR;
  }

  if(dvdnav_get_angle_info(this, &current, &num) != S_OK) {
    printerr("Error getting angle info");
    return S_ERR;
  }
  
  /* Set angle SPRM if valid */
  if((angle > 0) && (angle <= num)) {
    this->vm->state.AGL_REG = angle;
  } else {
    printerr("Passed an invalid angle number");
    return S_ERR;
  }

  return S_OK;
}
/* FIXME: change order of current_angle, number_of_angles */
dvdnav_status_t dvdnav_get_angle_info(dvdnav_t *this, int* current_angle,
				     int *number_of_angles) {
  if(!this || !this->vm) {
    return S_ERR;
  }

  if(!current_angle || !number_of_angles) {
    printerr("Passed a NULL pointer");
    return S_ERR;
  }

  vm_get_angle_info(this->vm, number_of_angles, current_angle);

  return S_OK;
}

dvdnav_status_t dvdnav_get_cell_info(dvdnav_t *this, int* current_angle,
				     int *number_of_angles) {
  if(!this || !this->vm) {
    return S_ERR;
  }
  *current_angle=this->position_next.cell;
  return S_OK;
} 

pci_t* dvdnav_get_current_nav_pci(dvdnav_t *this) {
  if(!this || !this->vm) return 0;
  return &this->pci;
}

dsi_t* dvdnav_get_current_nav_dsi(dvdnav_t *this) {
  if(!this || !this->vm) return 0;
  return &this->dsi;
}

uint32_t dvdnav_get_next_still_flag(dvdnav_t *this) {
  if(!this || !this->vm) {
    return S_ERR;
  }
  return this->position_next.still;
}

/*
 * $Log$
 * Revision 1.2  2004/09/06 05:53:59  NicJA
 * Initial Port (not quite working) of AMP2.  OVERLAY support isnt yet possible due to lack of cgxvideo.  Possibly a number of other issues (and a function needs coded in aros/arossupport.c)
 *
 * Revision 1.29  2002/07/25 14:51:40  richwareham
 * Moved get_current_nav_pci into dvdnac.c, changed example to use it instead of 'home-rolled'
 * check_packet.
 *
 * Revision 1.28  2002/07/25 14:42:33  richwareham
 * Patch from aschultz@cs.uni-magdeburg.de to allow for still-frame 'peek-ahead'
 *
 * Revision 1.27  2002/07/12 15:46:44  mroi
 * use new memcopy'less read ahead cache
 *
 * Revision 1.26  2002/07/06 16:24:54  mroi
 * * fix debug messages
 * * send spu stream change event only, when there are new streams
 *   (should fix problems with Terminator disk 2)
 *
 * Revision 1.25  2002/07/05 14:18:54  mroi
 * report all spu types (widescreen, letterbox and pan&scan), not widescreen
 * only and report the stream's scale permissions to detect pan&scan material
 *
 * Revision 1.24  2002/07/05 01:42:30  jcdutton
 * Add more debug info for Menu language selection.
 * Only do vm_start when we have to.
 *
 * Revision 1.23  2002/07/02 22:57:09  jcdutton
 * Rename some of the functions in vm.c to help readability.
 * Hopefully fix __FUNCTION__ problem. Use __func_ as recommended in C99.
 * Fix bug where libdvdnav would not immeadiately replay the same cell due to menu buttons.
 *
 * Revision 1.22  2002/06/25 13:37:11  jcdutton
 * Revert back to old read_cache method.
 * Some new optimizations added to the old read_cache method, thus reducing the amount of calls to read blocks from the DVD device.
 *
 * Revision 1.21  2002/06/06 15:03:09  richwareham
 * Biiiiiiig change to let doxygen generate some docs for the library. Note that I'm in no way sure that the autoconf stuff plays nice.
 *
 * Revision 1.20  2002/06/04 13:35:16  richwareham
 * Removed more C++ style comments
 *
 * Revision 1.19  2002/05/30 23:15:14  richwareham
 * First draft of removing HAVE_DVDREAD9
 *
 * Revision 1.18  2002/05/30 09:52:29  richwareham
 * 'Objectified' the read-ahead cache in preparation to implement a 'proper' threaded cache a-la that recommended in the DVD Demystified book.
 *
 * Revision 1.17  2002/05/09 11:57:24  richwareham
 * Angles now work (still a few wrinkles though -- e.g. angle does not reset to '1' when returning to menus)
 *
 * Revision 1.16  2002/04/24 21:15:25  jcdutton
 * Quiet please!!!
 *
 * Revision 1.15  2002/04/24 00:47:46  jcdutton
 * Some more cleanups.
 * Improve button passing.
 *
 * Revision 1.14  2002/04/23 13:26:08  jcdutton
 * Add some comments, FIXMEs.
 * The main point being that dvdnav_get_next_block is almost in a state where it can be optional whether the application programmer uses it, or implements their own version of the function. That is been the main reason for the re-write of this function recently.
 *
 * Revision 1.13  2002/04/23 12:55:40  jcdutton
 * Removed un-needed variables.
 * General Clean up.
 *
 * Revision 1.12  2002/04/23 12:34:39  f1rmb
 * Why rewrite vm function, use it instead (this remark is for me, of course ;-) ).
 * Comment unused var, shut compiler warnings.
 *
 * Revision 1.11  2002/04/23 02:12:27  jcdutton
 * Re-implemented seeking.
 *
 * Revision 1.10  2002/04/23 00:07:16  jcdutton
 * Name stills work better.
 *
 * Revision 1.9  2002/04/22 22:00:48  jcdutton
 * Start of rewrite of libdvdnav. Still need to re-implement seeking.
 *
 * Revision 1.8  2002/04/22 20:57:14  f1rmb
 * Change/fix SPU active stream id. Same for audio. Few new functions, largely
 * inspired from libogle ;-).
 *
 * Revision 1.7  2002/04/10 16:45:57  jcdutton
 * Actually fix the const this time!
 *
 * Revision 1.6  2002/04/07 14:10:11  richwareham
 * Stop C++ bitching about some things and extend the menus example
 *
 * Revision 1.5  2002/04/06 18:42:05  jcdutton
 * Slight correction to handle quicker menu transitions.
 *
 * Revision 1.4  2002/04/06 18:31:50  jcdutton
 * Some cleaning up.
 * changed exit(1) to assert(0) so they actually get seen by the user so that it helps developers more.
 *
 * Revision 1.3  2002/04/02 18:22:27  richwareham
 * Added reset patch from Kees Cook <kees@outflux.net>
 *
 * Revision 1.2  2002/04/01 18:56:28  richwareham
 * Added initial example programs directory and make sure all debug/error output goes to stderr.
 *
 * Revision 1.1.1.1  2002/03/12 19:45:57  richwareham
 * Initial import
 *
 * Revision 1.28  2002/02/02 23:26:20  richwareham
 * Restored title selection
 *
 * Revision 1.27  2002/02/01 15:48:10  richwareham
 * Re-implemented angle selection and title/chapter display
 *
 * Revision 1.26  2002/01/31 16:53:49  richwareham
 * Big patch from Daniel Caujolle-Bert to (re)implement SPU/Audio language display
 *
 * Revision 1.25  2002/01/24 20:53:50  richwareham
 * Added option to _not_ use DVD read-ahead to options
 *
 * Revision 1.24  2002/01/20 15:54:59  jcdutton
 * Implement seeking.
 * It is still a bit buggy, but works sometimes.
 * I need to find out how to make the jump clean.
 * At the moment, some corruption of the mpeg2 stream occurs, 
 * which causes libmpeg2 to crash.
 *
 * Revision 1.23  2002/01/18 00:23:52  jcdutton
 * Support Ejecting of DVD.
 * It will first un-mount the DVD, then eject it.
 *
 * Revision 1.22  2002/01/17 14:50:32  jcdutton
 * Fix corruption of stream during menu transitions.
 * Menu transitions are now clean.
 *
 * Revision 1.21  2002/01/15 00:37:03  jcdutton
 * Just a few cleanups, and a assert fix. (memset fixed it)
 *
 * Revision 1.20  2002/01/13 22:17:57  jcdutton
 * Change logging.
 *
 *
 */
