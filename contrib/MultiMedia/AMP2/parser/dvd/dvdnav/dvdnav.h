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

/**
 * \file dvdnav.h
 * The main file you should include if you want to access dvdnav
 * functionality.
 */

#ifndef DVDNAV_H_INCLUDED
#define DVDNAV_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* Defines the various events and dvdnav_event_t */
#include "dvdnav_events.h"

/* Various useful types */
#include "dvd_types.h"

#include <dvdread/dvd_reader.h>

/**
 * Opaque data-type can be viewed as a 'DVD handle'. You should get
 * a pointer to a dvdnav_t from the dvdnav_open() function.
 * \sa dvdnav_open()
 */
typedef struct dvdnav_s dvdnav_t;

/* Status */
typedef int dvdnav_status_t;

#define DVDNAV_STATUS_ERR 0
#define DVDNAV_STATUS_OK  1

/**
 * NOTE: */

/** 
 * \defgroup init Initialisation & housekeeping functions 
 * These functions allow you to open a DVD device and associate it
 * with a dvdnav_t.
 * 
 *  Unless otherwise stated, all functions return DVDNAV_STATUS_OK if
 * they succeeded, otherwise DVDNAV_STATUS_ERR is returned and the error may
 * be obtained by calling dvdnav_err_to_string().
 *
 * A minimal <tt>libdvdnav</tt> program should always call dvdnav_open()
 * and dvdnav_close().
 *
 * \par Example:
 * \include minimal.c
 * 
 * @{
 */

/**
 * Attempts to open the DVD drive at the specifiec path and pre-cache
 * any CSS-keys that your hacked libdvdread may use.
 *
 * \param dest Pointer to a dvdnav_t pointer to fill in.
 * \param path Any libdvdread acceptable path
 */
dvdnav_status_t dvdnav_open(dvdnav_t** dest, char *path); 

/**
 * Closes a dvdnav_t previously opened with dvdnav_open(), freeing any 
 * memory associated with it.
 *
 * \param self dvdnav_t to close.
 */
dvdnav_status_t dvdnav_close(dvdnav_t *self);

/**
 * Resets the virtual machine and buffers in a previously opened dvdnav
 *
 * \param self dvdnav_t to reset.
 */
dvdnav_status_t dvdnav_reset(dvdnav_t *self);

/**
 * Fills a pointer with a value pointing to a string describing
 * the path associated with an open dvdnav_t. It assigns it NULL
 * on error.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param path Pointer to char* to fill in.
 */
dvdnav_status_t dvdnav_path(dvdnav_t *self, char** path);

/**
 * Returns a human-readable string describing the last error.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \returns A pointer to said string.
 */
char* dvdnav_err_to_string(dvdnav_t *self);

/** \@} */

/** 
 * \defgroup char Changing and Reading DVD Player characteristics
 * 
 * These functions allow you to manipulate the various global characteristics
 * of the DVD playback engine.
 * 
 * @{
 */

/**
 * Returns the region mask (bit 0 set implies region 1, bit 1 set implies
 * region 2, etc) of the virtual machine. Generally you will only need to set
 * this if you are playing RCE discs which query the virtual machine as to its
 * region setting. 
 *
 * \par Note:
 * This has <b>nothing</b> to do with the region setting of the DVD drive.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param region Pointer to an int which will receive the region code mask.
 */
dvdnav_status_t dvdnav_get_region_mask(dvdnav_t *self, int *region);

/**
 * Sets the region mask of the virtual machine.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param mask 0x00..0xff -- the desired region mask.
 *
 * \sa dvdnav_get_region_mask()
 */
dvdnav_status_t dvdnav_set_region_mask(dvdnav_t *self, int mask);

/**
 * Specify whether read-ahead caching should be used. You may not want this if your
 * decoding engine does its own buffering or if you don't like the fact that this is
 * implemented in a multithreaded manner.
 * 
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param use_readahead 0 - no, 1 - yes
 */
dvdnav_status_t dvdnav_set_readahead_flag(dvdnav_t *self, int use_readahead);

