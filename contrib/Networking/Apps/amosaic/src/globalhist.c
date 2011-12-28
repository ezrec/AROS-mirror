// MDF - PORT FROM NCSA VERSION 2.1

/****************************************************************************
 * NCSA Mosaic for the X Window System                                      *
 * Software Development Group                                               *
 * National Center for Supercomputing Applications                          *
 * University of Illinois at Urbana-Champaign                               *
 * 605 E. Springfield, Champaign IL 61820                                   *
 * mosaic@ncsa.uiuc.edu                                                     *
 *                                                                          *
 * Copyright (C) 1993, Board of Trustees of the University of Illinois      *
 *                                                                          *
 * NCSA Mosaic software, both binary and source (hereafter, Software) is    *
 * copyrighted by The Board of Trustees of the University of Illinois       *
 * (UI), and ownership remains with the UI.                                 *
 *                                                                          *
 * The UI grants you (hereafter, Licensee) a license to use the Software    *
 * for academic, research and internal business purposes only, without a    *
 * fee.  Licensee may distribute the binary and source code (if released)   *
 * to third parties provided that the copyright notice and this statement   *
 * appears on all copies and that no charge is associated with such         *
 * copies.                                                                  *
 *                                                                          *
 * Licensee may make derivative works.  However, if Licensee distributes    *
 * any derivative work based on or derived from the Software, then          *
 * Licensee will (1) notify NCSA regarding its distribution of the          *
 * derivative work, and (2) clearly notify users that such derivative       *
 * work is a modified version and not the original NCSA Mosaic              *
 * distributed by the UI.                                                   *
 *                                                                          *
 * Any Licensee wishing to make commercial use of the Software should       *
 * contact the UI, c/o NCSA, to negotiate an appropriate license for such   *
 * commercial use.  Commercial use includes (1) integration of all or       *
 * part of the source code into a product for sale or license by or on      *
 * behalf of Licensee to third parties, or (2) distribution of the binary   *
 * code or source code to third parties that need it to utilize a           *
 * commercial product sold or licensed by or on behalf of Licensee.         *
 *                                                                          *
 * UI MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THIS SOFTWARE FOR   *
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED          *
 * WARRANTY.  THE UI SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY THE    *
 * USERS OF THIS SOFTWARE.                                                  *
 *                                                                          *
 * By using or copying this Software, Licensee agrees to abide by the       *
 * copyright law and all other applicable laws of the U.S. including, but   *
 * not limited to, export control laws, and the terms of this license.      *
 * UI shall have the right to terminate this license immediately by         *
 * written notice upon Licensee's breach of, or non-compliance with, any    *
 * of its terms.  Licensee may be held legally responsible for any          *
 * copyright infringement that is caused or encouraged by Licensee's        *
 * failure to abide by the terms of this license.                           *
 *                                                                          *
 * Comments and questions are welcome and can be sent to                    *
 * mosaic-x@ncsa.uiuc.edu.                                                  *
 ****************************************************************************/

#include "includes.h"
#include "HTML.h"   /* needed for cache, but should that be here */
#include "mosaic.h"
#include "globals.h"
#include "htmlgad.h"
#include "gui.h"
#include "XtoI.h"
#include "protos.h"
#include <time.h>
#include <unistd.h>

extern char *cached_url;

#define TRACE 0

/* Old compilers choke on const... */
#ifdef __STDC__
#define MO_CONST const
#else
#define MO_CONST
#endif /* not __STDC */

/* ------------------------------------------------------------------------ */
/* ---------------------------- GLOBAL HISTORY ---------------------------- */
/* ------------------------------------------------------------------------ */

/* We save history list out to a file (~/.mosaic-global-history) and
   reload it on entry.

   Initially the history file format will look like this:

   ncsa-mosaic-history-format-1            [identifying string]
   Global                                  [title]
   url Fri Sep 13 00:00:00 1986            [first word is url;
                                            subsequent words are
                                            last-accessed date (GMT)]
   [1-line sequence for single document repeated as necessary]
   ...
*/

#define NCSA_HISTORY_FORMAT_COOKIE_ONE "ncsa-mosaic-history-format-1"

#define HASH_TABLE_SIZE 200

/* Cached data in a hash entry for a given URL; one or both
   slots can be filled; non-filled slots will be NULL. */
