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

/* Characteristics/setting API calls */

dvdnav_status_t dvdnav_get_region_mask(dvdnav_t *this, int *region) {
  if(!this)
   return S_ERR;

  if(!region) {
    printerr("Passed a NULL pointer");
    return S_ERR;
  }

  if(!this->vm) {
    printerr("VM not yet initialised");
    return S_ERR;
  }

  (*region) = this->vm->state.registers.SPRM[20];
  
  return S_OK;
}

dvdnav_status_t dvdnav_set_region_mask(dvdnav_t *this, int mask) {
  if(!this)
   return S_ERR;

  if(!this->vm) {
    printerr("VM not yet initialised");
    return S_ERR;
  }

  this->vm->state.registers.SPRM[20] = (mask & 0xff);
  
  return S_OK;
}

dvdnav_status_t dvdnav_set_readahead_flag(dvdnav_t *this, int use_readahead) {
  if(!this)
   return S_ERR;

  this->use_read_ahead = use_readahead;

  return S_OK;
}

dvdnav_status_t dvdnav_get_readahead_flag(dvdnav_t *this, int* flag) {
  if(!this)
   return S_ERR;

  if(!flag) {
    printerr("Passed a NULL pointer");
    return S_ERR;
  }

  (*flag) = this->use_read_ahead;
  return S_OK;
}

static dvdnav_status_t set_language_register(dvdnav_t *this, char *code, int reg) {
  if(!this)
    return S_ERR;
    
  if(!code[0] || !code[1]) {
    printerr("Passed illegal language code");
    return S_ERR;
  }
  
  if(!this->vm) {
    printerr("VM not yet initialised");
    return S_ERR;
  }
  
  pthread_mutex_lock(&this->vm_lock);
  this->vm->state.registers.SPRM[reg] = (code[0] << 8) | code[1];
  pthread_mutex_unlock(&this->vm_lock);
  return S_OK;
}

dvdnav_status_t dvdnav_menu_language_select(dvdnav_t *this, char *code) {
  return set_language_register(this, code, 0);
}

dvdnav_status_t dvdnav_audio_language_select(dvdnav_t *this, char *code) {
  return set_language_register(this, code, 16);
}

dvdnav_status_t dvdnav_spu_language_select(dvdnav_t *this, char *code) {
  return set_language_register(this, code, 18);
}