/**
 * Query whether readahead caching/buffering will be used.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param flag Pointer to int to recieve flag value.
 *
 * \sa dvdnav_get_readahead_flag()
 */
dvdnav_status_t dvdnav_get_readahead_flag(dvdnav_t *self, int* flag);

/**
 * @}
 */

/**
 * \defgroup data Reading Data
 *
 * These functions are used to poll the playback enginge and actually get data 
 * off the DVD.
 *
 * @{
 */

/**
 * Attempts to get the next block off the DVD and copies it into the buffer 'buf'. 
 * If there is any special actions that may need to be performed, the value
 * pointed to by 'event' gets set 
 * accordingly.
 *
 * If 'event' is DVDNAV_BLOCK_OK then 'buf' is filled with the next block
 * (note that means it has to be at /least/ 2048 bytes big). 'len' is
 * then set to 2048.
 *
 * Otherwise, buf is filled with an appropriate event structure and
 * len is set to the length of that structure.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param buf Buffer (at least 2048 octets) to fill with next block/event structure.
 * \param event Pointer to int to get event type.
 * \param len Pointer to int to get the number of octets written into buf.
 */
dvdnav_status_t dvdnav_get_next_block(dvdnav_t *self, unsigned char *buf,
				      int *event, int *len);

/**
 * This basically does the same as dvdnav_get_next_block. The only difference is
 * that it avoids a memcopy, when the requested block was found in the cache.
 * I such a case (cache hit) this function will return a different pointer than
 * the one handed in, pointing directly into the relevant block in the cache.
 * Those pointer must _never_ be freed but instead returned to the library via
 + dvdnav_free_cache_block.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param buf Buffer (at least 2048 octets) to fill with next block/event structure.
 *            A different buffer might be returned, if the block was found in the internal cache.
 * \param event Pointer to int to get event type.
 * \param len Pointer to int to get the number of octets written into buf.
 */
dvdnav_status_t dvdnav_get_next_cache_block(dvdnav_t *self, unsigned char **buf,
					    int *event, int *len);

/**
 * All buffers which came from the internal cache (when dvdnav_get_next_cache_block
 * returned a buffer different from the one handed in) have to be freed with this
 * function. Although handing in other buffers not from the cache doesn't cause any harm.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param buf Buffer received from internal cache.
 */
dvdnav_status_t dvdnav_free_cache_block(dvdnav_t *self, unsigned char *buf);

/**
 * Get video aspect code.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * 
 * \returns Video aspect ratio code, 0 -- 4:3, 2 -- 16:9
 */
uint8_t dvdnav_get_video_aspect(dvdnav_t *self);

/**
 * Get video scaling permissions.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * 
 * \returns Video scaling permissions, bit0 - deny letterboxing, bit1 - deny pan&scan
 */
uint8_t dvdnav_get_video_scale_permission(dvdnav_t *self);

/**
 * @}
 */

/**
 * \defgroup nav Navigation Commands
 *
 * @{
 */

/**
 * Returns the number of titles on the disk.
 * 
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param titles Pointer to int to receive number of titles.
 */
dvdnav_status_t dvdnav_get_number_of_titles(dvdnav_t *self, int *titles);

/**
 * Returns the number of programs within the current title.
 * 
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param programs Pointer to int to receive number of programs.
 */
dvdnav_status_t dvdnav_get_number_of_programs(dvdnav_t *self, int *programs);

/**
 * If we are currently in a still-frame this function skips it (or attempts to).
 * This might fail if this still-frame is of infinite duration as most DVD
 * authors wouldn't expect you to be able to do this <tt>:)</tt>
 * 
 * \param self Pointer to dvdnav_t associated with this operation.
 */
dvdnav_status_t dvdnav_still_skip(dvdnav_t *self);

/**
 * Plays a specified title of the DVD.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param title 1..99 -- Title number to play.
 */
dvdnav_status_t dvdnav_title_play(dvdnav_t *self, int title);