typedef struct cached_data
{
  ImageInfo *image_data;
  char *local_name;
  int last_access;
} cached_data;

/* An entry in a hash bucket, containing a URL (in canonical,
   absolute form) and possibly cached info (right now, an ImageInfo
   struct for inlined images). */
typedef struct entry
{
  /* Canonical URL for this document. */
  char *url;
  /* This can be one of a couple of things:
     for an image, it's the ImageInfo struct;
     for an HDF file, it's the local filename (if any) */
  cached_data *cached_data;
  struct entry *next;
} entry;

/* A bucket in the hash table; contains a linked list of entries. */
typedef struct bucket
{
  entry *head;
  int count;
} bucket;

static bucket hash_table[HASH_TABLE_SIZE];

static mo_status mo_cache_image_data (cached_data *cd, void *info);
static mo_status mo_uncache_image_data (cached_data *cd);
static int mo_kbytes_in_image_data (void *image_data);

static int access_counter = 0;
static int dont_nuke_after_me = 0;
static int kbytes_cached = 0;

/* Given a URL, hash it and return the hash value, mod'd by the size
   of the hash table. */
static int hash_url (char *url)
{
  int len, i, val;

  if (!url)
    return 0;
  len = strlen (url);
  val = 0;
  for (i = 0; i < 10; i++)
    val += url[(i * val + 7) % len];

  return val % HASH_TABLE_SIZE;
}

#ifdef DEBUG
/* Each bucket in the hash table maintains a count of the number of
   entries contained within; this function dumps that information
   out to stdout. */
static void dump_bucket_counts (void)
{
  int i;

  for (i = 0; i < HASH_TABLE_SIZE; i++)
    fprintf (stdout, "Bucket %03d, count %03d\n", i, hash_table[i].count);
  
  return;
}
#endif /* DEBUG */

/* Assume url isn't already in the bucket; add it by
   creating a new entry and sticking it at the head of the bucket's
   linked list of entries. */
static void add_url_to_bucket (int buck, char *url)
{
  bucket *bkt = &(hash_table[buck]);
  entry *l;

  l = (entry *)malloc (sizeof (entry));
  l->url = strdup (url);
  l->cached_data = NULL;
  l->next = NULL;
  
  if (bkt->head == NULL)
    bkt->head = l;
  else
    {
      l->next = bkt->head;
      bkt->head = l;
    }

  bkt->count += 1;
}

/* This is the internal predicate that takes a URL, hashes it,
   does a search through the appropriate bucket, and either returns
   1 or 0 depending on whether we've been there. */
static int been_here_before (char *url)
{
  int hash = hash_url (url);
  entry *l;

  if (hash_table[hash].count)
    for (l = hash_table[hash].head; l != NULL; l = l->next)
      {
        if (!strcmp (l->url, url))
          return 1;
      }
  
  return 0;
}


/* ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------ */

/****************************************************************************
 * name:    mo_been_here_before_huh_dad
 * purpose: Predicate to determine if we've visited this URL before.
 * inputs:  
 *   - char *url: The URL.
 * returns: 
 *   mo_succeed if we've been here before; mo_fail otherwise
 * remarks: 
 *   We canonicalize the URL (stripping out the target anchor, 
 *   if one exists).
 ****************************************************************************/
mo_status mo_been_here_before_huh_dad (char *url)
{
  char *curl = mo_url_canonicalize (url, "");
  mo_status status;

  if (been_here_before (curl))
    status = mo_succeed;
  else
    status = mo_fail;

  free (curl);
  return status;
}


/****************************************************************************
 * name:    mo_here_we_are_son
 * purpose: Add a URL to the global history, if it's not already there.
 * inputs:  
 *   - char *url: URL to add.
 * returns: 
 *   mo_succeed
 * remarks: 
 *   We canonicalize the URL (stripping out the target anchor, 
 *   if one exists).
 ****************************************************************************/
mo_status mo_here_we_are_son (char *url)
{
  char *curl = mo_url_canonicalize (url, "");

  if (!been_here_before (curl))
    add_url_to_bucket (hash_url (curl), curl);
  
  free (curl);

  return mo_succeed;
}


/****************************************************************************
 * name:    mo_read_global_history (PRIVATE)
 * purpose: Given a filename, read the file's contents into the
 *          global history hash table.
 * inputs:  
 *   - char *filename: The file to read.
 * returns: 
 *   nothing
 * remarks: 
 *   
 ****************************************************************************/
