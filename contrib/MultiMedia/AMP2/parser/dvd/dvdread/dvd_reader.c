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

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "../../main/main.h"

#include "dvdread/dvd_udf.h"
#include "dvdread/dvd_reader.h"
#include "dvdcss/dvd.h"
#include "dvdcss/decss.h"

/***********************************************************************************/
/* reads Logical Block of the disc or image                                        */
/*   lb_number: disc-absolute logical block number                                 */
/*   block_count: number of 2048 byte blocks to read                               */
/*   data: pointer to enough allocated memory                                      */
/*   returns number of read blocks on success, 0 or negative error number on error */
/***********************************************************************************/
extern int UDFReadLB(unsigned long int lb_number, unsigned int block_count, unsigned char *data);

static unsigned char title_key[5], disk_key[2048];
static int dvd_is_encrypted = 0;

extern int find_key(unsigned char *buf, int length, unsigned char *key);

struct dvd_reader_s {
    int css_state; /* 0: don't need keys, 1: need keys, 2: have keys */
};

struct dvd_file_s {
    /* Basic information. */
    dvd_reader_t *dvd;

    /* Information required for an image file. */
    uint32_t lb_start;
    uint32_t seek_pos;

    /* Calculated at open-time, size in blocks. */
    ssize_t filesize;

    /* CSS */
    unsigned char title_key[5];
};

/**
 * Opens a block device of a DVD-ROM file, or an image file, or a directory
 * name for a mounted DVD or HD copy of a DVD.  Returns 0 if we can't get any
 * of those methods to work.
 *
 * If the given file is a block device, or is the mountpoint for a block
 * device, then that device is used for CSS authentication using libdvdcss.
 * If no device is available, then no CSS authentication is performed, 
 * and we hope that the image is decrypted.
 *
 * If the path given is a directory, then the files in that directory may be in
 * any one of these formats:
 *
 *   path/VIDEO_TS/VTS_01_1.VOB
 *   path/video_ts/vts_01_1.vob
 *   path/VTS_01_1.VOB
 *   path/vts_01_1.vob
 */
dvd_reader_t *DVDOpen( const char *path )
{
    dvd_reader_t *dvd;

    debug_printf("DVD Open\n");

    dvd = (dvd_reader_t *) malloc( sizeof( dvd_reader_t ) );
    if( !dvd ) return 0;
    dvd->css_state = 1; /* need keys */

    return dvd;
}

/**
 * Closes and cleans up the DVD reader object.  You must close all open files
 * before calling this function.
 */
void DVDClose( dvd_reader_t *dvd )
{
    debug_printf("DVD Close\n");

    if( dvd ) {
        free( dvd );
        dvd = 0;
    }
}

/**
 */
