/*
 *
 * mount.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mount.h"
#include "mpegio.h"
#include "../amigaos/req.h"

#define DCL(from, to) (to - from + 1)

/*
 * All structures below are ISO9660, but they are similar enough to OS9 (CD-i) ones
 * to be used for the limited OS9 parsing required for CD-i MPEG playback.
 */

struct primary_descriptor {
     char type                     [DCL (  1,   1)]; /* 711 */
     char id                       [DCL (  2,   6)];
     char version                  [DCL (  7,   7)]; /* 711 */
     char unused1                  [DCL (  8,   8)];
     char system_id                [DCL (  9,  40)]; /* achars */
     char volume_id                [DCL ( 41,  72)]; /* dchars */
     char unused2                  [DCL ( 73,  80)];
     char volume_space_size        [DCL ( 81,  88)]; /* 733 */
     char escape_sequences         [DCL ( 89, 120)];
     char volume_set_size          [DCL (121, 124)]; /* 723 */
     char volume_sequence_number   [DCL (125, 128)]; /* 723 */
     char logical_block_size       [DCL (129, 132)]; /* 723 */
     char path_table_size          [DCL (133, 140)]; /* 733 */
     char type_l_path_table        [DCL (141, 144)]; /* 731 */
     char opt_type_l_path_table    [DCL (145, 148)]; /* 731 */
     char type_m_path_table        [DCL (149, 152)]; /* 732 */
     char opt_type_m_path_table    [DCL (153, 156)]; /* 732 */
     char root_directory_record    [DCL (157, 190)]; /* 9.1 */
     char volume_set_id            [DCL (191, 318)]; /* dchars */
     char publisher_id             [DCL (319, 446)]; /* achars */
     char preparer_id              [DCL (447, 574)]; /* achars */
     char application_id           [DCL (575, 702)]; /* achars */
     char copyright_file_id        [DCL (703, 739)]; /* 7.5 dchars */
     char abstract_file_id         [DCL (740, 776)]; /* 7.5 dchars */
     char bibliographic_file_id    [DCL (777, 813)]; /* 7.5 dchars */
     char creation_date            [DCL (814, 830)]; /* 8.4.26.1 */
     char modification_date        [DCL (831, 847)]; /* 8.4.26.1 */
     char expiration_date          [DCL (848, 864)]; /* 8.4.26.1 */
     char effective_date           [DCL (865, 881)]; /* 8.4.26.1 */
     char file_structure_version   [DCL (882, 882)]; /* 711 */
     char unused4                  [DCL (883, 883)];
     char application_data         [DCL (884, 1395)];
     char unused5                  [DCL (1396, 2048)];
};

struct path_table {
     unsigned char name_len        [DCL ( 1,  2)]; /* 721 */
     char extent                   [DCL ( 3,  6)]; /* 731 */
     char parent                   [DCL ( 7,  8)]; /* 721 */
     char name                     [0];
};

struct directory_record {
     unsigned char length          [DCL ( 1,  1)]; /* 711 */
     char ext_attr_length          [DCL ( 2,  2)]; /* 711 */
     char extent                   [DCL ( 3, 10)]; /* 733 */
     char size                     [DCL (11, 18)]; /* 733 */
     char date                     [DCL (19, 25)]; /* 7 by 711 */
     char flags                    [DCL (26, 26)];
     char file_unit_size           [DCL (27, 27)]; /* 711 */
     char interleave               [DCL (28, 28)]; /* 711 */
     char volume_sequence_number   [DCL (29, 32)]; /* 723 */
     unsigned char name_len        [DCL (33, 33)]; /* 711 */
     char name                     [0];
};

#define VRS_ISO9660        "CD001"
#define VRS_ISO13346_BEGIN "BEA01"
#define VRS_ISO13346_NSR   "NSR02"
#define VRS_ISO13346_END   "TEA01"

static unsigned long get_long(void *gen_src)
{
  unsigned char *src = gen_src;
  unsigned long val;
  val = (src[0] << 24) | (src[1] << 16) | (src[2] << 8) | src[3];
  return val;
}

