/* 
 * Copyright (C) 2001 Rich Wareham <richwareham@users.sourceforge.net>
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

#ifndef DVDNAV_EVENTS_H_INCLUDED
#define DVDNAV_EVENTS_H_INCLUDED

#include <dvdread/ifo_types.h>
#include <dvdread/nav_types.h>
#include <dvdread/dvd_reader.h>

/**
 * \file dvdnav_events.h
 * This header defines events and event types 
 */

/*** EVENTS ***/

#define DVDNAV_BLOCK_OK     0 /*!< The next black was returned */
#define DVDNAV_NOP          1 /*!< No action should be taken   */
#define DVDNAV_STILL_FRAME  2 /*!< The preceeding block was the last in a 
				 still frame. */
#define DVDNAV_SPU_STREAM_CHANGE    3 /*!< The SPU stream was changed */
#define DVDNAV_AUDIO_STREAM_CHANGE  4 /*!< The Audio stream was changed */
#define DVDNAV_VTS_CHANGE   5 /*!< We have changed VTS */ 
#define DVDNAV_CELL_CHANGE  6 /*!< We have jumped to a new cell */
#define DVDNAV_NAV_PACKET   7 /*!< The packet just passed was the NAV packet */
#define DVDNAV_STOP         8 /*!< The last block was final, no more are coming */
#define DVDNAV_HIGHLIGHT    9 /*!< Change highlight region */
#define DVDNAV_SPU_CLUT_CHANGE 10 /*!< SPU CLUT */
#define DVDNAV_SEEK_DONE    11 /*!< Seek done, subtitles should be reset */
#define DVDNAV_HOP_CHANNEL  12 /*!< Sent when non-seemless stream change has happed 
                                E.g. Menu button pressed causing change in menu */

/*** EVENT TYPES ***/

/**
 * Structure providing information on DVDNAV_STILL_FRAME events.
 */
typedef struct {
  int length;   /*!<
		  The length (in seconds) the still frame
		  should be displayed for, or 0xff if
		  indefinate. */
} dvdnav_still_event_t;

/**
 * Structure providing information on DVDNAV_SPU_STREAM_CHANGE events.
 */
typedef struct {
  int physical_wide; /*!< The physical (MPEG) stream number for widescreen display. */
  int physical_letterbox; /*!< The physical (MPEG) stream number for letterboxed display. */
  int physical_pan_scan; /*!< The physical (MPEG) stream number for pan&scan display. */
  int logical;  /*!< The logical (DVD) stream number.   */
} dvdnav_spu_stream_change_event_t;

/**
 * Structure providing information on DVDNAV_AUDIO_STREAM_CHANGE events.
 */
typedef struct {
  int physical; /*!< The physical (MPEG) stream number. */
  int logical;  /*!< The logical (DVD) stream number.   */
} dvdnav_audio_stream_change_event_t;

/**
 * Structure providing information on DVDNAV_VTS_CHANGE events.
 */
typedef struct {
  int old_vtsN; /*!< The old VTS number */
  dvd_read_domain_t old_domain; /*!< The old domain */
  int new_vtsN; /*!< The new VTS number */
  dvd_read_domain_t new_domain; /*!< The new domain */
} dvdnav_vts_change_event_t;

/**
 * Structure providing information on DVDNAV_CELL_CHANGE events.
 */
typedef struct {
  cell_playback_t *old_cell; /*!< The old cell (or NULL if this is 
				the first cell) */
  cell_playback_t *new_cell; /*!< The cell_playback_t for the new cell */
} dvdnav_cell_change_event_t;

/**
 * Structure providing information on DVDNAV_NAV_PACKET events.
 */
typedef struct {
  pci_t *pci;
  dsi_t *dsi;
} dvdnav_nav_packet_event_t;

/**
 * Structure providing information on DVDNAV_HIGHLIGHT events.
 */
typedef struct {
  int display; /*!< 0 - hide, 1 - show, entries below only guaranteed useful
		  if this is '1' */
  uint32_t palette;  /*!< The CLUT entries for the highlight palette 
                        (4-bits per entry -> 4 entries) */
  uint16_t sx,sy,ex,ey; /*!< The start/end x,y positions */
  uint32_t pts;         /*!< Highlight PTS to match with SPU */
  uint32_t buttonN;     /*!< Button number for the SPU decoder. */
} dvdnav_highlight_event_t;

#endif /* DVDNAV_EVENTS_H_INCLUDED */