static dvd_file_t *DVDOpenVOBUDF( dvd_reader_t *dvd, int title, int menu )
{
    char filename[ MAX_UDF_FILE_NAME_LEN ];
    uint32_t start, len;
    dvd_file_t *dvd_file;

    if( title == 0 ) {
        sprintf( filename, "/VIDEO_TS/VIDEO_TS.VOB" );
    } else {
        sprintf( filename, "/VIDEO_TS/VTS_%02d_%d.VOB", title, menu ? 0 : 1 );
    }
    start = UDFFindFile( dvd, filename, &len );
    if( start == 0 ) return 0;

    dvd_file = (dvd_file_t *) malloc( sizeof( dvd_file_t ) );
    if( !dvd_file ) return 0;
    dvd_file->dvd = dvd;
    dvd_file->lb_start = start;
    dvd_file->seek_pos = 0;
    dvd_file->filesize = len / DVD_VIDEO_LB_LEN;

    /* Calculate the complete file size for every file in the VOBS */
    if( !menu ) {
        int cur;

        for( cur = 2; cur < 10; cur++ ) {
            sprintf( filename, "/VIDEO_TS/VTS_%02d_%d.VOB", title, cur );
            if( !UDFFindFile( dvd, filename, &len ) ) break;
            dvd_file->filesize += len / DVD_VIDEO_LB_LEN;
        }
    }

    dvd_is_encrypted = is_dvd_encrypted();

    if (dvd_is_encrypted == 0) {
        dvd->css_state = 0; /* don't need keys */
    }

/* FIXME */

    if (dvd_is_encrypted) {
      unsigned char *buf = NULL, tmp_key[5];
      int frame = 0, i, vobdec_ok = 0;

      dvd_is_encrypted = 0; /* HACK START */

      amp_printf("DVD is encrypted\n");

      /* FIXME: Return values */
      get_disk_key(disk_key);
      get_title_key(title_key, dvd_file->lb_start);

      /* Decrypt the title key */
      css_decrypt_title_key(title_key, disk_key);

      /* VOBDEC engine */
      #define FRAMES_TO_READ 16
      #define NUMBER_OF_READ 320

      debug_printf("VOBDEC engine\n");
      buf = malloc(FRAMES_TO_READ * 2048);
      if (buf == NULL) {
        amp_printf("VOBDEC failed, not enough memory\n");
      } else {
        for (i=0; i<NUMBER_OF_READ; i++) {

          debug_printf("VOBDEC reading\n");
          DVDReadBlocks(dvd_file, frame, FRAMES_TO_READ, buf);

          debug_printf("VOBDEC cracking\n");
          if (find_key(buf, (FRAMES_TO_READ*2048), tmp_key) > 0) {
            debug_printf("VOBDEC key found\n");
            memcpy(title_key, tmp_key, 5);
            vobdec_ok = 1;
            break;
          }
          frame += FRAMES_TO_READ;
        }
      }
      if (vobdec_ok == 0) {
        amp_printf("VOBDEC engine failed, playback will probably be corrupted\n");
      }

      dvd_is_encrypted = 1; /* HACK STOP */

    } else {
      amp_printf("DVD is NOT encrypted\n");
    }

    /* CSS */
    memcpy(dvd_file->title_key, title_key, 5);

#if 0
/* Loop over all titles and call dvdcss_title to crack the keys. */
static int initAllCSSKeys( dvd_reader_t *dvd )
{
    struct timeval all_s, all_e;
    struct timeval t_s, t_e;
    char filename[ MAX_UDF_FILE_NAME_LEN ];
    uint32_t start, len;
    int title;
     
    fprintf( stderr, "\n" );
    fprintf( stderr, "libdvdread: Attempting to retrieve all CSS keys\n" );
    fprintf( stderr, "libdvdread: This can take a _long_ time, "
          "please be patient\n\n" );
     
    gettimeofday(&all_s, NULL);
     
    for( title = 0; title < 100; title++ ) {
     gettimeofday( &t_s, NULL );
     if( title == 0 ) {
         sprintf( filename, "/VIDEO_TS/VIDEO_TS.VOB" );
     } else {
         sprintf( filename, "/VIDEO_TS/VTS_%02d_%d.VOB", title, 0 );
     }
     start = UDFFindFile( dvd, filename, &len );
     if( start != 0 && len != 0 ) {
         /* Perform CSS key cracking for this title. */
         fprintf( stderr, "libdvdread: Get key for %s at 0x%08x\n", 
               filename, start );
         if( DVDinput_title( dvd->dev, (int)start ) < 0 ) {
          fprintf( stderr, "libdvdread: Error cracking CSS key for %s (0x%08x)\n", filename, start);
         }
         gettimeofday( &t_e, NULL );
         fprintf( stderr, "libdvdread: Elapsed time %ld\n",  
               (long int) t_e.tv_sec - t_s.tv_sec );
     }
         
     if( title == 0 ) continue;
         
     gettimeofday( &t_s, NULL );
     sprintf( filename, "/VIDEO_TS/VTS_%02d_%d.VOB", title, 1 );
     start = UDFFindFile( dvd, filename, &len );
     if( start == 0 || len == 0 ) break;
         
     /* Perform CSS key cracking for this title. */
     fprintf( stderr, "libdvdread: Get key for %s at 0x%08x\n", 
           filename, start );
     if( DVDinput_title( dvd->dev, (int)start ) < 0 ) {
         fprintf( stderr, "libdvdread: Error cracking CSS key for %s (0x%08x)!!\n", filename, start);
     }
     gettimeofday( &t_e, NULL );
     fprintf( stderr, "libdvdread: Elapsed time %ld\n",  
           (long int) t_e.tv_sec - t_s.tv_sec );
    }
    title--;
    
    fprintf( stderr, "libdvdread: Found %d VTS's\n", title );
    gettimeofday(&all_e, NULL);
    fprintf( stderr, "libdvdread: Elapsed time %ld\n",  
          (long int) all_e.tv_sec - all_s.tv_sec );
    
    return 0;
}
#endif

    return dvd_file;
}

/**
 * Opens a file on the DVD given the title number and domain.  If the title
 * number is 0, the video manager information is opened
 * (VIDEO_TS.[IFO,BUP,VOB]).  Returns a file structure which may be used for
 * reads, or 0 if the file was not found.
 */
dvd_file_t *DVDOpenFile( dvd_reader_t *dvd, int titlenum, dvd_read_domain_t domain )
{
    char filename[ MAX_UDF_FILE_NAME_LEN ];
    uint32_t start, len;
    dvd_file_t *dvd_file;

    debug_printf("DVD Open File\n");

    switch( domain ) {
    case DVD_READ_INFO_FILE:
        if( titlenum == 0 ) {
            sprintf( filename, "/VIDEO_TS/VIDEO_TS.IFO" );
        } else {
            sprintf( filename, "/VIDEO_TS/VTS_%02i_0.IFO", titlenum );
        }
        break;
    case DVD_READ_INFO_BACKUP_FILE:
        if( titlenum == 0 ) {
            sprintf( filename, "/VIDEO_TS/VIDEO_TS.BUP" );
        } else {
            sprintf( filename, "/VIDEO_TS/VTS_%02i_0.BUP", titlenum );
        }
        break;
    case DVD_READ_MENU_VOBS:
        return DVDOpenVOBUDF( dvd, titlenum, 1 );
        break;
    case DVD_READ_TITLE_VOBS:
        if( titlenum == 0 ) return 0;
        return DVDOpenVOBUDF( dvd, titlenum, 0 );
        break;
    default:
        fprintf( stderr, "libdvdread: Invalid domain for file open.\n" );
        return 0;
    }

    start = UDFFindFile( dvd, filename, &len );
    if( !start ) return 0;

    dvd_file = (dvd_file_t *) malloc( sizeof( dvd_file_t ) );
    if( !dvd_file ) return 0;
    dvd_file->dvd = dvd;
    dvd_file->lb_start = start;
    dvd_file->seek_pos = 0;
    dvd_file->filesize = len / DVD_VIDEO_LB_LEN;

    return dvd_file;
}