static void mo_read_global_history (char *filename)
{
  FILE *fp;
  char line[MO_LINE_LENGTH];
  char *status;
  
  fp = fopen (filename, "r");
  if (!fp)
    goto screwed_no_file;
  
  status = fgets (line, MO_LINE_LENGTH, fp);
  if (!status || !(*line))
    goto screwed_with_file;
  
  /* See if it's our format. */
  if (strncmp (line, NCSA_HISTORY_FORMAT_COOKIE_ONE,
               strlen (NCSA_HISTORY_FORMAT_COOKIE_ONE)))
    goto screwed_with_file;

  /* Go fetch the name on the next line. */
  status = fgets (line, MO_LINE_LENGTH, fp);
  if (!status || !(*line))
    goto screwed_with_file;
  
  /* Start grabbing url's. */
  while (1)
    {
      char *url;
      
      status = fgets (line, MO_LINE_LENGTH, fp);
      if (!status || !(*line))
        goto done;
      
      url = strtok (line, " ");
      if (!url)
        goto screwed_with_file;

      /* We don't use the last-accessed date... yet. */
      /* lastdate = strtok (NULL, "\n"); blah blah blah... */

      add_url_to_bucket (hash_url (url), url);
    }
  
 screwed_with_file:
 done:
  fclose (fp);
  return;

 screwed_no_file:
  return;
}



/****************************************************************************
 * name:    mo_init_global_history
 * purpose: Initialize the global history hash table.
 * inputs:  
 *   none
 * returns: 
 *   mo_succeed
 * remarks: 
 *   
 ****************************************************************************/
mo_status mo_init_global_history (void)
{
  int i;

  /* Initialize the history structs. */
  for (i = 0; i < HASH_TABLE_SIZE; i++)
    {
      hash_table[i].count = 0;
      hash_table[i].head = 0;
    }

  return mo_succeed;
}


/****************************************************************************
 * name:    mo_wipe_global_history
 * purpose: Wipe out the current global history.
 * inputs:  
 *   none
 * returns: 
 *   mo_succeed
 * remarks: 
 *   Huge memory hole here.  However, we now call
 *   mo_flush_image_cache to at least clear out the image structures.
 ****************************************************************************/
mo_status mo_wipe_global_history (mo_window *win)
{
  mo_flush_image_cache (win);

  /* Memory leak! @@@ */
  mo_init_global_history ();

  return mo_succeed;
}


/****************************************************************************
 * name:    mo_setup_global_history
 * purpose: Called on program startup to do the global history
 *          initialization stuff, including figuring out where the
 *          global history file is and reading it.
 * inputs:  
 *   none
 * returns: 
 *   mo_succeed
 * remarks: 
 *   
 ****************************************************************************/
static char *cached_global_hist_fname = NULL;
mo_status mo_setup_global_history (void)
{
  char *home = getenv ("HOME");
  char *default_filename = Rdata.global_history_file;
  char *filename;

  mo_init_global_history ();

  /* This shouldn't happen. */
  if (!home)
     home = "t:";
  
  filename = (char *)malloc 
    ((strlen (home) + strlen (default_filename) + 8) * sizeof (char));
//  sprintf (filename, "%s/%s\0", home, default_filename);
  sprintf (filename, "%s\0", default_filename);
  cached_global_hist_fname = filename;

  mo_read_global_history (filename);
  
  return mo_succeed;
}


/****************************************************************************
 * name:    mo_write_global_history
 * purpose: Write the global history file out to disk.
 * inputs:  
 *   none
 * returns: 
 *   mo_succeed (usually)
 * remarks: 
 *   This assigns last-read times to all the entries in the history,
 *   which is a bad thing.
 ****************************************************************************/
mo_status mo_write_global_history (void)
{
  FILE *fp;
  int i;
  entry *l;
  time_t foo = time (NULL);
  char *ts = ctime (&foo);

  ts[strlen(ts)-1] = '\0';
 
  fp = fopen (cached_global_hist_fname, "w");
  if (!fp)
    return mo_fail;

  fprintf (fp, "%s\n%s\n", NCSA_HISTORY_FORMAT_COOKIE_ONE, "Global");
  
  for (i = 0; i < HASH_TABLE_SIZE; i++)
    {
      for (l = hash_table[i].head; l != NULL; l = l->next)
        {
          fprintf (fp, "%s %s\n", l->url, ts);
        }
    }
  
  fclose (fp);
  
  return mo_succeed;
}


