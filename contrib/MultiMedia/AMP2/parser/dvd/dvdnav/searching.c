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

#include <dvdnav.h>
#include "dvdnav_internal.h"

#include "vm.h"
#include <dvdread/nav_types.h>

/* Searching API calls */

dvdnav_status_t dvdnav_time_search(dvdnav_t *this,
				   unsigned long int time) {
/* Time search the current PGC based on the xxx table */
  return S_OK;
}

/* Scan the ADMAP for a particular block number. */
/* Return placed in vobu. */
/* Returns error status */

dvdnav_status_t dvdnav_scan_admap(dvdnav_t *this, int32_t domain, int32_t seekto_block, int32_t *vobu) {
  /* FIXME:Need to handle seeking outside current cell. */
  vobu_admap_t *admap = NULL;
  *vobu = -1;
  fprintf(stderr,"Seeking to target %u ...\n",
              seekto_block);

  /* Search through the VOBU_ADMAP for the nearest VOBU
   * to the target block */
  switch(domain) {
    case FP_DOMAIN:
    case VMGM_DOMAIN:
      admap = this->vm->vmgi->menu_vobu_admap;
      break;
    case VTSM_DOMAIN:
      admap = this->vm->vtsi->menu_vobu_admap;
      break;
    case VTS_DOMAIN:
      admap = this->vm->vtsi->vts_vobu_admap;
      break;
    default:
      fprintf(stderr,"Error: Unknown domain for seeking seek.\n");
  }
  if(admap) {
    int32_t address = 0;
    int32_t vobu_start, next_vobu;
    int found = 0;

    /* Search through ADMAP for best sector */
    vobu_start = 0x3fffffff;
    /* FIXME: Implement a faster search algorithm */
    while((!found) && ((address<<2) < admap->last_byte)) {
      next_vobu = admap->vobu_start_sectors[address];

      /* printf("Found block %u\n", next_vobu); */

      if(vobu_start <= seekto_block &&
          next_vobu > seekto_block) {
        found = 1;
      } else {
        vobu_start = next_vobu;
      }

      address ++;
    }
    if(found) {
      *vobu = vobu_start;
      return S_OK;
    } else {
      fprintf(stderr,"Could not locate block\n");
      return S_ERR;
    }
  }
  fprintf(stderr,"admap not located\n");
  return S_ERR;
}

dvdnav_status_t dvdnav_sector_search(dvdnav_t *this,
				   unsigned long int offset, int origin) {
/* FIXME: Implement */

  uint32_t target = 0;
  uint32_t length = 0;
  uint32_t first_cell_nr, last_cell_nr, cell_nr, fnd_cell_nr;
  int found;
  cell_playback_t *cell, *fnd_cell;
  dvd_state_t *state;
  dvdnav_status_t result;

  if((!this) || (!this->vm) )
    return -1;
  
  state = &(this->vm->state);
  if((!state) || (!state->pgc) )
    return -1;
   
  if(offset == 0) 
    return -1;

  if(this->position_current.still != 0)
    /* Cannot do seeking in a still frame. */
    return -1;

  pthread_mutex_lock(&this->vm_lock);
  result = dvdnav_get_position(this, &target, &length);
  fprintf(stderr,"FIXME: seeking to offset=%lu pos=%u length=%u\n", offset, target, length); 
  fprintf(stderr,"FIXME: Before cellN=%u blockN=%u\n" ,
      state->cellN,
      state->blockN);
  if(!result) {
    pthread_mutex_unlock(&this->vm_lock);
    return -1;
  }
 
  switch(origin) {
   case SEEK_SET:
    if(offset > length) {
      pthread_mutex_unlock(&this->vm_lock);
      return -1;
    }
    target = offset;
    break;
   case SEEK_CUR:
    if(target + offset > length) {
      pthread_mutex_unlock(&this->vm_lock);
      return -1;
    }
    target += offset;
    break;
   case SEEK_END:
    if(length - offset < 0) {
      pthread_mutex_unlock(&this->vm_lock);
      return -1;
    }
    target = length - offset;
   default:
    /* Error occured */
    pthread_mutex_unlock(&this->vm_lock);
    return -1;
  }

  /* First find closest cell number in program */
  first_cell_nr = state->pgc->program_map[state->pgN-1];
  if(state->pgN < state->pgc->nr_of_programs) {
    last_cell_nr = state->pgc->program_map[state->pgN] - 1;
  } else {
    last_cell_nr = state->pgc->nr_of_cells;
  }
    
  found = 0; target += state->pgc->cell_playback[first_cell_nr-1].first_sector;
  fnd_cell_nr = last_cell_nr + 1;
  for(cell_nr = first_cell_nr; (cell_nr <= last_cell_nr) && !found; cell_nr ++) {
    cell =  &(state->pgc->cell_playback[cell_nr-1]);
    if((cell->first_sector <= target) && (cell->last_sector >= target)) {
      state->cellN = cell_nr;
      state->blockN = 0;
      found = 1;
      fnd_cell_nr = cell_nr;
      fnd_cell = cell;
    }
  }

  if(fnd_cell_nr <= last_cell_nr) {
    int32_t vobu, start;
    dvdnav_status_t status;
    fprintf(stderr,"Seeking to cell %i from choice of %i to %i\n",
	   fnd_cell_nr, first_cell_nr, last_cell_nr);
    status = dvdnav_scan_admap(this, state->domain, target, &vobu);
    /* 
     * Clut does not actually change,
     * but as the decoders have been closed then opened,
     * A new clut has to be sent.
     */
    start =(state->pgc->cell_playback[state->cellN - 1].first_sector); 
    fprintf(stderr,"FIXME: After cellN=%u blockN=%u target=%x vobu=%x start=%x\n" ,
      state->cellN,
      state->blockN,
      target,
      vobu,
      start);
    state->blockN = vobu - start; 
    fprintf(stderr,"FIXME: After vobu=%x start=%x blockN=%x\n" ,
      vobu,
      start,
      state->blockN);
    pthread_mutex_unlock(&this->vm_lock);
    return target;
  } else {
    fprintf(stderr, "Error when seeking, asked to seek outside program\n");
  }

  fprintf(stderr,"FIXME: Implement seeking to location %u\n", target); 

  pthread_mutex_unlock(&this->vm_lock);
  return -1;
}