/**
 * Closes a file and frees the associated structure.
 */
void DVDCloseFile( dvd_file_t *dvd_file )
{
    debug_printf("DVD Close File\n");

    if( dvd_file ) {
        free( dvd_file );
        dvd_file = 0;
    }
}

/**
 * Reads block_count number of blocks from the file at the given block offset.
 * Returns number of blocks read on success, -1 on error.  This call is only
 * for reading VOB data, and should not be used when reading the IFO files.  
 * When reading from an encrypted drive, blocks are decrypted using libdvdcss 
 * where required.
 */
ssize_t DVDReadBlocks( dvd_file_t *dvd_file, int offset, size_t block_count, unsigned char *data )
{
    int i, ret;

/*
debug_printf("Read blocks: %d (%d), %d\n", dvd_file_pos + offset, offset, block_count);
*/

    ret = UDFReadLB(dvd_file->lb_start + offset, block_count, data);

    /* CSS */
    if (dvd_file->dvd->css_state) {
        for (i=0; i<block_count; i++) {
            css_descramble_sector(data, dvd_file->title_key);
            data += 2048;
        }
    }

/*
    if (dvd_is_encrypted) {
        for (i=0; i<block_count; i++) {
            css_descramble_sector(data, title_key);
            data += 2048;
        }
    }
*/

    return ret;
}

/**
 * offset_set = DVDFileSeek(dvd_file, seek_offset);
 *
 * Seek to the given position in the file.  Returns the resulting position in
 * bytes from the beginning of the file.  The seek position is only used for
 * byte reads from the file, the block read call always reads from the given
 * offset.
 */
int32_t DVDFileSeek( dvd_file_t *dvd_file, int32_t offset )
{
    if( offset > dvd_file->filesize * DVD_VIDEO_LB_LEN ) {
        return -1;
    }
    dvd_file->seek_pos = (uint32_t) offset;
    return offset;
}

/**
 * bytes_read = DVDReadBytes(dvd_file, data, bytes);
 *
 * Reads the given number of bytes from the file.  This call can only be used
 * on the information files, and may not be used for reading from a VOB.  This
 * reads from and increments the currrent seek position for the file.
 */
ssize_t DVDReadBytes( dvd_file_t *dvd_file, void *data, size_t byte_size )
{
    unsigned char *secbuf;
    unsigned int numsec, seek_sector, seek_byte;
    int ret;
    
    seek_sector = dvd_file->seek_pos / DVD_VIDEO_LB_LEN;
    seek_byte   = dvd_file->seek_pos % DVD_VIDEO_LB_LEN;

    numsec = ( ( seek_byte + byte_size ) / DVD_VIDEO_LB_LEN ) + 1;
    secbuf = (unsigned char *) malloc( numsec * DVD_VIDEO_LB_LEN );
    if( !secbuf ) {
     fprintf( stderr, "libdvdread: Can't allocate memory " 
           "for file read!\n" );
        return 0;
    }

    /* IFO files are not encrypted */
    ret = UDFReadLB(dvd_file->lb_start + seek_sector, numsec, secbuf);

    if( ret != (int) numsec ) {
        free( secbuf );
        return ret < 0 ? ret : 0;
    }

    memcpy( data, &(secbuf[ seek_byte ]), byte_size );
    free( secbuf );

    dvd_file->seek_pos += byte_size;
    return byte_size;
}

/* Internal, but used from dvd_udf.c */
int DVDReadBlocksUDFRaw( dvd_reader_t *device, uint32_t lb_number, size_t block_count, unsigned char *data, int encrypted )
{
    int ret;

    /* UDF filesystem is not encrypted */
    ret = UDFReadLB(lb_number, block_count, data);

/*
    if (dvd_is_encrypted) {
        for (i=0; i<block_count; i++) {
            css_descramble_sector(data, title_key);
            data += 2048;
        }
    }
*/

    return ret;
}

/**
 * blocks = DVDFileSize(dvd_file);
 *
 * Returns the file size in blocks.
 */
ssize_t DVDFileSize( dvd_file_t *dvd_file )
{
    return dvd_file->filesize;
}