/****************************************************************************
 * name:    mo_fetch_cached_image_data
 * purpose: Retrieve a piece of cached data associated with a URL.
 * inputs:  
 *   - char *url: The URL.
 * returns: 
 *   The piece of cached data (void *).
 * remarks: 
 *   We do *not* do anything to the URL.  If there is a target
 *   anchor in it, fine with us.  This means the target anchor
 *   should have been stripped out someplace else if it needed to be.
 ****************************************************************************/
void *mo_fetch_cached_image_data (char *url)
{
  int hash = hash_url (url);
  entry *l;

  if (hash_table[hash].count)
    for (l = hash_table[hash].head; l != NULL; l = l->next)
      {
        if (!strcmp (l->url, url))
          {
            if (l->cached_data && l->cached_data->image_data)
              {
                if (TRACE)
//                  fprintf (stderr, "[mo_fetch_cached_image_data] Hit for '%s', data 0x%08x\n", url, l->cached_data->image_data);
                l->cached_data->last_access = access_counter++;
                return l->cached_data->image_data;
              }
            else
              {
                if (TRACE)
//                  fprintf (stderr, "[mo_fetch_cached_image_data] Miss for '%s'\n",
//                         url);
                return NULL;
              }
          }
      }
  
  return NULL;
}

#ifdef DEBUG
/****************************************************************************
 * name:    mo_fetch_cached_local_name
 * purpose: Retrieve a piece of cached data associated with a URL.
 * inputs:  
 *   - char *url: The URL.
 * returns: 
 *   The piece of cached data (void *).
 * remarks: 
 *   We do *not* do anything to the URL.  If there is a target
 *   anchor in it, fine with us.  This means the target anchor
 *   should have been stripped out someplace else if it needed to be.
 ****************************************************************************/
void *mo_fetch_cached_local_name (char *url)
{
  int hash = hash_url (url);
  entry *l;

  if (hash_table[hash].count)
    for (l = hash_table[hash].head; l != NULL; l = l->next)
      {
        if (!strcmp (l->url, url))
          {
            if (l->cached_data)
              return l->cached_data->local_name;
            else
              return NULL;
          }
      }
  
  return NULL;
}
#endif


/****************************************************************************
 * name:    mo_cache_data
 * purpose: Cache a piece of data associated with a given URL.
 * inputs:  
 *   - char  *url: The URL.
 *   - void *info: The piece of data to cache (currently either
 *                 an ImageInfo struct for an image named as SRC
 *                 in an IMG tag, or the filename corresponding to the
 *                 local copy of a remote HDF file).
 *   - int   type: The type of data to cache (currently either
 *                 0 for an ImageInfo struct or 1 for a local name).
 * returns: 
 *   mo_succeed, unless something goes badly wrong
 * remarks: 
 *   We do *not* do anything to the URL.  If there is a target
 *   anchor in it, fine with us.  This means the target anchor
 *   should have been stripped out someplace else if it needed to be.
 ****************************************************************************/
mo_status mo_cache_data (char *url, void *info, int type)
{
  int hash = hash_url (url);
  entry *l;

  /* First, register ourselves if we're not already registered.
     Now, the same URL can be registered multiple times with different
     (or, in one instance, no) internal anchor. */
  if (!been_here_before (url))
    add_url_to_bucket (hash_url (url), url);

  /* Then, find the right entry. */
  if (hash_table[hash].count)
    for (l = hash_table[hash].head; l != NULL; l = l->next)
      {
        if (!strcmp (l->url, url))
          goto found;
      }
  
  return mo_fail;

 found:
  if (!l->cached_data)
    {
      l->cached_data = (cached_data *)malloc (sizeof (cached_data));
      l->cached_data->image_data = NULL;
      l->cached_data->local_name = NULL;
      l->cached_data->last_access = 0;
    }

  if (type == 0)
    {
      if (TRACE)
        fprintf (stderr, "[mo_cache_data] Caching '%s', data %p\n",
                 url, info);
      mo_cache_image_data (l->cached_data, info);
    }
  else if (type == 1)
    l->cached_data->local_name = info;

  return mo_succeed;
}