/**
 * Plays the specifiec title, starting from the specified
 * part (chapter).
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param title 1..99 -- Title number to play.
 * \param part 1..999 -- Part to start from.
 */
dvdnav_status_t dvdnav_part_play(dvdnav_t *self, int title, int part);

/**
 * Play the specified amount of parts of the specified title of
 * the DVD then STOP.
 *
 * \par Note:
 * Currently unimplemented!
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param title 1..99 -- Title number to play.
 * \param part 1..999 -- Part to start from.
 * \param parts_to_play 1..999 -- Number of parts to play.
 */
dvdnav_status_t dvdnav_part_play_auto_stop(dvdnav_t *self, int title,
					  int part, int parts_to_play);

/**
 * Play the specified title starting from the specified time
 *
 * \par Note:
 * Currently unimplemented!
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param title 1..99 -- Title number to play.
 * \param time Timecode to start from (hours, minutes, seconds + frames).
 */
dvdnav_status_t dvdnav_time_play(dvdnav_t *self, int title,
				unsigned long int time);

/**
 * Stops playing the current title (causes a STOP action in
 * dvdnav_get_next_block()).
 * 
 * \param self Pointer to dvdnav_t associated with this operation.
 *
 * \sa dvdnav_get_next_block()
 */
dvdnav_status_t dvdnav_stop(dvdnav_t *self);

/**
 * Stop playing current title and play the "GoUp"-program chain 
 * (which generally leads to the title menu or a higer-level menu).
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 */
dvdnav_status_t dvdnav_go_up(dvdnav_t *self);

/**
 * @}
 */

/** 
 * \defgroup search Searching
 *
 * @{
 */

/**
 * Stop playing the current title and start playback of the title
 * from the specified timecode.
 *
 * \par Note:
 * Currently unimplemented!
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param time Timecode to start from.
 */
dvdnav_status_t dvdnav_time_search(dvdnav_t *self, 
				  unsigned long int time);

/**
 * Stop playing the current title and start playback of the title
 * from the specified sector offset.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param offset Sector offset to start from.
 * \param origin Start from here, start or end.
 */
dvdnav_status_t dvdnav_sector_search(dvdnav_t *self, 
				  unsigned long int offset, int origin);

/**
 * Stop playing the current title and start playback of the title
 * from the specified part (chapter).
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param part 1..999 -- Part to start from.
 */
dvdnav_status_t dvdnav_part_search(dvdnav_t *self, int part);

/**
 * Stop playing the current title and start playback of the title
 * from the previous program (if it exists).
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 */
dvdnav_status_t dvdnav_prev_pg_search(dvdnav_t *self);

/**
 * Stop playing the current title and start playback of the title
 * from the first program.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 */
dvdnav_status_t dvdnav_top_pg_search(dvdnav_t *self);

/**
 * Stop playing the current title and start playback of the title
 * from the next program (if it exists).
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 */
dvdnav_status_t dvdnav_next_pg_search(dvdnav_t *self);

/**
 * Stop playing the current title and jump to the specified menu.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param menu Which menu to call (see DVDMenuID_t).
 *
 * \sa DVDMenuID_t (from <tt>libdvdread</tt>)
 */
dvdnav_status_t dvdnav_menu_call(dvdnav_t *self, DVDMenuID_t menu);

/**
 * Return the title number and chapter currently being played or
 * -1 if in a menu.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param title Pointer to into which will receive the current title number.
 * \param part Pointer to into which will receive the current part number.
 */
dvdnav_status_t dvdnav_current_title_info(dvdnav_t *self, int *title,
			     		  int *part);

/**
 * Return a string describing the title. This is an ID string encoded on the
 * disc byt the author. In many cases this is a descriptive string such as
 * `<tt>THE_MATRIX</tt>' but sometimes is sigularly uninformative such as
 * `<tt>PDVD-011421</tt>'.
 * 
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param title_str Pointer to C-style string to receive a string describing the title. 
 */
dvdnav_status_t dvdnav_get_title_string(dvdnav_t *self, char **title_str);