dvdnav_status_t dvdnav_part_search(dvdnav_t *this, int part) {

  if((!this) || (!this->vm) )
    return S_ERR;

  return S_OK;
}

dvdnav_status_t dvdnav_prev_pg_search(dvdnav_t *this) {
  dvd_state_t *state;

  if((!this) || (!this->vm) )
    return S_ERR;

  state = &(this->vm->state);
  if((!state) || (!state->pgc) )
    return S_ERR;

  /* Make sure this is not the first chapter */
  if(state->pgN <= 1 ) {
    fprintf(stderr,"dvdnav: at first chapter. prev chapter failed.\n");
    return S_ERR;
  }
  fprintf(stderr,"dvdnav: previous chapter\n");
  vm_jump_prog(this->vm, state->pgN - 1);
  this->vm->hop_channel++;
  fprintf(stderr,"dvdnav: previous chapter done\n");

  return S_OK;
}

dvdnav_status_t dvdnav_top_pg_search(dvdnav_t *this) {

  if((!this) || (!this->vm) )
    return S_ERR;
    
  fprintf(stderr,"dvdnav: top chapter. NOP.\n");
  
  return S_OK;
}

dvdnav_status_t dvdnav_next_pg_search(dvdnav_t *this) {
  dvd_state_t *state;

  if((!this) || (!this->vm) )
    return S_ERR;

  state = &(this->vm->state);
  if((!state) || (!state->pgc) )
    return S_ERR;

  /* Make sure this is not the last chapter */
  if(state->pgN >= state->pgc->nr_of_programs) {
    fprintf(stderr,"dvdnav: at last chapter. next chapter failed.\n");
    return S_ERR;
  }
  fprintf(stderr,"dvdnav: next chapter\n");
  vm_jump_prog(this->vm, state->pgN + 1);
  this->vm->hop_channel++;
  fprintf(stderr,"dvdnav: next chapter done\n");

  return S_OK;
}

dvdnav_status_t dvdnav_menu_call(dvdnav_t *this, DVDMenuID_t menu) {
  dvd_state_t *state;

  if((!this) || (!this->vm) )
    return S_ERR;

  pthread_mutex_lock(&this->vm_lock); 
  state = &(this->vm->state);
  if (vm_menu_call(this->vm, menu, 0))
    this->vm->hop_channel++;
  pthread_mutex_unlock(&this->vm_lock); 
  return S_OK;
}