mo_status mo_zap_cached_images_here (mo_window *win)
{
  char **hrefs;
  int num;
  void *ptr;
/*  HTMLPart *inst;*/

  /* Go fetch new hrefs. */
/*  get(HTML_gad, HTML_inst, (ULONG *)(&inst)); */
/*  hrefs = HTMLGetImageSrcs (inst, &(num)); */
  hrefs = TO_HTMLGetImageSrcs (win, &(num));

  if (num)
    {
      int i;
      for (i = 0; i < num; i++)
        {
          char *url = mo_url_canonicalize (hrefs[i], cached_url);
          ptr = mo_fetch_cached_image_data (url);
          if (ptr)
            {
              mo_cache_data (url, NULL, 0);
            }
        }

      /* All done; clean up. */
      for (i = 0; i < num; i++)
        free (hrefs[i]);
      free (hrefs);
    }

  return mo_succeed;
}


/****************************************************************************
 * name:    mo_flush_image_cache
 * purpose: 
 * inputs:  
 *   - mo_window *win: The current window.
 * returns: 
 *   nuthin
 * remarks: 
 ****************************************************************************/
mo_status mo_flush_image_cache (mo_window *win)
{
  entry *l;
  int hash;

  for (hash = 0; hash < HASH_TABLE_SIZE; hash++)
    for (l = hash_table[hash].head; l != NULL; l = l->next)
      if (l->cached_data)
        if (l->cached_data->image_data)
          {
            mo_uncache_image_data (l->cached_data);
          }

  return mo_succeed;
}


/* ------------------------------------------------------------------------ */
/* ------------------------- decent image caching ------------------------- */
/* ------------------------------------------------------------------------ */

/* CHUNK_OF_IMAGES determines the initial size of the array of cached
   pointers to image data; if more images must be cached, the array is
   grown with realloc by this amount.  It is good to keep the array as
   small as possible, as it must occasionally be sorted. */
#define CHUNK_OF_IMAGES 10

static cached_data **cached_cd_array = NULL;
static int num_in_cached_cd_array = 0;
static int size_of_cached_cd_array = 0;

static mo_status mo_dump_cached_cd_array (void)
{
  int i;
  if (!cached_cd_array)
    {
      fprintf (stderr, "[mo_dump_cached_cd_array] No array; punting\n");
      return mo_fail;
    }

  fprintf (stderr, "+++++++++++++++++++++++++\n");
  for (i = 0; i < size_of_cached_cd_array; i++)
    {
      if (cached_cd_array[i])
        fprintf (stderr, "  %02d data %p last_access %d\n", i,
                 cached_cd_array[i]->image_data, 
                 cached_cd_array[i]->last_access);
      else
        fprintf (stderr, "  %02d NULL\n", i);
    }
  fprintf (stderr, "---------------------\n");

  return mo_succeed;
}

static mo_status mo_init_cached_cd_array (void)
{
  cached_cd_array = (cached_data **)malloc (sizeof (cached_data *) * 
                                            CHUNK_OF_IMAGES);
  size_of_cached_cd_array += CHUNK_OF_IMAGES;

  if (TRACE)
    fprintf (stderr, "[mo_init] Did it %p -- allocated %d pointers.\n",
             cached_cd_array,
             size_of_cached_cd_array);

  bzero ((char *)cached_cd_array, 
         CHUNK_OF_IMAGES * sizeof (cached_cd_array[0]));

  return mo_succeed;
}


static mo_status mo_grow_cached_cd_array (void)
{
  cached_cd_array = (cached_data **)realloc 
    (cached_cd_array,
     sizeof (cached_data *) * (size_of_cached_cd_array + CHUNK_OF_IMAGES));

  if (TRACE)
    fprintf (stderr, "[grow] cached_cd_array %p, size_of_cached_cd_array 0x%08x, sum %p\n",
             cached_cd_array, size_of_cached_cd_array, 
             cached_cd_array + size_of_cached_cd_array);
  bzero ((char *)(cached_cd_array + size_of_cached_cd_array), 
         CHUNK_OF_IMAGES * sizeof (cached_cd_array[0]));

  size_of_cached_cd_array += CHUNK_OF_IMAGES;

  return mo_succeed;
}