int mount_cd(int *lba_start, int *lba_stop)
{
  struct primary_descriptor pd;
  struct path_table *pt, *pt_last;
  struct directory_record *dr;
  int type = MOUNT_ERR, files_found = 0;
  unsigned long path_table_size, path_table_sect;
  unsigned long parent, current, dir_lba = 0;
  unsigned long total, offset, lbn;
  unsigned char *sector_buffer, *ptr;
  int cd_start[10], cd_stop[10];

  sector_buffer = malloc(2048);
  if (sector_buffer == NULL) {
    return MOUNT_ERR;
  }

  /* init, may be removed */
  *lba_start = 0;
  *lba_stop = 0;

  /* read primary descriptor */
  cd_read((unsigned char *)&pd, 16, 1);

  /* CD-i */
  if (strncmp(pd.id, "CD-I ", 5) == 0) {
    if (strncmp(pd.system_id, "CD-RTOS", 7) == 0) {
      type = MOUNT_CDI;
    }
  }

  /* VCD/DVD */
  if (strncmp(pd.id, VRS_ISO9660, 5) == 0) {
    int lba = 17, state = 0;
    type = MOUNT_VCD;
    for (;;) {
      cd_read(sector_buffer, lba, 1);
      lba++;

      if (strncmp(&sector_buffer[1], VRS_ISO9660, 5) == 0) {
        continue;
      }

      if (strncmp(&sector_buffer[1], VRS_ISO13346_BEGIN, 5) == 0) {
        state = 1;
        continue;
      }

      if (strncmp(&sector_buffer[1], VRS_ISO13346_NSR, 5) == 0) {
        if (state == 1) {
          state = 2;
        }
        continue;
      }

      if (strncmp(&sector_buffer[1], VRS_ISO13346_END, 5) == 0) {
        if (state == 2) {
          return MOUNT_DVD;
        }
      }
      break;
    }
  }

  if (type == MOUNT_ERR) {
    return MOUNT_ERR;
  }

  if (type == MOUNT_CDI) {
    req_init("CD-i Track selection");
    req_name(0, "CD-i Track");
  } else {
    req_init("(S)VCD Track selection");
    req_name(0, "(S)VCD Track");
  }

  /* read path table */
  path_table_size = get_long(&pd.path_table_size[4]);
  path_table_sect = (path_table_size + 2047) / 2048;

  pt = (struct path_table *)malloc(path_table_sect * 2048);
  cd_read((unsigned char *)pt, get_long(&pd.type_m_path_table), path_table_sect);

  /* search through the directories */
  pt_last = (struct path_table *)((unsigned char *)pt + path_table_size);
  parent = 1;
  current = 1;

  while (pt < pt_last) {
    if (strncmp(pt->name, "MPEGAV", 6) == 0) {
      dir_lba = get_long(&pt->extent[0]);
      break;
    }

    if (strncmp(pt->name, "MPEG2", 5) == 0) {
      dir_lba = get_long(&pt->extent[0]);
      type = MOUNT_SVD; /* HACK */
      break;
    }

    (unsigned char *)pt += sizeof(struct path_table) + ((pt->name_len[0] - 1) & 0xfffffffe) + 2;
    current++;
  }

  if (dir_lba == 0) {
    return MOUNT_ERR;
  }

  /* parse the directory */
  cd_read(sector_buffer, dir_lba, 1);

  dr = (struct directory_record *)&sector_buffer[0];

  total = get_long(&dr->size[4]);
  lbn = get_long(&dr->extent[4]);
  offset = dr->length[0];

  cd_read(sector_buffer, lbn, 1);
  dr = (struct directory_record *)(&sector_buffer[offset]);

  for (;;) {
    offset += dr->length[0];
    dr = (struct directory_record *)(&sector_buffer[offset]);

    if (dr->length[0] == 0) {
      break;
    }

    if (offset >= total) {
      break;
    }

    if (offset >= 2048) {
      break; /* FIXME: should move on to the next sector */
    }

    ptr = strstr(dr->name, ";");
    if (ptr != NULL) {
      if (ptr[1] != '1') {
        continue; /* FIXME: should handle, not skip, fragmented files */
      }
      *ptr = '\0';
    }

    cd_start[files_found] = get_long(&dr->extent[4]);
    cd_stop[files_found] = cd_start[files_found] + (get_long(&dr->size[4]) / 2048);
    files_found++;
    req_add(0, dr->name);
  }

  if (files_found == 0) {
    return MOUNT_ERR;
  } else {
    int res;
    req_show();
    res = req_get(0);
    *lba_start = cd_start[res];
    *lba_stop = cd_stop[res];
  }

  return type;
}