dvdnav_status_t dvdnav_current_title_info(dvdnav_t *this, int *tt, int *pr) {
int vts_ttn = 0;
  int vts, i;
  domain_t domain;
  tt_srpt_t* srpt;
  
  if(!this)
   return S_ERR;

  if(!tt || !pr) {
    printerr("Passed a NULL pointer");
  }

  if(tt)
   *tt = -1;
  if(*pr)
   *pr = -1;

  domain = this->vm->state.domain;
  if((domain == FP_DOMAIN) || (domain == VMGM_DOMAIN)) {
    /* Not in a title */
    return S_OK;
  }
  
  vts_ttn = this->vm->state.VTS_TTN_REG;
  vts = this->vm->state.vtsN;

  if(pr) {
    *pr = this->vm->state.pgN;
  }

  /* Search TT_SRPT for title */
  if(!(vm_get_vmgi(this->vm))) {
    printerr("Oh poo, no SRPT");
    return S_ERR;
  }
  
  srpt = vm_get_vmgi(this->vm)->tt_srpt;
  for(i=0; i<srpt->nr_of_srpts; i++) {
    title_info_t* info = &(srpt->title[i]);
    if((info->title_set_nr == vts) && (info->vts_ttn == vts_ttn)) {
      if(tt)
       *tt = i+1;
    }
  }

  return S_OK;
}

static char __title_str[] = "DVDNAV";

dvdnav_status_t dvdnav_get_title_string(dvdnav_t *this, char **title_str) {
  if(!this)
   return S_ERR;

  if(!title_str) {
    printerr("Passed a NULL pointer");
    return S_ERR;
  }

  (*title_str) = __title_str;

  return S_OK;
}

dvdnav_status_t dvdnav_get_position(dvdnav_t *this, unsigned int* pos,
				    unsigned int *len) {
  uint32_t cur_sector;
  uint32_t first_cell_nr;
  uint32_t last_cell_nr;
  cell_playback_t *first_cell;
  cell_playback_t *last_cell;
  dvd_state_t *state;
  if((!this) || (!this->vm) )
   return 0;
  
  state = &(this->vm->state);
  if((!state) || (!state->pgc) )
   return 0;
   
  /* Sanity check */
  if(state->pgN > state->pgc->nr_of_programs) {
    return 0;
  }
  
  /* Get current sector */
  cur_sector = this->vobu.vobu_start + this->vobu.blockN;

  /* Find start cell of program. */
  first_cell_nr = state->pgc->program_map[state->pgN-1];
  first_cell = &(state->pgc->cell_playback[first_cell_nr-1]);
  if(state->pgN < state->pgc->nr_of_programs) {
    last_cell_nr = state->pgc->program_map[state->pgN] - 1;
  } else {
    last_cell_nr = state->pgc->nr_of_cells;
  }
  last_cell = &(state->pgc->cell_playback[last_cell_nr-1]);

  *pos= cur_sector - first_cell->first_sector;
  *len= last_cell->last_sector - first_cell->first_sector;
  /* printf("dvdnav:searching:current pos=%u length=%u\n",*pos,*len); */


  return S_OK;
}

dvdnav_status_t dvdnav_get_position_in_title(dvdnav_t *this,
					     unsigned int *pos,
					     unsigned int *len) {
  uint32_t cur_sector;
  uint32_t first_cell_nr;
  uint32_t last_cell_nr;
  cell_playback_t *first_cell;
  cell_playback_t *last_cell;
  dvd_state_t *state;
  if((!this) || (!this->vm) )
   return S_ERR;
  
  state = &(this->vm->state);
  if((!state) || (!state->pgc) )
   return S_ERR;
   
  /* Sanity check */
  if(state->pgN > state->pgc->nr_of_programs) {
    return S_ERR;
  }
  
  /* Get current sector */
  cur_sector = this->vobu.vobu_start + this->vobu.blockN;

  /* Now find first and last cells in title. */
  first_cell_nr = state->pgc->program_map[0];
  first_cell = &(state->pgc->cell_playback[first_cell_nr-1]);
  last_cell_nr = state->pgc->nr_of_cells;
  last_cell = &(state->pgc->cell_playback[last_cell_nr-1]);
  
  *pos = cur_sector - first_cell->first_sector;
  *len = last_cell->last_sector - first_cell->first_sector;
  
  return S_OK;
}