/**
 * Return the current position (in blocks) within the current
 * part and the length (in blocks) of said part.
 * 
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param pos Pointer to unsigned int to get the current position.
 * \param len Pointer to unsinged int to hold the length of the current part.
 */
dvdnav_status_t dvdnav_get_position(dvdnav_t *self, unsigned int* pos,
				    unsigned int *len);

/**
 * Return the current position (in blocks) within the current
 * title and the length (in blocks) of said title.
 * 
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param pos Pointer to unsigned int to get the current position.
 * \param len Pointer to unsinged int to hold the length of the current title.
 */
dvdnav_status_t dvdnav_get_position_in_title(dvdnav_t *self,
					     unsigned int* pos,
					     unsigned int *len);

/**
 * @}
 */

/**
 * \defgroup highlight Highlights
 *
 * @{
 */

/**
 * Get the currently highlighted button 
 * number (1..36) or 0 if no button is highlighed.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param button Pointer to the value to fill in.
 */
dvdnav_status_t dvdnav_get_current_highlight(dvdnav_t *self, int* button);

/**
 * Returns the Presentation Control Information (PCI) structure associated
 * with the current position.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 *
 * \sa pci_t (in <tt>libdvdread</tt>)
 */
pci_t* dvdnav_get_current_nav_pci(dvdnav_t *self);

/**
 * Returns the DSI (data seach information) structure associated
 * with the current position.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 *
 * \sa pci_t (in <tt>libdvdread</tt>)
 */
dsi_t* dvdnav_get_current_nav_dsi(dvdnav_t *self);

/**
 * Get the area associated with a certain button.
 *
 * \param nav_pci Pointer to the PCI structure you may have got via
 * dvdnav_get_current_nav_pci().
 *
 * \param button Button number to query.
 * \param mode 0..3 -- Button mode to query.
 * \param highlight Pointer to dvdnav_highlight_area_t to fill in.
 *
 * \sa dvdnav_highlight_area_t
 */
dvdnav_status_t dvdnav_get_highlight_area(pci_t* nav_pci , int32_t button, int32_t mode,
                                           dvdnav_highlight_area_t* highlight);

/**
 * Move button highlight around as suggested by function name (e.g. with arrow keys).
 * 
 * \param self Pointer to dvdnav_t associated with this operation.
 */
dvdnav_status_t dvdnav_upper_button_select(dvdnav_t *self);
/**
 * Move button highlight around as suggested by function name (e.g. with arrow keys).
 * 
 * \param self Pointer to dvdnav_t associated with this operation.
 */
dvdnav_status_t dvdnav_lower_button_select(dvdnav_t *self);
/**
 * Move button highlight around as suggested by function name (e.g. with arrow keys).
 * 
 * \param self Pointer to dvdnav_t associated with this operation.
 */
dvdnav_status_t dvdnav_right_button_select(dvdnav_t *self);
/**
 * Move button highlight around as suggested by function name (e.g. with arrow keys).
 * 
 * \param self Pointer to dvdnav_t associated with this operation.
 */
dvdnav_status_t dvdnav_left_button_select(dvdnav_t *self);

/**
 * Activate (press) the currently highlighted button.
 * \param self Pointer to dvdnav_t associated with this operation.
 */
dvdnav_status_t dvdnav_button_activate(dvdnav_t *self);

/**
 * Highlight a specific button.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param button 1..39 -- Button number to activate.
 */
dvdnav_status_t dvdnav_button_select(dvdnav_t *self, int button);

/**
 * Activate (press) specified button.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param button 1..39 -- Button number to activate.
 */
dvdnav_status_t dvdnav_button_select_and_activate(dvdnav_t *self, int button);

/**
 * Select button at specified (image) co-ordinates.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param x X co-ordinate in image.
 * \param y Y xo-ordinate in image.
 */
dvdnav_status_t dvdnav_mouse_select(dvdnav_t *self, int x, int y);

/**
 * Activate (press) button at specified co-ordinates.
 * 
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param x X co-ordinate in image.
 * \param y Y xo-ordinate in image.
 */