static int mo_sort_cd_for_qsort (MO_CONST void *a1, MO_CONST void *a2)
{
  cached_data **d1 = (cached_data **)a1;
  cached_data **d2 = (cached_data **)a2;

  /* NULL entries will be at end of array -- this may be good,
     or may not be -- hmmmmmm. */
  if (!d1 || !(*d1))
    return 1;
  if (!d2 || !(*d2))
    return -1;

  if (TRACE)
    fprintf (stderr, "sort: hi there! %d %d\n",
             (*d1)->last_access, (*d2)->last_access);

  return ((*d1)->last_access < (*d2)->last_access ? -1 : 1);
}

static mo_status mo_sort_cached_cd_array (void)
{
  if (!cached_cd_array)
    {
      if (TRACE)
        fprintf (stderr, "[mo_sort_cached_cd_array] No array; punting\n");
      return mo_fail;
    }

  if (num_in_cached_cd_array == 0)
    {
      if (TRACE)
        fprintf (stderr, "[mo_sort_cached_cd_array] Num in array 0; punting\n");
      return mo_fail;
    }

  if (TRACE)
    {
      fprintf (stderr, "[mo_sort_cached_cd_array] Sorting %p!\n",
               cached_cd_array);
      mo_dump_cached_cd_array ();
    }
  qsort 
    ((void *)cached_cd_array, size_of_cached_cd_array, 
     sizeof (cached_cd_array[0]), mo_sort_cd_for_qsort);
  if (TRACE)
    mo_dump_cached_cd_array ();
  
  return mo_succeed;
}

static mo_status mo_remove_cd_from_cached_cd_array (cached_data *cd)
{
  int i, freed_kbytes = 0;
  
  if (!cached_cd_array)
    return mo_fail;

  for (i = 0; i < size_of_cached_cd_array; i++)
    {
      if (cached_cd_array[i] == cd)
        {
          if (TRACE)
            fprintf 
              (stderr, 
               "[mo_remove_cd_from_cached_cd_array] Found data %p, location %d\n", 
               cached_cd_array[i]->image_data, i);
          freed_kbytes = mo_kbytes_in_image_data 
            (cached_cd_array[i]->image_data);
          mo_free_image_data (cached_cd_array[i]->image_data);
          cached_cd_array[i] = NULL;
          goto done;
        }
    }

  if (TRACE)
    fprintf (stderr, "[mo_remove_cd] UH OH, DIDN'T FIND IT!!!!!!!!!\n");
  return mo_fail;
  
 done:
  num_in_cached_cd_array--;
  kbytes_cached -= freed_kbytes;
  return mo_succeed;
}