dvdnav_status_t dvdnav_mouse_activate(dvdnav_t *self, int x, int y);

/**
 * @}
 */

/** 
 * \defgroup languages Languages
 *
 * @{
 */

/**
 * Set which menu language we should use.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param code 2 char ISO639 Language code in a C-style string.
 */
dvdnav_status_t dvdnav_menu_language_select(dvdnav_t *self,
					   char *code);

/**
 * Set which audio language we should use.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param code 2 char ISO639 Language code in a C-style string.
 */
dvdnav_status_t dvdnav_audio_language_select(dvdnav_t *self,
					    char *code);

/**
 * Set which spu language we should use.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param code 2 char ISO639 Language code in a C-style string.
 */
dvdnav_status_t dvdnav_spu_language_select(dvdnav_t *self,
					  char *code);

/**
 * @}
 */

/** 
 * \defgroup streams Sub-Picture Unit (Subtitles) and Audio Streams 
 *
 * All these commands manipulate the audio/subtitle stream numbers that the
 * player engine thinks is playing. Note that all the streams are still returned
 * multiplexed by dvdnav_get_next_block(). You should try to make sure that the
 * MPEG demuxer and the player engine both have the same thoughts on what stream the
 * user is currently listening to.
 *
 * \sa dvdnav_get_next_block()
 * 
 * @{
 */

/**
 * Set a specific PHYSICAL MPEG stream.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param audio 0..7 -- Stream number.
 */
dvdnav_status_t dvdnav_physical_audio_stream_change(dvdnav_t *self,
						   int audio);

/**
 * Set a specific logical audio stream.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param audio 0..7 -- Stream number.
 */
dvdnav_status_t dvdnav_logical_audio_stream_change(dvdnav_t *self,
		       				  int audio);

/**
 * Set the int pointed to to the current PHYSICAL audio
 * stream.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param audio Pointer to int which will receive value.
 */
dvdnav_status_t dvdnav_get_physical_audio_stream(dvdnav_t *self, int* audio);

/**
 * Set the int pointed to to the current LOGICAL audio
 * stream.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param audio Pointer to int which will receive value.
 */
dvdnav_status_t dvdnav_get_logical_audio_stream(dvdnav_t *self, int* audio);

/**
 * Set a specific PHYSICAL MPEG SPU stream and whether it should be
 * displayed.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param stream 0..31 or 63 (dummy) -- Stram number.
 * \param display: 0..1 -- Is this actually being displayed?
 */
dvdnav_status_t dvdnav_physical_spu_stream_change(dvdnav_t *self,
					  	  int stream, int display);

/**
 * Set a specific LOGICAL SPU stream and whether it should be
 * displayed.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param stream 0..31 or 63 (dummy) -- Stram number.
 * \param display: 0..1 -- Is this actually being displayed?
 */
dvdnav_status_t dvdnav_logical_spu_stream_change(dvdnav_t *self,
						 int stream, int display);

/**
 * Set the ints pointed to to the current PHYSICAL SPU
 * stream & display flag.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param stream Pointer which will receive value.
 * \param display Pointer which will receive value.
 */
dvdnav_status_t dvdnav_get_physical_spu_stream(dvdnav_t *self,
					       int* stream, int* display);

/**
 * Set the ints pointed to to the current LOGICAL SPU
 * stream & display flag.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param stream Pointer which will receive value.
 * \param display Pointer which will receive value.
 */
dvdnav_status_t dvdnav_get_logical_spu_stream(dvdnav_t *self,
		       			     int* stream, int* disply);

/**
 * Converts a *logical* audio stream id into country code 
 * (returns <tt>0xffff</tt> if no such stream).
 * 
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param stream Stream number
 */
uint16_t dvdnav_audio_stream_to_lang(dvdnav_t *self, uint8_t stream);

/**
 * Converts a *logical* subpicture stream id into country code 
 * (returns <tt>0xffff</tt> if no such stream).
 * 
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param stream Stream number
 */