static mo_status mo_add_cd_to_cached_cd_array (cached_data *cd)
{
  int i, num;
  int kbytes_in_new_image = mo_kbytes_in_image_data (cd->image_data);
  
  if (TRACE)
    fprintf (stderr, "[mo_add_cd] New image is %d kbytes.\n",
             kbytes_in_new_image);

  if (!cached_cd_array)
    {
      mo_init_cached_cd_array ();
      if (TRACE)
        fprintf (stderr, "[mo_add_cd] Init'd cached_cd_array.\n");
    }
  else
    {
      /* Maybe it's already in there. */
      for (i = 0; i < size_of_cached_cd_array; i++)
        {
          if (cached_cd_array[i] == cd)
            return mo_succeed;
        }
    }

  /* Here's the magic part. */
  if ((kbytes_cached + kbytes_in_new_image) > Rdata.image_cache_size)
    {
      int num_to_remove = 0;
      if (TRACE)
        fprintf (stderr, "[mo_add_cd] Going to sort %p...\n", 
                 cached_cd_array);
      mo_sort_cached_cd_array ();
      if (TRACE)
        {
          fprintf (stderr, 
                   "[mo_add_to] Just sorted in preparation for purging...\n");
          mo_dump_cached_cd_array ();
        }
      while ((kbytes_cached + kbytes_in_new_image) > Rdata.image_cache_size)
        {
          if (TRACE)
            fprintf(stderr, "[mo_add_cd] Trying to free another image (%d > %d).\n",(kbytes_cached + kbytes_in_new_image), Rdata.image_cache_size);
          /* Try to remove one -- we rely on the fact that NULL
             entries in cached_cd_array are at the end of the array. */
          if (num_to_remove < size_of_cached_cd_array &&
              cached_cd_array[num_to_remove] != NULL)
            {
              if (TRACE)
                fprintf (stderr, "        ** going to try to remove %d; last_access %d < dont_nuke_after_me %d??\n",num_to_remove, cached_cd_array[num_to_remove]->last_access,dont_nuke_after_me);
              if (cached_cd_array[num_to_remove]->last_access <
                  dont_nuke_after_me)
                {
                  if (TRACE)
                    fprintf (stderr, "        ** really removing %d\n",
                             num_to_remove);
                  mo_uncache_image_data (cached_cd_array[num_to_remove]);
                  if (TRACE)
                    mo_dump_cached_cd_array ();
                }
              num_to_remove++;
            }
          else
            {
              if (TRACE)
                fprintf (stderr, "        ** no more to remove\n");
              if (TRACE)
                mo_dump_cached_cd_array ();
              goto removed_em_all;
            }
        }
    }
 removed_em_all:
  
  if (num_in_cached_cd_array == size_of_cached_cd_array)
    {
      if (TRACE)
        fprintf (stderr, "[mo_add_cd] Growing array... \n");
      num = size_of_cached_cd_array;
      mo_grow_cached_cd_array ();
    }
  else
    {
      num = -1;
      for (i = 0; i < size_of_cached_cd_array; i++)
        {
          if (cached_cd_array[i] == NULL)
            {
              num = i;
              goto got_num;
            }
        }
      if (TRACE)
        fprintf 
          (stderr, 
           "[mo_add_cd_to_cached_cd_array] UH OH couldn't find empty slot\n");
      /* Try to grow array -- flow of control should never reach here,
         though. */
      num = size_of_cached_cd_array;
      mo_grow_cached_cd_array ();
    }
  
 got_num:
  cached_cd_array[num] = cd;
  num_in_cached_cd_array++;

  kbytes_cached += kbytes_in_new_image;

  if (TRACE)
    {
      fprintf 
        (stderr, 
         "[mo_add_cd_to_cached_cd_array] Added cd, data %p, num %d\n",
         cd->image_data, num);
      fprintf 
        (stderr,
         "[mo_add_cd_to_cached_cd_array] Now cached %d kbytes.\n", kbytes_cached);
      mo_dump_cached_cd_array ();
    }

  return mo_succeed;
}

static int mo_kbytes_in_image_data (void *image_data)
{
  ImageInfo *img = (ImageInfo *)image_data;
  int bytes, kbytes;

  if (!img)
    return 0;

	bytes = (GetBitMapAttr((struct BitMap *)img->image,BMA_HEIGHT) *
			GetBitMapAttr((struct BitMap *)img->image,BMA_WIDTH) *
			GetBitMapAttr((struct BitMap *)img->image,BMA_DEPTH))/8;

  kbytes = bytes >> 10;

  if (TRACE)
    fprintf (stderr, "[mo_kbytes_in_image_data] bytes %d, kbytes %d\n",
             bytes, kbytes);

  if (kbytes == 0)
    kbytes = 1;
  
  return kbytes;
}

static mo_status mo_cache_image_data (cached_data *cd, void *info)
{
  /* Beeeeeeeeeeeeeeeeee smart! */
  if (Rdata.image_cache_size <= 0)
    Rdata.image_cache_size = 1;

  /* It's possible we'll be getting NULL info here, so we
     should uncache in this case... */
  if (!info)
    mo_uncache_image_data (cd);

  cd->image_data = info;
  cd->last_access = access_counter++;

  mo_add_cd_to_cached_cd_array (cd);

  return mo_succeed;
}

static mo_status mo_uncache_image_data (cached_data *cd)
{
  mo_remove_cd_from_cached_cd_array (cd);

  if (cd->image_data->image) {
//    puts("freeing bm");
//    FreeBitMap(cd->image_data->image); /* DT does this for now... */
  }
  if (cd->image_data->mask_bitmap) FreeBitMap(cd->image_data->mask_bitmap);
  if (cd->image_data->dt) {
//    puts("freeing dt");
    DisposeDTObject(cd->image_data->dt);
  }
  if (cd->image_data->fname) {
    unlink(cd->image_data->fname);
    free(cd->image_data->fname);
  }
  cd->image_data = NULL;

  return mo_succeed;
}

mo_status mo_set_image_cache_nuke_threshold (void)
{
  if (TRACE)
    fprintf (stderr, "[mo_set_nuke_threshold] Setting to %d\n",
             access_counter);
  dont_nuke_after_me = access_counter;
  return mo_succeed;
}