uint16_t dvdnav_spu_stream_to_lang(dvdnav_t *self, uint8_t stream);

/**
 * Converts a *physical* audio stream id into a logical stream number 
 * 
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param audio_num Stream number
 */
int8_t dvdnav_get_audio_logical_stream(dvdnav_t *self, uint8_t audio_num);

/**
 * Converts a *physical* subpicture stream id into a logical stream number 
 * 
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param subp_num Stream number
 */
int8_t dvdnav_get_spu_logical_stream(dvdnav_t *self, uint8_t subp_num);

/**
 * Get active spu stream.
 * 
 * \param self Pointer to dvdnav_t associated with this operation.
 */
int8_t dvdnav_get_active_spu_stream(dvdnav_t *self);

/**
 * @}
 */

/** 
 * \defgroup angles Multiple angles
 *
 * The <tt>libdvdnav</tt> library abstracts away the difference between seamless and
 * non-seamless angles. From the point of view of the programmer you just set the
 * angle number and all is well in the world. 
 *
 * \par Note:
 * It is quite possible that some tremendously strange DVD feature might change the
 * angle number from under you. Generally you should always view the results from
 * dvdnav_get_angle_info() as definitive only up to the next time you call
 * dvdnav_get_next_block().
 *
 * @{
 */

/**
 * Sets the current angle. If you try to follow a non existant angle
 * the call fails.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param angle 1..9 -- Currentl angle to follow.
 */
dvdnav_status_t dvdnav_angle_change(dvdnav_t *self, int angle);

/**
 * Returns the current angle and number of angles present.
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 * \param current_angle Pointer to int which will get the current angle.
 * \param number_of_angles Pointer to int which will get the number of angles.
 */
dvdnav_status_t dvdnav_get_angle_info(dvdnav_t *self, int* current_angle,
				     int *number_of_angles);

/**
 * FIXME: WTF does this do? -- High qulaity documentation huh?
 */
dvdnav_status_t dvdnav_get_cell_info(dvdnav_t *self, int* current_angle,
				     int *number_of_angles);

/**
 * Returns the still time status from the next cell
 *
 * \param self Pointer to dvdnav_t associated with this operation.
 */
uint32_t dvdnav_get_next_still_flag(dvdnav_t *self);

/**
 * @}
 */

/** 
 * \defgroup domain Domain Queries
 *
 * The following functions can be used to query whether we are in
 * particular domains or not.
 *
 * @{
 */

/**
 * Are we in the First Play domain. (Menu) 
 * \param self Pointer to dvdnav_t associated with this operation.
 * \returns -1 on failure, 1 if condition is true, 0 if condition is false
 */
int8_t dvdnav_is_domain_fp(dvdnav_t *self);
/**
 * Are we in the Video management Menu domain. (Menu) 
 * \param self Pointer to dvdnav_t associated with this operation.
 * \returns -1 on failure, 1 if condition is true, 0 if condition is false
 */
int8_t dvdnav_is_domain_vmgm(dvdnav_t *self);
/**
 * Are we in the Video Title Menu domain (Menu) 
 * \param self Pointer to dvdnav_t associated with this operation.
 * \returns -1 on failure, 1 if condition is true, 0 if condition is false
 */
int8_t dvdnav_is_domain_vtsm(dvdnav_t *self);
/**
 * Are we in the Video Title domain (playing movie)?
 * \param self Pointer to dvdnav_t associated with this operation.
 * \returns -1 on failure, 1 if condition is true, 0 if condition is false
 */
int8_t dvdnav_is_domain_vts(dvdnav_t *self);

/**
 * @}
 */

/*
 * The following info appears on the front page of the reference manual. It is
 * included here to keep it with the main dvdnav source files. The copyright notice
 * refers to the comments _only_. All machine-readable source code is covered by
 * the copyright notice at the top of this file.
 *
 * Oh, and the official language of the documentation is English -- 
 * English English (as in the English spoken in England) not US English.
 */

#ifdef __cplusplus
}
#endif

#endif /* DVDNAV_H_INCLUDED */
